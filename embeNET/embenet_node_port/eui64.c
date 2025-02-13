/**
 * @file
 * @license   commercial
 * @copyright Embetech sp. z o.o.
 * @version   1.0.0
 * @purpose   embeNET Node port
 * @brief     Implementation of EUI64 interface for embeNET Node
 */

#include "embenet_port/eui64.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <stm32wlxx_hal.h>
#pragma GCC diagnostic pop

#include <string.h>

uint64_t EMBENET_EUI64_Get(void) {
  uint64_t native;
  memcpy(&native, (uint8_t *)UID64_BASE, sizeof(native));
  return native;
}
