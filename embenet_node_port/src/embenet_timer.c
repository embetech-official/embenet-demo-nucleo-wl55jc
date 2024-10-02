/**
@file
@license   Commercial
@copyright (c) 2023 EMBETECH SP. Z O.O. All rights reserved.
@version   1.1.4417
@purpose   embeNET Node port
@brief     Implementation of Timer interface for the embeNET Node
*/

#include "embenet_timer.h"

#include "embenet_critical_section.h"
#include "port_interrupt_priorities.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <stm32wlxx.h>
#include <stm32wlxx_ll_bus.h>
#include <stm32wlxx_ll_rcc.h>
#include <stm32wlxx_ll_tim.h>
#pragma GCC diagnostic pop


#define GUARD_US (EMBENET_TimeUs)(20)

enum {
    SET_COMPARE_GUARD_US = 20,
    TICKS_PER_S          = 125000U,
    US_PER_TICK          = 8U,
};

static EMBENET_TIMER_CompareCallback callback;
static void*                         callbackContext;

void EMBENET_TIMER_Init(EMBENET_TIMER_CompareCallback compareCallback, void* context) {
    LL_TIM_DeInit(TIM2); // Make sure the timer is deinitialized and reset to default values

    callback        = compareCallback;
    callbackContext = context;

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), INTERRUPT_PRIORITY_TIMER, INTERRUPT_SUBPRIORITY_TIMER));
    NVIC_EnableIRQ(TIM2_IRQn);

    LL_RCC_ClocksTypeDef clocks;
    LL_RCC_GetSystemClocksFreq(&clocks);
    LL_TIM_InitTypeDef TIM_InitStruct = {.CounterMode       = LL_TIM_COUNTERMODE_UP,
                                         .Prescaler         = (uint16_t)(clocks.SYSCLK_Frequency / TICKS_PER_S - 1U),
                                         .ClockDivision     = LL_TIM_CLOCKDIVISION_DIV1,
                                         .Autoreload        = (0xFFFFFFFFU >> 3U),
                                         .RepetitionCounter = 0};
    LL_TIM_Init(TIM2, &TIM_InitStruct);
    LL_TIM_DisableARRPreload(TIM2);
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);

    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {.OCMode = LL_TIM_OCMODE_FROZEN, .OCState = LL_TIM_OCSTATE_DISABLE, .CompareValue = 0, .OCPolarity = LL_TIM_OCPOLARITY_HIGH};
    LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);

    LL_TIM_OC_DisableFast(TIM2, LL_TIM_CHANNEL_CH1);
    LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM2);

    LL_TIM_EnableCounter(TIM2);
}


void EMBENET_TIMER_Deinit(void) {
    LL_TIM_DisableCounter(TIM2);
    LL_TIM_DisableIT_CC1(TIM2);
    NVIC_DisableIRQ(TIM2_IRQn);

    LL_TIM_DeInit(TIM2);
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM2);
}


void EMBENET_TIMER_SetCompare(EMBENET_TimeUs compareValue) {
    EMBENET_CRITICAL_SECTION_Enter();
    EMBENET_TimeUs currentValue = EMBENET_TIMER_ReadCounter();
    LL_TIM_EnableIT_CC1(TIM2);
    LL_TIM_ClearFlag_CC1(TIM2);
    NVIC_ClearPendingIRQ(TIM2_IRQn);
    if ((EMBENET_TimeUs)(compareValue - SET_COMPARE_GUARD_US - currentValue) < (EMBENET_TimeUs)(EMBENET_TIMER_GetMaxCompareDuration())) {
        LL_TIM_OC_SetCompareCH1(TIM2, compareValue / US_PER_TICK);
    } else {
        // compareValue is too close to current value and timer will be late, interrupt is triggered immediately
        LL_TIM_GenerateEvent_CC1(TIM2);
    }
    EMBENET_CRITICAL_SECTION_Exit();
}


EMBENET_TimeUs EMBENET_TIMER_ReadCounter(void) {
    return (LL_TIM_GetCounter(TIM2) * US_PER_TICK);
}


EMBENET_TimeUs EMBENET_TIMER_GetMaxCompareDuration(void) {
    return (EMBENET_TimeUs)0x7FFFFFFFU;
}


void TIM2_IRQHandler(void) {
    if (LL_TIM_IsActiveFlag_CC1(TIM2)) {
        LL_TIM_ClearFlag_CC1(TIM2);
        LL_TIM_DisableIT_CC1(TIM2);

        if (callback != NULL) {
            callback(callbackContext);
        }
    }
}
