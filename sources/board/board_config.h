#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_

#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx.h"

// Define recording start/stop pin
#define BOARD_START_STOP_PORT GPIOA
#define BOARD_START_STOP_PIN  GPIO_PIN_0

// Define LED config
#define BOARD_LED_GREEN_PORT  GPIOD
#define BOARD_LED_GREEN_PIN   GPIO_PIN_13

// Define microphone config

// Define sd card connection

#endif // BOARD_CONFIG_H_