#include <embenet_node_stm32wle5_bsp_interface.h>


#define RF_SW_CTRL3_PIN                GPIO_PIN_3
#define RF_SW_CTRL3_GPIO_PORT          GPIOC
#define RF_SW_CTRL3_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()
#define RF_SW_CTRL3_GPIO_CLK_DISABLE() __HAL_RCC_GPIOC_CLK_DISABLE()

#define RF_SW_CTRL1_PIN               GPIO_PIN_4
#define RF_SW_CTRL1_GPIO_PORT         GPIOC
#define RF_SW_CTRL1_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define RF_SW_RX_GPIO_CLK_DISABLE()   __HAL_RCC_GPIOC_CLK_DISABLE()

#define RF_SW_CTRL2_PIN                GPIO_PIN_5
#define RF_SW_CTRL2_GPIO_PORT          GPIOC
#define RF_SW_CTRL2_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()
#define RF_SW_CTRL2_GPIO_CLK_DISABLE() __HAL_RCC_GPIOC_CLK_DISABLE()


/// @brief Initialize and configure radio descriptor (hsubghz)
void EMBENET_NODE_BSP_RadioInit(void) {
    hsubghz.Init.BaudratePrescaler = SUBGHZSPI_BAUDRATEPRESCALER_2;
    if (HAL_SUBGHZ_Init(&hsubghz) != HAL_OK) {
    }
}

/// @brief Initialize RF Switch
void EMBENET_NODE_BSP_RBI_Init(void) {
    GPIO_InitTypeDef gpio_init_structure = {0};

    /* Enable the Radio Switch Clock */
    RF_SW_CTRL1_GPIO_CLK_ENABLE();
    RF_SW_CTRL2_GPIO_CLK_ENABLE();

    /* Configure the Radio Switch pin */
    gpio_init_structure.Pin   = RF_SW_CTRL1_PIN;
    gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull  = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    HAL_GPIO_Init(RF_SW_CTRL1_GPIO_PORT, &gpio_init_structure);

    gpio_init_structure.Pin = RF_SW_CTRL2_PIN;
    HAL_GPIO_Init(RF_SW_CTRL2_GPIO_PORT, &gpio_init_structure);

    gpio_init_structure.Pin = RF_SW_CTRL3_PIN;
    HAL_GPIO_Init(RF_SW_CTRL3_GPIO_PORT, &gpio_init_structure);

    EMBENET_NODE_BSP_ConfigRFSwitch(EMBENET_NODE_BSP_RF_SWITCH_OFF);
}


/// @brief Deinitialize RF Switch
void EMBENET_NODE_BSP_RBI_Deinit(void) {
    EMBENET_NODE_BSP_ConfigRFSwitch(EMBENET_NODE_BSP_RF_SWITCH_OFF);
    HAL_GPIO_DeInit(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN);
    HAL_GPIO_DeInit(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN);
    HAL_GPIO_DeInit(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL3_PIN);
}


/// @brief Return TX offset for given RF Path
int8_t EMBENET_NODE_BSP_GetTxOffset(void) {
    return 0;
}


void EMBENET_NODE_BSP_ConfigRFSwitch(EMBENET_NODE_BSP_RfSwitch pos) {
    switch (pos) {
        case EMBENET_NODE_BSP_RF_SWITCH_OFF:
            HAL_GPIO_WritePin(RF_SW_CTRL3_GPIO_PORT, RF_SW_CTRL3_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN, GPIO_PIN_RESET);
            break;
        case EMBENET_NODE_BSP_RF_SWITCH_RX:
            HAL_GPIO_WritePin(RF_SW_CTRL3_GPIO_PORT, RF_SW_CTRL3_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN, GPIO_PIN_RESET);
            break;
        case EMBENET_NODE_BSP_RF_SWITCH_TX_LP:
            HAL_GPIO_WritePin(RF_SW_CTRL3_GPIO_PORT, RF_SW_CTRL3_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN, GPIO_PIN_SET);
            break;
        case EMBENET_NODE_BSP_RF_SWITCH_TX_HP:
            HAL_GPIO_WritePin(RF_SW_CTRL3_GPIO_PORT, RF_SW_CTRL3_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN, GPIO_PIN_SET);
            break;
    }
}


EMBENET_NODE_BSP_TxConfig EMBENET_NODE_BSP_GetTxConfig(void) {
    return EMBENET_NODE_BSP_TX_CONFIG_LP_HP;
}

EMBENET_NODE_BSP_TCXOVoltage const EMBENET_NODE_BSP_TCXOSupplyVoltage  = EMBENET_NODE_BSP_TCXO_VOLTAGE_1_7V; // Unused, but required
uint32_t const                     EMBENET_NODE_BSP_HSEFreq            = 32000000U;
uint8_t const                      EMBENET_NODE_BSP_XOCapacitanceValue = 0; // Unused, but required
bool const                         EMBENET_NODE_BSP_HasTCXO            = true;
bool const                         EMBENET_NODE_BSP_HasDCDC            = true;
