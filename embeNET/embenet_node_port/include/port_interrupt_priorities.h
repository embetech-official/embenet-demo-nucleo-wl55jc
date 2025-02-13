/**
 * @file
 * @license   commercial
 * @copyright Embetech sp. z o.o.
 * @version   1.0.0
 * @purpose   embeNET Node port
 * @brief     Interrupt priorities
 */

#ifndef PORT_INTERRUPT_PRIORITIES_H_
#define PORT_INTERRUPT_PRIORITIES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Interrupt priority order.
 */
enum {
  INTERRUPT_PRIORITY_RADIO = 3,
  INTERRUPT_PRIORITY_TIMER = 3,
};

/**
 * @brief Interrupt subpriority order.
 */
enum {
  INTERRUPT_SUBPRIORITY_RADIO = 0,
  INTERRUPT_SUBPRIORITY_TIMER = 1,
};

#ifdef __cplusplus
}
#endif

#endif /* PORT_INTERRUPT_PRIORITIES_H_ */
