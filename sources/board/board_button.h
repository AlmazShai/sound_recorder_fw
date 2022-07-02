#ifndef BOARD_BUTTON_H_
#define BOARD_BUTTON_H_

#include "err_code.h"

#define BUTTON_DEBOUNCE_TIME_MS 100
#define BUTTON_LONG_PRESS_MS    2000

/**
 * @brief Button events types
 *
 */
typedef enum
{
    BOARD_BUTTON_EVT_SHORT_PRESS, // Button short press action
    BOARD_BUTTON_EVT_LONG_PRESS,  // Button long press action
} board_button_evt_t;

/**
 * @brief Button event callback typedef
 *
 */
typedef void (*board_button_evt_cb_t)(board_button_evt_t evt_type);

/**
 * @brief   Initialize button
 *
 * @retval  CODE_SUCCESS      - Button peripheral initalized successfully
 * @retval  CODE_ERR_INTERNAL - SDK error occurred wile initialize
 */
ret_code_t board_button_init(void);

/**
 * @brief   Set button event handler fuction.
 *
 * @param cb  - Pointer to callback function
 */
void board_button_set_evt_cb(board_button_evt_cb_t cb);

/**
 * @brief   Enable the button action event.
 *
 */
void board_button_evt_enable(void);

/**
 * @brief   Disable the button action event.
 *
 */
void board_button_evt_disable(void);

#endif // BOARD_BUTTON_H_
