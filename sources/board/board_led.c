#include "board.h"

#include "stm32f4xx_hal.h"

#include "board_config.h"

static TIM_HandleTypeDef tim_led = {0};

void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&tim_led);
}

static void led_toggle_cb(TIM_HandleTypeDef* htim)
{
    if (htim != &tim_led)
    {
        return;
    }
    HAL_GPIO_TogglePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN);
}

inline static void gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    // init LED gpio
    GPIO_InitStruct.Pin   = BOARD_LED_GREEN_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BOARD_LED_GREEN_PORT, &GPIO_InitStruct);
}

inline static ret_code_t led_tim_init(void)
{
    uint16_t           prescalervalue = 0;
    uint32_t           tmpvalue       = 0;
    TIM_OC_InitTypeDef sConfigLed     = {0};

    /* TIM4 clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();

    /* Enable the TIM4 global Interrupt */
    HAL_NVIC_SetPriority(TIM4_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);

    /* -----------------------------------------------------------------------
    TIM4 Configuration: Output Compare Timing Mode:
      To get TIM4 counter clock at 50 KHz, the prescaler is computed as follows:
      Prescaler = (TIM4CLK / TIM4 counter clock) - 1
      Prescaler = ((f(APB1) * 2) /50 KHz) - 1

      CC update rate = TIM4 counter clock / sConfigLed.Pulse = 1 Hz
      ==> Toggling frequency = 2 Hz
    ----------------------------------------------------------------------- */

    /* Compute the prescaler value */
    tmpvalue       = HAL_RCC_GetPCLK1Freq();
    prescalervalue = (uint16_t)((tmpvalue * 2) / 100000) - 1;

    /* Time base configuration */
    tim_led.Instance           = TIM4;
    tim_led.Init.Period        = 65535;
    tim_led.Init.Prescaler     = prescalervalue;
    tim_led.Init.ClockDivision = 0;
    tim_led.Init.CounterMode   = TIM_COUNTERMODE_UP;
    if (HAL_TIM_OC_Init(&tim_led) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }

    /* Output Compare Timing Mode configuration: Channel1 */
    sConfigLed.OCMode       = TIM_OCMODE_TIMING;
    sConfigLed.OCIdleState  = TIM_OCIDLESTATE_SET;
    sConfigLed.Pulse        = 10000;
    sConfigLed.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfigLed.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfigLed.OCFastMode   = TIM_OCFAST_ENABLE;
    sConfigLed.OCNIdleState = TIM_OCNIDLESTATE_SET;

    /* Initialize the TIM4 Channel1 with the structure above */
    if (HAL_TIM_OC_ConfigChannel(&tim_led, &sConfigLed, TIM_CHANNEL_1) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }

    if (HAL_TIM_RegisterCallback(&tim_led, HAL_TIM_OC_DELAY_ELAPSED_CB_ID,
                                 led_toggle_cb) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }
    return CODE_SUCCESS;
}

ret_code_t board_led_init(void)
{
    gpio_init();
    return led_tim_init();
}

void board_led_blink_enable(void)
{
    HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN, GPIO_PIN_RESET);
    // start timer
    HAL_TIM_OC_Start_IT(&tim_led, TIM_CHANNEL_1);
}

void board_led_blink_disable(void)
{
    // stop timer
    HAL_TIM_OC_Stop_IT(&tim_led, TIM_CHANNEL_1);
    HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN, GPIO_PIN_RESET);
}
