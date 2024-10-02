/**
@file
@license   Commercial
@copyright (c) 2023 EMBETECH SP. Z O.O. All rights reserved.
@version   1.1.4417
@purpose   embeNET Node port
@brief     Implementation of AES-128 interface for embeNET Node
*/

#include "embenet_aes128.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <stm32wlxx_hal.h>
#include <stm32wlxx_hal_cryp.h>
#include <stm32wlxx_hal_rcc.h>
#pragma GCC diagnostic pop


#include <stdlib.h>
#include <string.h>

static CRYP_HandleTypeDef hcryp;
static uint32_t           aesKey[4];

/**
 * @brief CRYP MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hcryp: CRYP handle pointer
 * @retval None
 */
void HAL_CRYP_MspInit(CRYP_HandleTypeDef* hcryp) {
    if (hcryp->Instance == AES) {
        /* USER CODE BEGIN AES_MspInit 0 */

        /* USER CODE END AES_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_AES_CLK_ENABLE();
        /* AES interrupt Init */
        HAL_NVIC_SetPriority(AES_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(AES_IRQn);
        /* USER CODE BEGIN AES_MspInit 1 */
        /* USER CODE END AES_MspInit 1 */
    }
}

/**
 * @brief CRYP MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hcryp: CRYP handle pointer
 * @retval None
 */
void HAL_CRYP_MspDeInit(CRYP_HandleTypeDef* hcryp) {
    if (hcryp->Instance == AES) {
        /* USER CODE BEGIN AES_MspDeInit 0 */

        /* USER CODE END AES_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_AES_CLK_DISABLE();

        /* AES interrupt DeInit */
        HAL_NVIC_DisableIRQ(AES_IRQn);
        /* USER CODE BEGIN AES_MspDeInit 1 */
        /* Disable CRYP Interrupt */
        /* USER CODE END AES_MspDeInit 1 */
    }
}

void EMBENET_AES128_Init(void) {
    hcryp.Instance             = AES;
    hcryp.Init.DataType        = CRYP_DATATYPE_8B;
    hcryp.Init.KeySize         = CRYP_KEYSIZE_128B;
    hcryp.Init.pKey            = aesKey;
    hcryp.Init.pInitVect       = NULL;
    hcryp.Init.Algorithm       = CRYP_AES_ECB;
    hcryp.Init.DataWidthUnit   = CRYP_DATAWIDTHUNIT_BYTE;
    hcryp.Init.HeaderWidthUnit = CRYP_HEADERWIDTHUNIT_BYTE;
    hcryp.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ALWAYS;

    HAL_CRYP_DeInit(&hcryp);
    HAL_CRYP_Init(&hcryp);
}

void EMBENET_AES128_Deinit(void) {
    HAL_CRYP_DeInit(&hcryp);
}

void EMBENET_AES128_SetKey(uint8_t const key[16U]) {
    // HAL obtains key in native endianess (as 4 4B words)
    aesKey[0] = ((uint32_t)key[0] << 24) + ((uint32_t)key[1] << 16) + ((uint32_t)key[2] << 8) + ((uint32_t)key[3]);
    aesKey[1] = ((uint32_t)key[4] << 24) + ((uint32_t)key[5] << 16) + ((uint32_t)key[6] << 8) + ((uint32_t)key[7]);
    aesKey[2] = ((uint32_t)key[8] << 24) + ((uint32_t)key[9] << 16) + ((uint32_t)key[10] << 8) + ((uint32_t)key[11]);
    aesKey[3] = ((uint32_t)key[12] << 24) + ((uint32_t)key[13] << 16) + ((uint32_t)key[14] << 8) + ((uint32_t)key[15]);
}

void EMBENET_AES128_Encrypt(uint8_t data[16U]) {
    uint32_t in[4];
    memcpy(in, data, 16);
    uint32_t out[4];
    HAL_CRYP_Encrypt(&hcryp, in, (uint16_t)16, out, 10000);
    memcpy(data, out, 16);
}

void EMBENET_AES128_Decrypt(uint8_t data[16U]) {
    uint32_t in[4];
    memcpy(in, data, 16);
    uint32_t out[4];
    HAL_CRYP_Decrypt(&hcryp, in, (uint16_t)16, out, 10000);
    memcpy(data, out, 16);
}
