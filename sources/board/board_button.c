#include "board_button.h"

#include "stm32f4xx_hal.h"

#include "board_config.h"
#include <stdbool.h>
#include <stdio.h>

typedef enum
{
    TIM_STATE_IDLE = 0x00,
    TIM_STATE_PUSH_DEBOUNCING,
    TIM_STATE_RELEASE_DEBOUNCING,
    TIM_STATE_LONG_PRESS_DETECTION,
} tim_state_t;

static board_button_evt_cb_t evt_cb     = NULL;
static TIM_HandleTypeDef     tim_button = {0};
static volatile tim_state_t  tim_state  = TIM_STATE_IDLE;

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(BOARD_START_STOP_PIN);
}

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&tim_button);
}

static void handle_cb(board_button_evt_t evt_type)
{
    if (evt_cb != NULL)
    {
        evt_cb(evt_type);
    }
}

static void start_debounce_timer(void)
{
    __HAL_TIM_SET_COUNTER(&tim_button, 0);
    assert_param(HAL_TIM_OC_Start_IT(&tim_button, TIM_CHANNEL_1) == HAL_OK);
}

static void stop_debounce_timer(void)
{
    assert_param(HAL_TIM_OC_Stop_IT(&tim_button, TIM_CHANNEL_1) == HAL_OK);
}

static void start_lond_press_timer(void)
{
    __HAL_TIM_SET_COUNTER(&tim_button, 0);
    assert_param(HAL_TIM_OC_Start_IT(&tim_button, TIM_CHANNEL_2) == HAL_OK);
}

static void stop_long_press_timer(void)
{
    assert_param(HAL_TIM_OC_Stop_IT(&tim_button, TIM_CHANNEL_2) == HAL_OK);
}

static void timeout_handler(TIM_HandleTypeDef* htim)
{
    GPIO_PinState pin_state =
        HAL_GPIO_ReadPin(BOARD_START_STOP_PORT, BOARD_START_STOP_PIN);
    // debounce timeout
    if (tim_state == TIM_STATE_PUSH_DEBOUNCING)
    {
        // stop the button debounce timer
        stop_debounce_timer();
        // if button still pushed, start long press detection timer
        if (pin_state == GPIO_PIN_SET)
        {
            start_lond_press_timer();
            tim_state = TIM_STATE_LONG_PRESS_DETECTION;
        }
        // no reset timer state, push duration less than debounce time
        else
        {
            printf("Button press duration less than debounce time\n");
            start_debounce_timer();
            tim_state = TIM_STATE_RELEASE_DEBOUNCING;
        }
    }
    // button release debouncing timeout
    else if (tim_state == TIM_STATE_RELEASE_DEBOUNCING)
    {
        tim_state = TIM_STATE_IDLE;
        stop_debounce_timer();
    }
    // long press timeout
    else if (tim_state == TIM_STATE_LONG_PRESS_DETECTION)
    {
        // long press action detected
        if (pin_state == GPIO_PIN_SET)
        {
            stop_long_press_timer();
            handle_cb(BOARD_BUTTON_EVT_LONG_PRESS);
            tim_state = TIM_STATE_IDLE;
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if (pin != BOARD_START_STOP_PIN)
    {
        return;
    }
    // debouncing process ongoing
    if ((tim_state == TIM_STATE_PUSH_DEBOUNCING) ||
        (tim_state == TIM_STATE_RELEASE_DEBOUNCING))
    {
        return;
    }

    // check edge type
    GPIO_PinState pin_state =
        HAL_GPIO_ReadPin(BOARD_START_STOP_PORT, BOARD_START_STOP_PIN);
    // start debounce timer on push action
    if ((pin_state == GPIO_PIN_SET) && (tim_state == TIM_STATE_IDLE))
    {
        start_debounce_timer();
        tim_state = TIM_STATE_PUSH_DEBOUNCING;
        printf("Start button debouncing\n");
    }
    // calculate button push duration on button release
    else
    {
        // short button press detected
        if (tim_state == TIM_STATE_LONG_PRESS_DETECTION)
        {
            stop_long_press_timer();
            handle_cb(BOARD_BUTTON_EVT_SHORT_PRESS);
        }
        start_debounce_timer();
        tim_state = TIM_STATE_RELEASE_DEBOUNCING;
    }
}

inline static void gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);

    GPIO_InitStruct.Pin   = BOARD_START_STOP_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(BOARD_START_STOP_PORT, &GPIO_InitStruct);
}

