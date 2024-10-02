/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// Standard C library
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
// embeNET includes
#include "embenet_node.h"
#include "enms_node.h"
// demo services
#include "custom_service.h"
#include "mqttsn_client_service.h"
// board and chip specific header files
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/// Descriptor of the ENMS service (network maintenance and visualization)
EnmsNode enmsNode;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
 * User-defined callback that will be called, when the node joins the network.
 * @param[in] panId Identifier of the Network that node joined
 * @param[in] quickJoinCredentials pointer to the Quick Join Credentials that MAY be stored by the user to facilitate rejoining process.
 */
static void onJoined(EMBENET_PANID panId, const EMBENET_NODE_QuickJoinCredentials *quickJoinCredentials) {
    printf("Joined network with PANID: 0x%04" PRIx16 "\n", panId);

    // Start ENMS Service that provides network-wide telemetry information
    EnmsNodeResult enmsStartStatus = ENMS_NODE_Start(&enmsNode);
    if (ENMS_NODE_RESULT_OK == enmsStartStatus) {
        printf("ENMS service started\n");
    } else {
        printf("ENMS service failed to start with status: %d\n", (int)enmsStartStatus);
    }

#if 1 != IS_ROOT
    // Start exemplary, user-defined custom service
    custom_service_start();
    // Start MQTT-SN demo service
    mqttsn_client_service_start();
#endif
}

/**
 * @brief User-defined callback, that will be called after the node leaves the network
 */
static void onLeft(void) {
    printf("Node has left the network\n");
    // Stop ENMS service
    EnmsNodeResult enmsStopStatus = ENMS_NODE_Stop(&enmsNode);
    if (ENMS_NODE_RESULT_OK == enmsStopStatus) {
        printf("ENMS service stopped\n");
    } else {
        printf("ENMS service failed to stop with status: %d\n", (int)enmsStopStatus);
    }

#if 1 != IS_ROOT
    // Stop exemplary, user-defined custom service
    custom_service_stop();
    // Stop MQTT-SN demo service
    mqttsn_client_service_stop();
#endif

}

/**
 * @brief User-defined callback, that will be called when node tries to join the network
 *
 * NOTE: This callback is included in this demo only for debugging purposes
 */
static void onJoinAttempt(EMBENET_PANID panId, const void *panData, size_t panDataSize) {
    printf("Node is attempting to join the network with PANID 0x%04" PRIx16 "\n", panId);
    printf("Network-wide data (%uB)\n", (unsigned)panDataSize);
}


/**
 * @brief User-defined callback, that will be called when the node receives UDP datagram on closed port
 *
 * NOTE: This callback is included for purely debugging purposes. It should be Never used as a method of reliable data transfer
 */
static void dataOnUregisteredPort(uint16_t port) {
    printf("Got UDP datagram on unregistered port no: %" PRIu16 "\n", port);
}

/**
 * @brief User-defined callback, that will be called when provided quick join credentials become obsolete.
 *
 * If the quick join feature is used, user should delete the stored data and store new data, when onJoined callback will be called again.
 * This demo application does not use the quick join feature.
 */
static void onQuickJoinCredentialsObsolete(void) {
    printf("Quick join credentials became obsolete\n");
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    /* USER CODE BEGIN 2 */
    LOGGER_UART_Init();
    printf("\n"
            "+---------------------------------------------+\n"
            "   embeNET Node demo for nucleo-wl55jc board   \n"
            "+---------------------------------------------+\n");

    // Initialize structure with user-defined event handlers
    EMBENET_NODE_EventHandlers handlers = {
        .onJoined = onJoined,
        .onLeft = onLeft,
        .onJoinAttempt = onJoinAttempt,
        .onQuickJoinCredentialsObsolete = onQuickJoinCredentialsObsolete,
        .onDataOnUnregisteredPort = dataOnUregisteredPort
    };

    // Initialize network stack
    if (EMBENET_RESULT_OK == EMBENET_NODE_Init(&handlers)) {
        printf("embeNET Node initialized\n");
    } else {
        printf("Failed to initialize embeNET Node\n");
    }
    
    // Get hardware ID using 96-bit CPU uid
    uint8_t hardwareId[16] = { 0x00 };
    memcpy(hardwareId, (void const*) UID_BASE, 12);
    // Initialize ENMS service on its default port. You may specify custom Hardware Identifier
    if (ENMS_NODE_RESULT_OK == ENMS_NODE_Init(&enmsNode, ENMS_DEFAULT_PORT, hardwareId, NULL)) {
        printf("ENMS service initialized\n");
    } else {
        printf("Failed to initialize ENMS service!\n");
    }

#if 1 == IS_ROOT
    printf("Acting as root with UID: 0x%x%08x\n", (unsigned)(EMBENET_NODE_GetUID()>>32), (unsigned)(EMBENET_NODE_GetUID()));
    // When the application is built for Root node, start as root instead of joining the network
    EMBENET_NODE_RootStart(NULL, 0);
#else
    printf("Acting as node with UID: 0x%x%08x\n", (unsigned)(EMBENET_NODE_GetUID()>>32), (unsigned)(EMBENET_NODE_GetUID()));

    // Initialize exemplary, user-defined custom service
    custom_service_init();
    // Initialize MQTT-SN service
    mqttsn_client_service_init();

    // Additionally tell the ENMS what services are running
    (void) ENMS_NODE_RegisterService(&enmsNode, "custom", 1);
    (void) ENMS_NODE_RegisterService(&enmsNode, "mqttsn", 1);

    // embeNET network configuration:
    // K1 key, used to authenticate the network node should join and
    // PSK - Node's secret key.
    // Note that the psk value should be preferably stored in secure memory, or be preloaded using custom bootloader.
    EMBENET_NODE_Config config = {
        .k1.val = { 0xc0, 0x8b, 0x76, 0x62, 0x77, 0x09, 0x9e, 0x7d, 0x7e, 0x9c, 0x02, 0x22, 0xf1, 0x68, 0xcc, 0x9e },
        .psk.val = {0x46, 0xd7, 0xdc, 0x94, 0xe8, 0xee, 0x74, 0x96, 0xce, 0xaf, 0x54, 0xa3, 0xab, 0x64, 0xcb, 0xeb },
    };

    printf("Trying to join a network...\n");
    // Make the node join the network
    EMBENET_NODE_Join(&config);
#endif
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        // Periodically call embeNET Node process function
        EMBENET_NODE_Proc();
#if 1 != IS_ROOT
        // When acting as Node, run the MQTT-SN service process
        mqttsn_client_service_proc();
#endif
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
  {
  }

  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSE_EnableTcxo();
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
  }

  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_2, 12, LL_RCC_PLLR_DIV_4);
  LL_RCC_PLL_EnableDomain_SYS();
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
  }

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAHB3Prescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(48000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
 * This hander is called by the embeNET stack when a critical error aborts operation.
 */
__attribute__((noreturn)) void EXPECT_OnAbortHandler(char const *why, char const *file, int line) {
    printf("Program aborted: %s %s:%i\n", why, file, line);
    while(1) {
        ;
    }
    __builtin_unreachable();
}

_ssize_t _write(int file, const void *ptr, size_t len) {
    (void) file; /* Not used, avoid warning */
    LOGGER_UART_Write(ptr, len);
    return len;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
        NVIC_SystemReset();
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
