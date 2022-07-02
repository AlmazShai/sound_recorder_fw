#ifndef BOARD_LED_H_
#define BOARD_LED_H_

#include "err_code.h"

/**
 * @brief   This function initialize led and internal timer initialize.
 *
 * @retval  CODE_SUCCESS      - Success
 * @retval  CODE_ERR_INTERNAL - SDK error
 */
ret_code_t board_led_init(void);

/**
 * @brief   Start the led blinking.
 *
 */
void board_led_blink_enable(void);

/**
 * @brief   Stop the led blinking.
 *
 */
void board_led_blink_disable(void);

#endif // BOARD_LED_H_
