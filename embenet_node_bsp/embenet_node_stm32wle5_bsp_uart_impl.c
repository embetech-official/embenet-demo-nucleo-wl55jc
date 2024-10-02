/**
 @file
 @license   Commercial
 @copyright (c) 2022 EMBETECH SP. Z O.O. All rights reserved.
 @version   1.0.3547
 @purpose   UART implementation.
 @brief

 */
#include <embenet_node_stm32wle5_bsp_interface.h>

#include <stm32wlxx.h>
#include <stm32wlxx_ll_bus.h>
#include <stm32wlxx_ll_gpio.h>
#include <stm32wlxx_ll_lpuart.h>
#include <stddef.h>



#if 1 == IS_ROOT

static EMBENET_NODE_BSP_UART_Callback_t txCb;
static EMBENET_NODE_BSP_UART_Callback_t rxCb;
static void *context;

void EMBENET_NODE_BSP_UART_Init(EMBENET_NODE_BSP_UART_Callback_t txEndCallback, EMBENET_NODE_BSP_UART_Callback_t rxCallback, void *context) {
	txCb = txEndCallback;
	rxCb = rxCallback;
	context = context;

	LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_PCLK1);
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPUART1);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

	/**LPUART1 GPIO Configuration
	 PA3   ------> LPUART1_RX
	 PA2   ------> LPUART1_TX
	 */
	LL_GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = LL_GPIO_PIN_3 | LL_GPIO_PIN_2;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	LL_LPUART_InitTypeDef LPUART_InitStruct = { 0 };
	LPUART_InitStruct.PrescalerValue = LL_LPUART_PRESCALER_DIV1;
	LPUART_InitStruct.BaudRate = 115200;
	LPUART_InitStruct.DataWidth = LL_LPUART_DATAWIDTH_8B;
	LPUART_InitStruct.StopBits = LL_LPUART_STOPBITS_1;
	LPUART_InitStruct.Parity = LL_LPUART_PARITY_NONE;
	LPUART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
	LPUART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
	LL_LPUART_Init(LPUART1, &LPUART_InitStruct);
	LL_LPUART_SetTXFIFOThreshold(LPUART1, LL_LPUART_FIFOTHRESHOLD_8_8);
	LL_LPUART_SetRXFIFOThreshold(LPUART1, LL_LPUART_FIFOTHRESHOLD_8_8);
	LL_LPUART_EnableOverrunDetect(LPUART1);
	LL_LPUART_EnableDMADeactOnRxErr(LPUART1);

	LL_LPUART_Enable(LPUART1);

	NVIC_SetPriority(LPUART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_ClearPendingIRQ(LPUART1_IRQn);
	NVIC_EnableIRQ(LPUART1_IRQn);
	LL_LPUART_EnableIT_RXNE(LPUART1);
	LL_LPUART_EnableIT_TC(LPUART1);
}

void EMBENET_NODE_BSP_UART_Deinit(void) {
	LL_LPUART_DisableIT_RXNE(LPUART1);
	LL_LPUART_DisableIT_TC(LPUART1);
	NVIC_DisableIRQ(LPUART1_IRQn);
	LL_LPUART_Disable(LPUART1);
	LL_LPUART_DeInit(LPUART1);

	LL_APB1_GRP2_DisableClock(LL_APB1_GRP2_PERIPH_LPUART1);

	txCb = NULL;
	rxCb = NULL;
	context = NULL;
}

void EMBENET_NODE_BSP_UART_WriteByte(uint8_t c) {
	LL_LPUART_TransmitData8(LPUART1, c);
}

uint8_t EMBENET_NODE_BSP_UART_ReadByte(void) {
	return LL_LPUART_ReceiveData8(LPUART1);
}

void LPUART1_IRQHandler(void) {
	if (LL_LPUART_IsActiveFlag_RXNE(LPUART1)) {
		rxCb(context); // Must NOT be null (otherwise init will fail)
	} else if (LL_LPUART_IsActiveFlag_TC(LPUART1)) {
		LL_LPUART_ClearFlag_TC(LPUART1);
		txCb(context); // Must NOT be null (otherwise init will fail)
	} else if (LL_LPUART_IsActiveFlag_PE(LPUART1)) {
		LL_LPUART_ClearFlag_PE(LPUART1);
	} else if (LL_LPUART_IsActiveFlag_FE(LPUART1)) {
		LL_LPUART_ClearFlag_FE(LPUART1);
	} else if (LL_LPUART_IsActiveFlag_NE(LPUART1)) {
		LL_LPUART_ClearFlag_NE(LPUART1);
	} else if (LL_LPUART_IsActiveFlag_ORE(LPUART1)) {
		LL_LPUART_ClearFlag_ORE(LPUART1);
	}
}

#else

void EMBENET_NODE_BSP_UART_Init(EMBENET_NODE_BSP_UART_Callback_t txEndCallback, EMBENET_NODE_BSP_UART_Callback_t rxCallback, void *context) {
}

void EMBENET_NODE_BSP_UART_Deinit(void) {
}

void EMBENET_NODE_BSP_UART_WriteByte(uint8_t c) {
}

uint8_t EMBENET_NODE_BSP_UART_ReadByte(void) {
	return 0;
}

#endif

