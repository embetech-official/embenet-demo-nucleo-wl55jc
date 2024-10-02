#ifndef EMBENET_NODE_STM32WLE5_BSP_INTERFACE_H_
#define EMBENET_NODE_STM32WLE5_BSP_INTERFACE_H_
#pragma once


#include <stdbool.h>
#include <stdint.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <stm32wlxx.h>
#include <stm32wlxx_hal_subghz.h>
#pragma GCC diagnostic pop

/**
 * @brief UART callback type
 * @param[in] context optional context that will be passed to the callbacks upon calling
 */
typedef void (*EMBENET_NODE_BSP_UART_Callback_t)(void* context);

/**
 * @brief Initializes UART (GPIO, clocks, peripherals, etc.).
 * @param[in] txEndCallback Callback to be called on finished transmission
 * @param[in] rxCb Callback to be called on reception
 * @param[in] context optional context that will be passed to the callbacks upon calling
 */
void EMBENET_NODE_BSP_UART_Init(const EMBENET_NODE_BSP_UART_Callback_t txEndCallback, const EMBENET_NODE_BSP_UART_Callback_t rxCallback, void* context);


/// @brief Deinitializes UART
void EMBENET_NODE_BSP_UART_Deinit(void);

/**
 * @brief Writes byte to TX register. Does not wait if register is already occupied
 * @param[in] c character to write
 */
void EMBENET_NODE_BSP_UART_WriteByte(const uint8_t c);


/// @brief Reads byte from RX register. Does not wait for data.
uint8_t EMBENET_NODE_BSP_UART_ReadByte(void);


extern SUBGHZ_HandleTypeDef hsubghz; ///< Radio descriptor is defined in embenet_radio.c

/// @brief  RF Switch position
typedef enum {
    EMBENET_NODE_BSP_RF_SWITCH_OFF   = 0, ///< RF Switch in OFF position
    EMBENET_NODE_BSP_RF_SWITCH_RX    = 1, ///< RF Switch in RX position
    EMBENET_NODE_BSP_RF_SWITCH_TX_LP = 2, ///< RF Switch in TX position with low power path
    EMBENET_NODE_BSP_RF_SWITCH_TX_HP = 3, ///< RF Switch in TX position with high power path
} EMBENET_NODE_BSP_RfSwitch;


/// @brief Available RF Tx Configuration
typedef enum {
    EMBENET_NODE_BSP_TX_CONFIG_LP    = 0, ///< Only low power TX path available
    EMBENET_NODE_BSP_TX_CONFIG_HP    = 1, ///< Only high power TX path available
    EMBENET_NODE_BSP_TX_CONFIG_LP_HP = 2, ///< both high- and low power TX paths available
} EMBENET_NODE_BSP_TxConfig;


/// @brief Initialize RF Switch
void EMBENET_NODE_BSP_RBI_Init(void);


/// @brief Deinitialize RF Switch
void EMBENET_NODE_BSP_RBI_Deinit(void);


/// @brief Return TX offset for given RF Path
int8_t EMBENET_NODE_BSP_GetTxOffset(void);

/**
 * @brief Set RF Switch to given position
 * @param  pos: Specifies the Radio RF switch path to be set.
 *         This parameter can be one of following parameters:
 *           @arg EMBENET_NODE_BSP_RF_SWITCH_OFF
 *           @arg EMBENET_NODE_BSP_RF_SWITCH_RX
 *           @arg EMBENET_NODE_BSP_RF_SWITCH_TX_LP
 *           @arg EMBENET_NODE_BSP_RF_SWITCH_TX_HP
 */
void EMBENET_NODE_BSP_ConfigRFSwitch(EMBENET_NODE_BSP_RfSwitch pos);

/**
 * @brief  Return Board available TX Path
 * @retval EMBENET_NODE_BSP_TX_CONFIG_LP_HP
 * @retval EMBENET_NODE_BSP_TX_CONFIG_LP
 * @retval EMBENET_NODE_BSP_TX_CONFIG_HP
 */
EMBENET_NODE_BSP_TxConfig EMBENET_NODE_BSP_GetTxConfig(void);


typedef enum {
    EMBENET_NODE_BSP_TCXO_VOLTAGE_1_6V = 0x00U,
    EMBENET_NODE_BSP_TCXO_VOLTAGE_1_7V = 0x01U,
    EMBENET_NODE_BSP_TCXO_VOLTAGE_1_8V = 0x02U,
    EMBENET_NODE_BSP_TCXO_VOLTAGE_2_2V = 0x03U,
    EMBENET_NODE_BSP_TCXO_VOLTAGE_2_4V = 0x04U,
    EMBENET_NODE_BSP_TCXO_VOLTAGE_2_7V = 0x05U,
    EMBENET_NODE_BSP_TCXO_VOLTAGE_3_0V = 0x06U,
    EMBENET_NODE_BSP_TCXO_VOLTAGE_3_3V = 0x07U,
} EMBENET_NODE_BSP_TCXOVoltage;


extern uint32_t const EMBENET_NODE_BSP_HSEFreq; ///< @brief Shall contain Board HSE frequency in Hz

/** @brief Inidicates default value of trimming capacitor in steps of 0.47 pF added to the minimal value:
 *
 * 0x00 sets the trimming cap to 11.3 pF (minimum)
 * 0x2F sets the trimming cap to 33.4 pF (maximum)
 */
extern uint8_t const EMBENET_NODE_BSP_XOCapacitanceValue;

extern bool const                         EMBENET_NODE_BSP_HasTCXO;           ///< Informs whether TCXO is used in this board
extern EMBENET_NODE_BSP_TCXOVoltage const EMBENET_NODE_BSP_TCXOSupplyVoltage; //< @brief When TCXO is used, and is driven via dedicated pin, selects supply voltage
extern bool const                         EMBENET_NODE_BSP_HasDCDC;           ///< Informs whether DCDC converter is present in this board

#endif