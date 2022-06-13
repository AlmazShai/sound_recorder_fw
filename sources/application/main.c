/**
 * @file main.c
 * @author Almaz Shaidullin (almazshai@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-06-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "board.h"
#include "stm32f4xx_hal.h"

#include <stdio.h>

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {}
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

static void board_evt_handler(board_evt_t evt_type)
{
    switch (evt_type)
    {
        case BOARD_EVT_BTN_SHORT_PRESS:
        	printf("Button short press action\n");
            board_led_blink_enable();
            break;
        case BOARD_EVT_BTN_LONG_PRESS:
        	printf("Button long press action\n");
            board_led_blink_disable();
            break;
        default:
            // TODO: handle error
            break;
    }
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    HAL_Init();

    ret_code_t err_code;

    err_code = board_init();
    if (err_code != CODE_SUCCESS)
    {
    	// TODO: handle somehow
    }

    board_set_evt_cb(board_evt_handler);

    board_button_enable_evt();

    printf("Application initialized\n");

    while (1)
    {
    }
    return 0;
}
