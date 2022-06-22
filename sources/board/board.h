#ifndef BOARD_H_
#define BOARD_H_

#include "err_code.h"

#define BUTTON_DEBOUNCE_TIME_MS 100
#define BUTTON_LONG_PRESS_MS    2000

typedef enum
{
    BOARD_EVT_BTN_SHORT_PRESS,
    BOARD_EVT_BTN_LONG_PRESS,
} board_evt_t;

typedef void (*board_evt_cb_t)(board_evt_t evt_type);

ret_code_t board_init(void);

void board_set_evt_cb(board_evt_cb_t cb);

ret_code_t board_button_init();

void board_button_evt_enable(void);

void board_button_evt_disable(void);

ret_code_t board_led_init(void);

void board_led_blink_enable(void);

void board_led_blink_disable(void);

#endif // BOARD_H_
