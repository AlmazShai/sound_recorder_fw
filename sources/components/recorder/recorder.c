#include "recorder.h"

#include "assert.h"
#include "app_config.h"
#include "board_mic.h"
#include "pdm2pcm_glo.h"
#include "rec_storage.h"

#include <stdbool.h>
#include <stdio.h>

#define U16_BYTE_SWAP(A) ((((uint16_t)(A)&0xff00) >> 8) | (((uint16_t)(A)&0x00ff) << 8))

// recorder state
static recorder_state_t state = RECORDER_STATE_IDLE;

// internal buffer params
static uint16_t      pdm_buff[RECORDER_PDM_BUFF_SIZE] = {0};
static uint16_t      pcm_buff[RECORDER_PCM_BUFF_SIZE] = {0};
static volatile bool first_half_ready                 = false;
static volatile bool second_half_ready                = false;
static uint16_t      pcm_samples_n                    = 0;

// PDM filters params
PDM_Filter_Handler_t PDM_FilterHandler;
PDM_Filter_Config_t  PDM_FilterConfig;

static void mic_evt_handler(board_mic_evt_t evt)
{
    switch (evt)
    {
        case BOARD_MIC_EVT_HALF_TR_COMPLETE:
            if (first_half_ready)
            {
                printf("pdm_buff overflow\n");
            }
            first_half_ready = true;
            break;
        case BOARD_MIC_EVT_TR_COMPLETE:
            if (second_half_ready)
            {
                printf("pdm_buff overflow\n");
            }
            second_half_ready = true;
            break;
        case BOARD_MIC_EVT_ERROR:
            printf("Board mic i2s error\n");
            break;
        default:
            break;
    }
}

inline static uint16_t get_dec_factor_config(uint32_t df)
{
    if (df >= 128)
    {
        return PDM_FILTER_DEC_FACTOR_128;
    }
    if (df >= 80)
    {
        return PDM_FILTER_DEC_FACTOR_80;
    }
    if (df >= 64)
    {
        return PDM_FILTER_DEC_FACTOR_64;
    }
    if (df >= 48)
    {
        return PDM_FILTER_DEC_FACTOR_48;
    }
    if (df >= 32)
    {
        return PDM_FILTER_DEC_FACTOR_32;
    }
    if (df >= 24)
    {
        return PDM_FILTER_DEC_FACTOR_24;
    }
    return PDM_FILTER_DEC_FACTOR_16;
}

inline static void pdm_filter_init(void)
{
    assert(APP_MIC_SAMPLING_FREQ_HZ != 0);
    assert(APP_MIC_SAMPLING_FREQ_HZ < BOARD_MIC_I2S_SCK_FREQ_HZ);

    uint32_t err_code;
    uint32_t dec_factor_value = BOARD_MIC_I2S_SCK_FREQ_HZ / APP_MIC_SAMPLING_FREQ_HZ;
    uint16_t dec_factor_cfg   = get_dec_factor_config(dec_factor_value);
    pcm_samples_n             = RECORDER_PDM_BUFF_SIZE * 16 / dec_factor_value;

    /* Init PDM filters */
    PDM_FilterHandler.bit_order        = PDM_FILTER_BIT_ORDER_LSB;
    PDM_FilterHandler.endianness       = PDM_FILTER_ENDIANNESS_LE;
    PDM_FilterHandler.high_pass_tap    = 2104533974; // 2122358088;
    PDM_FilterHandler.out_ptr_channels = 2;
    PDM_FilterHandler.in_ptr_channels  = 1;
    err_code = PDM_Filter_Init((PDM_Filter_Handler_t*)(&PDM_FilterHandler));
    assert(err_code == 0);

    /* PDM lib config phase */
    PDM_FilterConfig.output_samples_number = pcm_samples_n;
    PDM_FilterConfig.mic_gain              = 40;
    PDM_FilterConfig.decimation_factor     = dec_factor_cfg;
    err_code = PDM_Filter_setConfig((PDM_Filter_Handler_t*)&PDM_FilterHandler,
                                    &PDM_FilterConfig);
    assert(err_code == 0);
}

static void pdm_2_pcm_conversion(uint16_t* p_in_pdm, uint16_t* p_out_pcm)
{
    static uint16_t
        tmp_buff[RECORDER_PDM_BUFF_SIZE / 2]; // make static to avoid stack overflow

    uint16_t index;
    uint32_t err_code;

    for (index = 0; index < RECORDER_PDM_BUFF_SIZE / 2; index++)
    {
        tmp_buff[index] = p_in_pdm[index]; // U16_BYTE_SWAP(p_in_pdm[index]);
    }

    /* PDM to PCM filter */
    err_code = PDM_Filter((uint8_t*)tmp_buff, (uint16_t*)p_out_pcm, &PDM_FilterHandler);

    /* Duplicate samples since a single microphone in mounted on STM32F4-Discovery */
    for (index = 0; index < pcm_samples_n; index++)
    {
        p_out_pcm[(index << 1) + 1] = p_out_pcm[index << 1];
    }

    if (err_code != 0)
    {
        printf("PDM_Filter error: %d\n", (int)err_code);
    }
}

ret_code_t recorder_init(void)
{
    ret_code_t           err_code;
    board_mic_puff_cfg_t cfg;

    cfg.p_buff    = pdm_buff;
    cfg.buff_size = RECORDER_PDM_BUFF_SIZE;
    board_mic_set_cb(mic_evt_handler);
    err_code = board_mic_init(&cfg);
    if (err_code != CODE_SUCCESS)
    {
        return err_code;
    }
    pdm_filter_init();

    err_code = rec_storage_init();
    return err_code;
}

ret_code_t recorder_start(void)
{
    ret_code_t err_code;
    first_half_ready  = false;
    second_half_ready = false;

    err_code = rec_storage_start_saving();
    if (err_code != CODE_SUCCESS)
    {
        return err_code;
    }

    err_code = board_mic_start_stream();
    if (err_code != CODE_SUCCESS)
    {
        return err_code;
    }
    state = RECORDER_STATE_RUN;
    return CODE_SUCCESS;
}

ret_code_t recorder_stop(void)
{
    ret_code_t err_code;

    err_code = board_mic_stop_stream();
    if (err_code != CODE_SUCCESS)
    {
        return err_code;
    }
    err_code = rec_storage_stop_saving();
    state = RECORDER_STATE_IDLE;
    return err_code;
}

void recorder_process(void)
{
    if (state == RECORDER_STATE_IDLE)
    {
        return;
    }

    ret_code_t err_code;

    // save first half buffer
    if (first_half_ready)
    {
        pdm_2_pcm_conversion(pdm_buff, pcm_buff);

        err_code = rec_storage_save_samples(pcm_buff, pcm_samples_n * 2);
        assert(err_code == CODE_SUCCESS);

        first_half_ready = false;
    }
    // save second half buffer
    if (second_half_ready)
    {
        pdm_2_pcm_conversion(&pdm_buff[RECORDER_PCM_BUFF_SIZE / 2], pcm_buff);

        err_code = rec_storage_save_samples(pcm_buff, pcm_samples_n * 2);
        assert(err_code == CODE_SUCCESS);

        second_half_ready = false;
    }
}

recorder_state_t recorder_get_state(void)
{
    return state;
}
