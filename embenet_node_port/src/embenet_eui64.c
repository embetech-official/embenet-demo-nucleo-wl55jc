/**
@file
@license   Commercial
@copyright (c) 2023 EMBETECH SP. Z O.O. All rights reserved.
@version   1.1.4417
@purpose   embeNET Node port
@brief     Implementation of EUI64 interface for embeNET Node
*/

#include "embenet_eui64.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <stm32wlxx.h>
#pragma GCC diagnostic pop

#include <string.h>


uint64_t EMBENET_EUI64_Get(void) {
    uint64_t native;
    memcpy(&native, (uint8_t*)UID64_BASE, sizeof(native));
    return native;
}
