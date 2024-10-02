/**
@file
@license   Commercial
@copyright (c) 2023 EMBETECH SP. Z O.O. All rights reserved.
@version   1.1.4417
@purpose   embeNET Node port
@brief     Implementation of critical section interface for embeNET Node
*/

#include "embenet_critical_section.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <stm32wlxx.h>
#pragma GCC diagnostic pop

static volatile int      irqNestCounter;
static volatile uint32_t previousIrqState;

void EMBENET_CRITICAL_SECTION_Enter(void) {
    uint32_t irqState = __get_PRIMASK();
    __disable_irq();
    if (0 == irqNestCounter) {
        previousIrqState = irqState;
    }
    ++irqNestCounter;
}


void EMBENET_CRITICAL_SECTION_Exit(void) {
    --irqNestCounter;
    if (irqNestCounter < 0) {
        irqNestCounter = 0;
    }
    if (0 == irqNestCounter && 0 == previousIrqState) {
        __enable_irq();
    }
}
