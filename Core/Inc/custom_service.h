#ifndef CUSTOM_SERVICE_H_
#define CUSTOM_SERVICE_H_

/**
 * @brief Initializes an exemplary, user-defined, custom service.
 *
 * Opens a UDP socket and initializes a periodic task.
 */
void custom_service_init(void);

/**
 * @brief Starts the exemplary, user-defined, custom service.
 */
void custom_service_start(void);

/**
 * @brief Stops the exemplary, user-defined, custom service.
 */
void custom_service_stop(void);

#endif
