#include "board_mic.h"

#include "stm32f4xx_hal.h"

#include "board_config.h"

static I2S_HandleTypeDef  i2s;
static DMA_HandleTypeDef  i2s_dma;
static board_mic_evt_cb_t evt_cb    = NULL;
static uint16_t*          p_buff    = NULL;
static uint16_t           buff_size = 0;

void DMA1_Stream3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(i2s.hdmarx);
}

static void handle_evt_cb(board_mic_evt_t evt)
{
    if (evt_cb != NULL)
    {
        evt_cb(evt);
    }
}

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef* hi2s)
{
    if (hi2s->Instance == BOARD_MIC_I2S_INST)
    {
        handle_evt_cb(BOARD_MIC_EVT_ERROR);
    }
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef* hi2s)
{
    if (hi2s->Instance == BOARD_MIC_I2S_INST)
    {
        handle_evt_cb(BOARD_MIC_EVT_TR_COMPLETE);
    }
}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef* hi2s)
{
    if (hi2s->Instance == BOARD_MIC_I2S_INST)
    {
        handle_evt_cb(BOARD_MIC_EVT_HALF_TR_COMPLETE);
    }
}

inline static void gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;

    GPIO_InitStruct.Pin       = BOARD_MIC_I2S_SCK_PIN;
    GPIO_InitStruct.Alternate = BOARD_MIC_I2S_SCK_AF;
    HAL_GPIO_Init(BOARD_MIC_I2S_SCK_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin       = BOARD_MIC_I2S_SD_PIN;
    GPIO_InitStruct.Alternate = BOARD_MIC_I2S_SD_AF;
    HAL_GPIO_Init(BOARD_MIC_I2S_SD_GPIO_PORT, &GPIO_InitStruct);
}

inline static ret_code_t dma_init(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* Configure the i2s_dma handle parameters */
    i2s_dma.Init.Channel             = BOARD_MIC_I2S_DMA_CHANNEL;
    i2s_dma.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    i2s_dma.Init.PeriphInc           = DMA_PINC_DISABLE;
    i2s_dma.Init.MemInc              = DMA_MINC_ENABLE;
    i2s_dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    i2s_dma.Init.MemDataAlignment    = DMA_PDATAALIGN_HALFWORD;
    i2s_dma.Init.Mode                = DMA_CIRCULAR;
    i2s_dma.Init.Priority            = DMA_PRIORITY_HIGH;
    i2s_dma.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    i2s_dma.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    i2s_dma.Init.MemBurst            = DMA_MBURST_SINGLE;
    i2s_dma.Init.PeriphBurst         = DMA_MBURST_SINGLE;

    i2s_dma.Instance = BOARD_MIC_I2S_DMA_STREAM;

    /* Associate the DMA handle */
    __HAL_LINKDMA(&i2s, hdmarx, i2s_dma);

    if (HAL_DMA_Init(&i2s_dma) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }

    HAL_NVIC_SetPriority(BOARD_MIC_I2S_DMA_IRQ, 8, 0);
    HAL_NVIC_EnableIRQ(BOARD_MIC_I2S_DMA_IRQ);
    return CODE_SUCCESS;
}

inline static void i2s_clk_init(void)
{
    RCC_PeriphCLKInitTypeDef rccclkinit;
    /*Enable PLLI2S clock*/
    HAL_RCCEx_GetPeriphCLKConfig(&rccclkinit);

    /* Audio frequency multiple of 8 (8/16/32/48/96/192)*/
    /* PLLI2S_VCO Output = PLLI2S_VCO Input * PLLI2SN = 192 Mhz */
    /* I2SCLK = PLLI2S_VCO Output/PLLI2SR = 192/6 = 32 Mhz */
    rccclkinit.PeriphClockSelection = RCC_PERIPHCLK_I2S;
    rccclkinit.PLLI2S.PLLI2SN       = 192;
    rccclkinit.PLLI2S.PLLI2SR       = 6;
    HAL_RCCEx_PeriphCLKConfig(&rccclkinit);
}

inline static ret_code_t i2s_init(void)
{
    __HAL_RCC_SPI2_CLK_ENABLE();
    /* Initialize the i2s Instance parameter */
    i2s.Instance = BOARD_MIC_I2S_INST;

    /* Disable I2S block */
    __HAL_I2S_DISABLE(&i2s);

    /* I2S peripheral configuration */
    i2s.Init.AudioFreq   = 2 * BOARD_MIC_SAMPLING_FREQ_HZ;
    i2s.Init.ClockSource = I2S_CLOCK_PLL;
    i2s.Init.CPOL        = I2S_CPOL_HIGH;
    i2s.Init.DataFormat  = I2S_DATAFORMAT_16B;
    i2s.Init.MCLKOutput  = I2S_MCLKOUTPUT_DISABLE;
    i2s.Init.Mode        = I2S_MODE_MASTER_RX;
    i2s.Init.Standard    = I2S_STANDARD_LSB;

    /* Initialize the I2S peripheral with the structure above */
    if (HAL_I2S_Init(&i2s) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }
    return CODE_SUCCESS;
}

ret_code_t board_mic_init(board_mic_puff_cfg_t* cfg)
{
    if ((cfg->p_buff == NULL) || (cfg->buff_size == 0))
    {
        return CODE_ERR_INVALID_PARAM;
    }
    p_buff    = cfg->p_buff;
    buff_size = cfg->buff_size;

    gpio_init();

    if (dma_init() != CODE_SUCCESS)
    {
        return CODE_ERR_INTERNAL;
    }
    
    i2s_clk_init();
    if (i2s_init() != CODE_SUCCESS)
    {
        return CODE_ERR_INTERNAL;
    }
    return CODE_SUCCESS;
}

void board_mic_set_cb(board_mic_evt_cb_t cb)
{
    evt_cb = cb;
}

ret_code_t board_mic_start_stream(void)
{
    // check buffer init
    if ((p_buff == NULL) || (buff_size == 0))
    {
        return CODE_ERR_INVALID_STATE;
    }
    HAL_StatusTypeDef ret_code;
    ret_code = HAL_I2S_Receive_DMA(&i2s, p_buff, buff_size);
    if (ret_code != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }
    return CODE_SUCCESS;
}

ret_code_t board_mic_stop_stream(void)
{
    HAL_StatusTypeDef ret_code;

    ret_code = HAL_I2S_DMAStop(&i2s);
    if (ret_code != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }
    return CODE_SUCCESS;
}
