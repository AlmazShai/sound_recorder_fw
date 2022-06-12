#include "button.h"

#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_cortex.h"

static void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(BUTTON_GPIO_PIN);
}

static void HAL_GPIO_EXTI_IRQHandler(uint16_t pin)
{
    if (pin != BUTTON_GPIO_PIN)
    {
        return;
    }

    // check edge type

    // save systick on falling edge

    // check button push duration on rising edge

    // handle evt callback
    if (cb != NULL)
    {
        cb(evt_type);
    }
}

void button_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin   = BUTTON_GPIO_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(BUTTON_GPIO_PORT, &GPIO_InitStruct);
}

void button_set_evt_cb(button_evt_cb_t cb)
{
    evt_cb = cb;
}

void button_enable_evt(void)
{
    // Enable and set Button EXTI Interrupt to the lowest priority
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void button_disable_evt(void)
{
    // Disable button EXTI Interrupt
    HAL_NVIC_DisableIRQ(EXTI0_IRQn);
}