inline static ret_code_t timer_init(void)
{
    TIM_OC_InitTypeDef sConfigLed = {0};

    /* TIM4 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();

    /* Enable the TIM3 global Interrupt */
    HAL_NVIC_SetPriority(TIM3_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);

    /* -----------------------------------------------------------------------
    TIM3 Configuration: Output Compare Timing Mode:
      To get TIM3 counter clock at 10 KHz, the prescaler is computed as follows:
      Prescaler = (TIM3CLK / TIM3 counter clock) - 1
      Prescaler = ((f(APB1) * 2) / 10 KHz) - 1

      TIM3 updating period = 0.1 ms
    ----------------------------------------------------------------------- */

    /* Compute the prescaler value */
    uint32_t tmpvalue       = HAL_RCC_GetPCLK1Freq();
    uint16_t prescalervalue = (uint16_t)((tmpvalue * 2) / 10000) - 1;

    /* Time base configuration */
    tim_button.Instance           = TIM3;
    tim_button.Init.Period        = 65535;
    tim_button.Init.Prescaler     = prescalervalue;
    tim_button.Init.ClockDivision = 0;
    tim_button.Init.CounterMode   = TIM_COUNTERMODE_UP;
    if (HAL_TIM_OC_Init(&tim_button) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }

    /* Output Compare Timing Mode configuration: Channel1 */
    sConfigLed.OCMode       = TIM_OCMODE_TIMING;
    sConfigLed.OCIdleState  = TIM_OCIDLESTATE_SET;
    sConfigLed.Pulse        = BUTTON_DEBOUNCE_TIME_MS * 10;
    sConfigLed.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfigLed.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfigLed.OCFastMode   = TIM_OCFAST_ENABLE;
    sConfigLed.OCNIdleState = TIM_OCNIDLESTATE_SET;

    // Initialize debouncing compare value
    if (HAL_TIM_OC_ConfigChannel(&tim_button, &sConfigLed, TIM_CHANNEL_1) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }

    sConfigLed.Pulse = BUTTON_LONG_PRESS_MS * 10;
    // Initialize long press capture value
    if (HAL_TIM_OC_ConfigChannel(&tim_button, &sConfigLed, TIM_CHANNEL_2) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }

    if (HAL_TIM_RegisterCallback(&tim_button, HAL_TIM_OC_DELAY_ELAPSED_CB_ID,
                                 timeout_handler) != HAL_OK)
    {
        return CODE_ERR_INTERNAL;
    }

    return CODE_SUCCESS;
}

ret_code_t board_button_init(void)
{
    gpio_init();
    return timer_init();
}

void board_button_set_evt_cb(board_button_evt_cb_t cb)
{
    evt_cb = cb;
}

void board_button_evt_enable(void)
{
    // Enable and set Button EXTI Interrupt to the lowest priority
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void board_button_evt_disable(void)
{
    // stop timer
    if ((tim_state == TIM_STATE_PUSH_DEBOUNCING) ||
        (tim_state == TIM_STATE_RELEASE_DEBOUNCING))
    {
        HAL_TIM_OC_Stop_IT(&tim_button, TIM_CHANNEL_1);
    }
    else if (tim_state == TIM_STATE_LONG_PRESS_DETECTION)
    {
        HAL_TIM_OC_Stop_IT(&tim_button, TIM_CHANNEL_2);
    }
    // Disable button EXTI Interrupt
    HAL_NVIC_DisableIRQ(EXTI0_IRQn);
}
