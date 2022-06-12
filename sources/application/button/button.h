#ifndef BUTTON_H_
#define BUTTON_H_

#include "board_config.h"

#define BUTTON_DEBOUNCE_TIME_MS 400
#define BUTTON_LONG_PRESS_S     5

#define BUTTON_GPIO_PIN         BOARD_START_STOP_PIN
#define BUTTON_GPIO_PORT        BOARD_START_STOP_PORT

typedef enum
{
    BUTTON_EVT_SHORT_PRESS,
    BUTTON_EVT_LONG_PRESS,
} button_evt_t;

typedef void (*button_evt_cb_t)(button_evt_t evt_type);

void button_init(void);

void button_set_evt_cb(button_evt_cb_t cb);

void button_enable_evt(void);

void button_disable_evt(void);

#endif // BUTTON_H_