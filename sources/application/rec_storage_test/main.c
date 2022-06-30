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

#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <stdbool.h>

#include "board.h"
#include "board_sd.h"
#include "rec_storage.h"

#define SAMPLE_BUFF_SIZE 124

static uint16_t buff[SAMPLE_BUFF_SIZE];

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    HAL_Init();

    ret_code_t err_code;

    err_code = board_init();
    assert(err_code == CODE_SUCCESS);

    err_code = rec_storage_init();
    assert(err_code == CODE_SUCCESS);

    printf("Rec storage test application start\n");

    err_code = rec_storage_start_saving();
    assert(err_code == CODE_SUCCESS);

    uint16_t sample_value = 0;
    for (size_t i = 0; i < 100; i++)
    {
        // fill up the buffer
        for (size_t i = 0; i < SAMPLE_BUFF_SIZE; i++)
        {
            buff[i] = sample_value;
            sample_value++;
        }
        err_code = rec_storage_save_samples(buff, sizeof(buff));
        assert(err_code == CODE_SUCCESS);
    }

    err_code = rec_storage_stop_saving();
    assert(err_code == CODE_SUCCESS);

    while (1)
    {}
    return -1;
}

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
    printf("Wrong parameters value: file %s on line %d\r\n", file, (int)line);
}
#endif /* USE_FULL_ASSERT */
