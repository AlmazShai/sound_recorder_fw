#include "board_sd.h"

#include "stm32f4xx_hal.h"

#include "board_config.h"

static SD_HandleTypeDef hsd;

static void gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitStruct.Pin = BOARD_SD_CK_PIN | BOARD_SD_D0_PIN | BOARD_SD_D1_PIN |
                          BOARD_SD_D2_PIN | BOARD_SD_D3_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(BOARD_SD_DX_PORT, &GPIO_InitStruct);


    GPIO_InitStruct.Pin       = BOARD_SD_CMD_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(BOARD_SD_CMD_PORT, &GPIO_InitStruct);
}

static ret_code_t sdio_init(void)
{

    __HAL_RCC_SDIO_CLK_ENABLE();


    hsd.Instance                 = SDIO;
    hsd.Init.ClockEdge           = SDIO_CLOCK_EDGE_RISING;
    hsd.Init.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;
    hsd.Init.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE;
    hsd.Init.BusWide             = SDIO_BUS_WIDE_1B;
    hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    hsd.Init.ClockDiv            = 4;

    /* HAL SD initialization */
    if (HAL_SD_Init(&hsd) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }

    /* Configure SD Bus width (4 bits mode selected) */
    /* Enable wide operation */
    if (HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }
    return CODE_SUCCESS;
}

static ret_code_t wait_while_busy(void)
{
	uint16_t timeout_tick = 0;
    while (HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER)
    {
        HAL_Delay(1);
        timeout_tick++;
        if (timeout_tick > BOARD_SD_OP_TIMEOUT_MS)
        {
            return CODE_ERR_TIMEOUT;
        }
    }
    return CODE_SUCCESS;
}

ret_code_t board_sd_init(void)
{
    gpio_init();

    return sdio_init();
}

ret_code_t board_sd_read_block(uint32_t* p_data, uint32_t read_addr, uint32_t block_n)
{
    if (HAL_SD_ReadBlocks(&hsd, (uint8_t*)p_data, read_addr, block_n,
                          BOARD_SD_OP_TIMEOUT_MS) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }

    return wait_while_busy();
}

ret_code_t board_sd_write_block(uint32_t* p_data, uint32_t write_addr, uint32_t block_n)
{
    if (HAL_SD_WriteBlocks(&hsd, (uint8_t*)p_data, write_addr, block_n,
                           BOARD_SD_OP_TIMEOUT_MS) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }

    return wait_while_busy();
}

ret_code_t board_sd_erase(uint32_t start_addr, uint32_t end_addr)
{
    if (HAL_SD_Erase(&hsd, start_addr, end_addr) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }

    return wait_while_busy();
}

ret_code_t board_sd_erase_all(void)
{
    HAL_SD_CardInfoTypeDef sd_info = {0};
    if (HAL_SD_GetCardInfo(&hsd, &sd_info) != CODE_SUCCESS)
    {
        return CODE_ERR_INTERNAL;
    }
    return board_sd_erase(0, sd_info.LogBlockNbr);
}

bool board_sd_is_busy(void)
{
    return (HAL_SD_GetCardState(&hsd) == HAL_SD_CARD_TRANSFER) ? true : false;
}

ret_code_t board_sd_get_info(board_sd_info_t* p_info)
{
    HAL_SD_CardInfoTypeDef sd_info = {0};
    if (HAL_SD_GetCardInfo(&hsd, &sd_info) != CODE_SUCCESS)
    {
        return CODE_ERR_INTERNAL;
    }
    p_info->log_block_size = sd_info.LogBlockSize;
    p_info->log_block_n    = sd_info.LogBlockNbr;
    return CODE_SUCCESS;
}
