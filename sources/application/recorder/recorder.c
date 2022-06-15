#include "recorder.h"

#include "board_mic.h"

#include <stdbool.h>
#include <stdio.h>


static uint16_t         pdm_buff[RECORDER_BUFF_SIZE];
static volatile bool    first_half_ready  = false;
static volatile bool    second_half_ready = false;
static recorder_state_t state             = RECORDER_STATE_IDLE;

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

ret_code_t recorder_init(void)
{
    ret_code_t err_code;
    board_mic_puff_cfg_t cfg;
    cfg.p_buff = pdm_buff;
    cfg.buff_size = RECORDER_BUFF_SIZE;
    board_mic_set_cb(mic_evt_handler);
    err_code = board_mic_init(&cfg);
    if (err_code != CODE_SUCCESS)
    {
        return err_code;
    }
    return CODE_SUCCESS;
}

ret_code_t recorder_start(void)
{
    ret_code_t err_code;
    first_half_ready  = false;
    second_half_ready = false;
    err_code          = board_mic_start_stream();
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
    state = RECORDER_STATE_IDLE;
    return CODE_SUCCESS;
}

void recorder_process(void)
{
    if (state == RECORDER_STATE_IDLE)
    {
        return;
    }

    if (first_half_ready)
    {
        first_half_ready = false;
    }
    if (second_half_ready)
    {
        second_half_ready = false;
    }
}
