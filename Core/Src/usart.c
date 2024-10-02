/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
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
#include "ring_buffer.h"
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

#define DEBUG_UART_TX_BUFFER_SIZE 1024

static RingBuffer txRingBuffer;
static uint8_t txRingBufferSpace[DEBUG_UART_TX_BUFFER_SIZE];

static USART_TypeDef *uart;

/* USER CODE END 0 */

/* LPUART1 init function */

void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */
	RingBuffer_Init(&txRingBuffer, txRingBufferSpace, sizeof(txRingBufferSpace));
  /* USER CODE END LPUART1_Init 0 */

  LL_LPUART_InitTypeDef LPUART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_PCLK1);

  /* Peripheral clock enable */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPUART1);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  /**LPUART1 GPIO Configuration
  PA3   ------> LPUART1_RX
  PA2   ------> LPUART1_TX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_3|LL_GPIO_PIN_2;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  LPUART_InitStruct.PrescalerValue = LL_LPUART_PRESCALER_DIV1;
  LPUART_InitStruct.BaudRate = 115200;
  LPUART_InitStruct.DataWidth = LL_LPUART_DATAWIDTH_8B;
  LPUART_InitStruct.StopBits = LL_LPUART_STOPBITS_1;
  LPUART_InitStruct.Parity = LL_LPUART_PARITY_NONE;
  LPUART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX;
  LPUART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
  LL_LPUART_Init(LPUART1, &LPUART_InitStruct);
  LL_LPUART_SetTXFIFOThreshold(LPUART1, LL_LPUART_FIFOTHRESHOLD_8_8);
  LL_LPUART_SetRXFIFOThreshold(LPUART1, LL_LPUART_FIFOTHRESHOLD_8_8);
  LL_LPUART_EnableOverrunDetect(LPUART1);
  LL_LPUART_EnableDMADeactOnRxErr(LPUART1);

  /* USER CODE BEGIN WKUPType LPUART1 */

  /* USER CODE END WKUPType LPUART1 */

  LL_LPUART_Enable(LPUART1);

  /* Polling LPUART1 initialisation */
  while(!(LL_LPUART_IsActiveFlag_TEACK(LPUART1)))
  {
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

	/* USART1 interrupt Init */
	NVIC_SetPriority(LPUART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 4, 0));
	NVIC_EnableIRQ(LPUART1_IRQn);
	uart = LPUART1;
  /* USER CODE END LPUART1_Init 2 */

}
/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */
	RingBuffer_Init(&txRingBuffer, txRingBufferSpace, sizeof(txRingBufferSpace));
  /* USER CODE END USART1_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  /**USART1 GPIO Configuration
  PB7   ------> USART1_RX
  PB6   ------> USART1_TX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_7|LL_GPIO_PIN_6;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART1, &USART_InitStruct);
  LL_USART_SetTXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_8_8);
  LL_USART_SetRXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_DisableFIFO(USART1);
  LL_USART_DisableOverrunDetect(USART1);
  LL_USART_DisableDMADeactOnRxErr(USART1);
  LL_USART_ConfigAsyncMode(USART1);

  /* USER CODE BEGIN WKUPType USART1 */

  /* USER CODE END WKUPType USART1 */

  LL_USART_Enable(USART1);

  /* Polling USART1 initialisation */
  while(!(LL_USART_IsActiveFlag_TEACK(USART1)))
  {
  }
  /* USER CODE BEGIN USART1_Init 2 */
	/* USART1 interrupt Init */
	NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 4, 0));
	NVIC_EnableIRQ(USART1_IRQn);
	uart = USART1;
  /* USER CODE END USART1_Init 2 */

}

/* USER CODE BEGIN 1 */

void LOGGER_UART_Init() {
#if 1 == IS_ROOT
	MX_USART1_UART_Init();
#else
	MX_LPUART1_UART_Init();
#endif
}

