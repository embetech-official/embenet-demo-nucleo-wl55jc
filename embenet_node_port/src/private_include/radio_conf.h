#ifndef RADIO_CONF_H_
#define RADIO_CONF_H_
#pragma once


#ifdef __cplusplus
extern "C" {
#endif



#include "embenet_node_stm32wle5_bsp_interface.h"

#include <string.h>

typedef enum {
    RBI_SWITCH_OFF    = EMBENET_NODE_BSP_RF_SWITCH_OFF,
    RBI_SWITCH_RX     = EMBENET_NODE_BSP_RF_SWITCH_RX,
    RBI_SWITCH_RFO_LP = EMBENET_NODE_BSP_RF_SWITCH_TX_LP,
    RBI_SWITCH_RFO_HP = EMBENET_NODE_BSP_RF_SWITCH_TX_HP,
} RBI_Switch_TypeDef;


#define RBI_CONF_RFO_LP_HP EMBENET_NODE_BSP_TX_CONFIG_LP_HP
#define RBI_CONF_RFO_LP    EMBENET_NODE_BSP_TX_CONFIG_LP
#define RBI_CONF_RFO_HP    EMBENET_NODE_BSP_TX_CONFIG_HP

#define RBI_Init()                 EMBENET_NODE_BSP_RBI_Init()
#define RBI_DeInit()               EMBENET_NODE_BSP_RBI_Deinit()
#define RBI_GetTxConfig()          EMBENET_NODE_BSP_GetTxConfig()
#define RBI_ConfigRFSwitch(config) EMBENET_NODE_BSP_ConfigRFSwitch(config)
#define RBI_IsTCXO()               (EMBENET_NODE_BSP_HasTCXO)
#define RBI_IsDCDC()               (EMBENET_NODE_BSP_HasDCDC)


/**
 * @brief drive value used anytime radio is NOT in TX low power mode
 * @note override the default configuration of radio_driver.c
 */
#define SMPS_DRIVE_SETTING_DEFAULT SMPS_DRV_40

/**
 * @brief drive value used anytime radio is in TX low power mode
 *        TX low power mode is the worst case because the PA sinks from SMPS
 *        while in high power mode, current is sunk directly from the battery
 * @note override the default configuration of radio_driver.c
 */
#define SMPS_DRIVE_SETTING_MAX SMPS_DRV_60


/**
 * @brief Provides the frequency of the chip running on the radio and the frequency step
 * @remark These defines are used for computing the frequency divider to set the RF frequency
 * @note override the default configuration of radio_driver.c
 */
#define XTAL_FREQ (EMBENET_NODE_BSP_HSEFreq)

/**
 * @brief in XO mode, set internal capacitor (from 0x00 to 0x2F starting 11.2pF with 0.47pF steps)
 * @note override the default configuration of radio_driver.c
 */
#define XTAL_DEFAULT_CAP_VALUE (EMBENET_NODE_BSP_XOCapacitanceValue)

/**
 * @brief voltage of vdd tcxo.
 * @note override the default configuration of radio_driver.c
 */
#define TCXO_CTRL_VOLTAGE (EMBENET_NODE_BSP_TCXOSupplyVoltage)

/**
 * @brief Radio maximum wakeup time (in ms)
 * @note override the default configuration of radio_driver.c
 */
#define RF_WAKEUP_TIME (1UL)

/**
 * @brief DCDC is enabled
 * @remark this define is only used if the DCDC is present on the board
 * @note override the default configuration of radio_driver.c
 */
#define DCDC_ENABLE (1UL)


#ifndef CRITICAL_SECTION_BEGIN
/**
 * @brief macro used to enter the critical section
 */
#    define CRITICAL_SECTION_BEGIN()
#endif

#ifndef CRITICAL_SECTION_END
/**
 * @brief macro used to exit the critical section
 */
#    define CRITICAL_SECTION_END()
#endif

/* Function mapping */
/**
 * @brief SUBGHZ interface init to radio Middleware
 */
#define RADIO_INIT() 

/**
 * @brief Memset utilities interface to radio Middleware
 */
#define RADIO_MEMSET8(dest, value, size) memset(dest, value, size)

/**
 * @brief Memcpy utilities interface to radio Middleware
 */
#define RADIO_MEMCPY8(dest, src, size) memcpy(dest, src, size)

#ifdef __cplusplus
}
#endif

#endif