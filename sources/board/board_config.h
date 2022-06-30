#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_

#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx.h"

// Define recording start/stop pin
#define BOARD_START_STOP_PORT       GPIOA
#define BOARD_START_STOP_PIN        GPIO_PIN_0

// Define LED config
#define BOARD_LED_GREEN_PORT        GPIOD
#define BOARD_LED_GREEN_PIN         GPIO_PIN_13

// Define microphone config
#define BOARD_MIC_I2S_INST          SPI2
#define BOARD_MIC_I2S_SCK_PIN       GPIO_PIN_10
#define BOARD_MIC_I2S_SCK_GPIO_PORT GPIOB
#define BOARD_MIC_I2S_SCK_AF        GPIO_AF5_SPI2

#define BOARD_MIC_I2S_SD_PIN        GPIO_PIN_3
#define BOARD_MIC_I2S_SD_GPIO_PORT  GPIOC
#define BOARD_MIC_I2S_SD_AF         GPIO_AF5_SPI2

#define BOARD_MIC_I2S_DMA_STREAM    DMA1_Stream3
#define BOARD_MIC_I2S_DMA_CHANNEL   DMA_CHANNEL_0
#define BOARD_MIC_I2S_DMA_IRQ       DMA1_Stream3_IRQn

// Define sd card connection
#define BOARD_SD_CMD_PIN            GPIO_PIN_2
#define BOARD_SD_CMD_PORT           GPIOD
#define BOARD_SD_CK_PIN             GPIO_PIN_12
#define BOARD_SD_D0_PIN             GPIO_PIN_8
#define BOARD_SD_D1_PIN             GPIO_PIN_9
#define BOARD_SD_D2_PIN             GPIO_PIN_10
#define BOARD_SD_D3_PIN             GPIO_PIN_11
#define BOARD_SD_DX_PORT            GPIOC

#endif // BOARD_CONFIG_H_
