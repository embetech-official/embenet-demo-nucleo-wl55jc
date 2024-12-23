/**
 * @file
 * @license   commercial
 * @copyright Embetech sp. z o.o.
 * @version   1.0.0
 * @purpose   embeNET Node port
 * @brief     Implementation of Random interface for embeNET Node
 */

#include "embenet_port/random.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "radio_driver.h"

#pragma GCC diagnostic pop

uint32_t EMBENET_RANDOM_Get(void) {
  // Get random value from radio transceiver. This function is guaranteed to be invoked AFTER radio initialization
  return SUBGRF_GetRandom();
}