void LOGGER_UART_Write(const void *ptr, size_t len) {
	if (ptr && len) {
		for (size_t l = 0; l < len; ++l) {
			// enter critical section
			uint32_t primaskBit = __get_PRIMASK();
			__disable_irq();
			// put data to ring buffer
			RingBuffer_PutChar(&txRingBuffer, ((const uint8_t*) ptr)[l]);
			// exit critical section
			__set_PRIMASK(primaskBit);
			// trigger IRQ-driven transmission
#if 1 == IS_ROOT
			LL_USART_EnableIT_TXE(uart);
#else
			LL_LPUART_EnableIT_TXE(uart);
#endif
		}
	}
}

void LOGGER_UART_FlushPolling(void) {
	// enter critical section
	uint32_t primaskBit = __get_PRIMASK();
	__disable_irq();

	uint8_t c;
	while (true == RingBuffer_GetChar(&txRingBuffer, &c)) {
#if 1 == IS_ROOT
		while (!LL_USART_IsActiveFlag_TXE(uart)) {
		}
		LL_USART_TransmitData8(uart, c);
#else
        while (!LL_LPUART_IsActiveFlag_TXE(uart)) {
		}
		LL_LPUART_TransmitData8(uart, c);
#endif
	}
	// exit critical section
	__set_PRIMASK(primaskBit);
}

/******************************************************************************/
/* STM32WLxx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32wlxx.s).                    */
/******************************************************************************/
#if 1 == IS_ROOT
/**
 * @brief This function handles USART1 Interrupt.
 */
void USART1_IRQHandler(void) {
	/* USER CODE BEGIN USART1_IRQn 0 */
	if (LL_USART_IsActiveFlag_TXE(uart)) {
		uint8_t c;
		if (true == RingBuffer_GetChar(&txRingBuffer, &c)) {
			LL_USART_TransmitData8(uart, c);
		} else {
			LL_USART_DisableIT_TXE(uart);
		}
	} else if (LL_USART_IsActiveFlag_PE(uart)) {
		LL_USART_ClearFlag_PE(uart);
	} else if (LL_USART_IsActiveFlag_FE(uart)) {
		LL_USART_ClearFlag_FE(uart);
	} else if (LL_USART_IsActiveFlag_NE(uart)) {
		LL_USART_ClearFlag_NE(uart);
	} else if (LL_USART_IsActiveFlag_ORE(uart)) {
		LL_USART_ClearFlag_ORE(uart);
	}

	/* USER CODE END USART1_IRQn 0 */
	/* USER CODE BEGIN USART1_IRQn 1 */

	/* USER CODE END USART1_IRQn 1 */
}
#else
/**
 * @brief This function handles LPUART1 Interrupt.
 */
void LPUART1_IRQHandler(void) {
	/* USER CODE BEGIN LPUART1_IRQn 0 */
	if (LL_LPUART_IsActiveFlag_TXE(uart)) {
		uint8_t c;
		if (true == RingBuffer_GetChar(&txRingBuffer, &c)) {
			LL_LPUART_TransmitData8(uart, c);
		} else {
			LL_LPUART_DisableIT_TXE(uart);
		}
	} else if (LL_LPUART_IsActiveFlag_PE(uart)) {
		LL_LPUART_ClearFlag_PE(uart);
	} else if (LL_LPUART_IsActiveFlag_FE(uart)) {
		LL_LPUART_ClearFlag_FE(uart);
	} else if (LL_LPUART_IsActiveFlag_NE(uart)) {
		LL_LPUART_ClearFlag_NE(uart);
	} else if (LL_LPUART_IsActiveFlag_ORE(uart)) {
		LL_LPUART_ClearFlag_ORE(uart);
	}

	/* USER CODE END LPUART1_IRQn 0 */
	/* USER CODE BEGIN LPUART1_IRQn 1 */

	/* USER CODE END LPUART1_IRQn 1 */
}
#endif

/* USER CODE END 1 */
