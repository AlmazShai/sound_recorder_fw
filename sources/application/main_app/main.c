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
#include "recorder.h"
#include "board_button.h"
#include "board_led.h"

/**
 * @brief Main event types
 *
 */
typedef enum
{
    CONTROL_EVT_NONE,  // No event
    CONTROL_EVT_START, // Start recording event
    CONTROL_EVT_STOP,  // Stop recording event
} control_evt_t;

static control_evt_t ctrl_evt = CONTROL_EVT_NONE;

static void board_evt_handler(board_button_evt_t evt_type)
{
    switch (evt_type)
    {
        case BOARD_BUTTON_EVT_SHORT_PRESS:
            printf("Button short press action\n");
            ctrl_evt = CONTROL_EVT_START;
            break;
        case BOARD_BUTTON_EVT_LONG_PRESS:
            printf("Button long press action\n");
            ctrl_evt = CONTROL_EVT_STOP;
            break;
        default:
            // TODO: handle error
            break;
    }
}

/**
 * @brief Main control events handler
 *
 */
static void main_ctrl(void)
{
    switch (ctrl_evt)
    {
        case CONTROL_EVT_NONE:
            break;
        case CONTROL_EVT_START:
            if (recorder_get_state() == RECORDER_STATE_RUN)
            {
                return;
            }
            if (recorder_start() == CODE_SUCCESS)
            {
                board_led_blink_enable();
            }
            else
            {
                printf("RECORDING START ERROR\n");
            }
            break;
        case CONTROL_EVT_STOP:
            if (recorder_get_state() == RECORDER_STATE_IDLE)
            {
                return;
            }
            if (recorder_stop() == CODE_SUCCESS)
            {
                board_led_blink_disable();
            }
            else
            {
                printf("RECORDING STOP ERRO\n");
            }
            break;
        default:
            break;
    }
    ctrl_evt = CONTROL_EVT_NONE;
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    HAL_Init();

    ret_code_t err_code;

    // initialize board
    err_code = board_init();
    assert(err_code == CODE_SUCCESS);

    // initialize button
    err_code = board_button_init();
    assert(err_code == CODE_SUCCESS);

    // initialize led
    err_code = board_led_init();
    assert(err_code == CODE_SUCCESS);

    // initialize recorder
    err_code = recorder_init();
    assert(err_code == CODE_SUCCESS);

    board_button_set_evt_cb(board_evt_handler);

    board_button_evt_enable();

    printf("Application initialized\n");

    while (1)
    {
        recorder_process();
        main_ctrl();

        // TODO: add cpu sleeping
    }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {}
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
