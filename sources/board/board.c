#include "board.h"

#include "board_config.h"

#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_tim.h"

static board_evt_cb_t    evt_cb = NULL;
static TIM_HandleTypeDef tim_led;

static void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&tim_led);
}

static void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(BOARD_START_STOP_PIN);
}

static void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef* htim) {}

static void HAL_GPIO_EXTI_IRQHandler(uint16_t pin)
{
    if (pin != BOARD_START_STOP_PIN)
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

static void button_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // Init button gpio
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin   = BOARD_START_STOP_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(BOARD_START_STOP_PORT, &GPIO_InitStruct);

    // init LED gpio
    GPIO_InitStruct.Pin   = BOARD_LED_GREEN_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BOARD_LED_GREEN_PORT, &GPIO_InitStruct);
}

ret_code_t led_tim_init(void)
{
    uint16_t prescalervalue = 0;
    uint32_t tmpvalue       = 0;

    /* TIM4 clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();

    /* Enable the TIM4 global Interrupt */
    HAL_NVIC_SetPriority(TIM4_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);

    /* -----------------------------------------------------------------------
    TIM4 Configuration: Output Compare Timing Mode:
      To get TIM4 counter clock at 550 KHz, the prescaler is computed as follows:
      Prescaler = (TIM4CLK / TIM4 counter clock) - 1
      Prescaler = ((f(APB1) * 2) /550 KHz) - 1

      CC update rate = TIM4 counter clock / CCR_Val = 32.687 Hz
      ==> Toggling frequency = 16.343 Hz
    ----------------------------------------------------------------------- */

    /* Compute the prescaler value */
    tmpvalue       = HAL_RCC_GetPCLK1Freq();
    prescalervalue = (uint16_t)((tmpvalue * 2) / 550000) - 1;

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
    sConfigLed.Pulse        = CCR1Val;
    sConfigLed.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfigLed.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfigLed.OCFastMode   = TIM_OCFAST_ENABLE;
    sConfigLed.OCNIdleState = TIM_OCNIDLESTATE_SET;

    /* Initialize the TIM4 Channel1 with the structure above */
    if (HAL_TIM_OC_ConfigChannel(&tim_led, &sConfigLed, TIM_CHANNEL_1) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }
    return CODE_SUCCESS;
}

ret_code_t clock_init(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);

    /* The voltage scaling allows optimizing the power consumption when the device is
       clocked below the maximum system frequency, to update the voltage scaling value
       regarding system frequency refer to product datasheet.  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM       = 8;
    RCC_OscInitStruct.PLL.PLLN       = 336;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ       = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
       clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                   RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);

    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }

    /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
    if (HAL_GetREVID() == 0x1001)
    {
        /* Enable the Flash prefetch */
        __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
    }
    return CODE_SUCCESS;
}

ret_code_t board_init(void)
{
    ret_code_t err_code;
    button_init();
    err_code = led_tim_init();
    if(err_code != CODE_SUCCESS)
    {
        return err_code;
    }
    return clock_init();
}

void board_set_evt_cb(board_evt_cb_t cb)
{
    evt_cb = cb;
}

void board_button_enable_evt(void)
{
    // Enable and set Button EXTI Interrupt to the lowest priority
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void board_button_disable_evt(void)
{
    // Disable button EXTI Interrupt
    HAL_NVIC_DisableIRQ(EXTI0_IRQn);
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