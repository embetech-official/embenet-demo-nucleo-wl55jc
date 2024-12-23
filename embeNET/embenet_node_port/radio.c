/**
 * @file
 * @license   commercial
 * @copyright Embetech sp. z o.o.
 * @version   1.0.0
 * @purpose   embeNET Node port
 * @brief     Implementation of radio transceiver interface for embeNET Node
 */

#include <embenet_port/critical_section.h>
#include <embenet_port/radio.h>

#include "embenet_node_stm32wle5_bsp_interface.h"
#include "port_interrupt_priorities.h"
#include "radio_driver.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <stm32wlxx.h>
#include <stm32wlxx_hal_subghz.h>
#pragma GCC diagnostic pop

#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifndef EMBENET_RADIO_MAX_OUTPUT_POWER_REDUCTION
#define EMBENET_RADIO_MAX_OUTPUT_POWER_REDUCTION 0
#endif

enum {
  SyncWord = 0x904EU,
  SyncwordLengthBits = 2U * CHAR_BIT,
  PreambleLengthBits = 8U * CHAR_BIT,
  Bandwidth = 100000,
  Bitrate = 50000,
  FrequencyDeviation = 12500,
  MaxTxp = 22 - EMBENET_RADIO_MAX_OUTPUT_POWER_REDUCTION,
  MinTxp = 0,
  Sensitivity = -100,
  BaseFrequency = 863100000,
  LbtChannelBusyThreshold = -85,
};

enum {
  IdleToTxReady = (500 + 100),
  IdleToRxReady = (150 + 100),
  ActiveToTxReady = (240 + 100),
  ActiveToRxReady = (100 + 100),
  LbtDuration = (240), // 200us of LBT and 80us of preparation
  TxDelay = (200 + LbtDuration),
  RxDelay = (200), // between GO signal and start listening - cannot measure but could be the same as delayTx without LBT
  TxStartOffset = (200),
  RxEndOffset = (70),
  RxStartOffset = (1700),
};

// FSK bandwidth definition
typedef struct {
  uint32_t bandwidth;
  uint8_t RegValue;
} FskBandwidth;

// pre-computed FSK bandwidth register values
FskBandwidth const fskBandwidths[] = {
    {4800, 0x1F},   {5800, 0x17},   {7300, 0x0F},   {9700, 0x1E},   {11700, 0x16},  {14600, 0x0E},  {19500, 0x1D},  {23400, 0x15},
    {29300, 0x0D},  {39000, 0x1C},  {46900, 0x14},  {58600, 0x0C},  {78200, 0x1B},  {93800, 0x13},  {117300, 0x0B}, {156200, 0x1A},
    {187200, 0x12}, {234300, 0x0A}, {312000, 0x19}, {373600, 0x11}, {467000, 0x09}, {500000, 0x00}, // Invalid Bandwidth
};

/**
 * @brief Sets transceiver frequency for a given channel.
 * @param[in] channel
 */
static void setFrequency(EMBENET_RADIO_Channel channel) {
  static int previouslySetChannel = -1;
  if(previouslySetChannel != channel) {
    previouslySetChannel = channel;
    SUBGRF_SetRfFrequency(((uint32_t)channel * (uint32_t)Bandwidth) + (uint32_t)BaseFrequency);
  }
}

/**
 * @brief	Sets output power.
 * @param 	powerDbm, must be in range <EMBENET_RADIO_MIN_OUTPUT_POWER, EMBENET_RADIO_MAX_OUTPUT_POWER>
 * @retval  RFO_LP
 * @retval  RFO_HP
 */
static uint8_t setPower(EMBENET_RADIO_Power powerDbm) { return SUBGRF_SetRfTxPower((int8_t)(powerDbm + EMBENET_NODE_BSP_GetTxOffset())); }

/**
 * @brief	Returns the known FSK bandwidth register value.
 * @param[in] Bandwidth value in Hz
 * @retval regValue Bandwidth register value
 */
static uint8_t getFskBandwidthRegValue(uint32_t bandwidth) {
  uint8_t i;
  if(bandwidth == 0) {
    return (0x1FU);
  }
  uint32_t delta = 1;
  while(delta < 20000) {
    for(i = 0; i < (sizeof(fskBandwidths) / sizeof(FskBandwidth)); i++) {
      if(bandwidth - fskBandwidths[i].bandwidth < delta) {
        return fskBandwidths[i].RegValue;
      }
    }
    delta++;
  }
  return 0x1FU;
}

SUBGHZ_HandleTypeDef hsubghz; // radio_driver.c expects this to have external linkage

typedef enum {
  StateIdle,
  StateRxReady,
  StateListen,
  StateRx,
  StateTxReady,
  StateTx,
  StateActive,
} State;

// structure to hold radio variables and states
typedef struct {
  EMBENET_RADIO_CaptureCbt onFrameStart; ///< handler to method called when start of frame interrupt occurs
  EMBENET_RADIO_CaptureCbt onFrameEnd;   ///< handler to method called when end of frame interrupt occurs
  void *handlersContext;                 ///< context passed to hanlders
  PacketParams_t packetParams;           ///< packet configuration
  bool frameReady;                       ///< denotes if frame is ready
  bool frameCrcIsValid;                  ///< denotes if ready frame crc is valid
  int8_t channelBusyThreshold;
  uint8_t txSwitch;
} EMBENET_RADIO_Descriptor;
static EMBENET_RADIO_Descriptor embenetRadioDescr;
static State volatile state;

EMBENET_RADIO_Status EMBENET_RADIO_Init(void) {
  embenetRadioDescr = (EMBENET_RADIO_Descriptor){0};
  __HAL_RCC_SUBGHZ_CLK_ENABLE();

  /* Force the SUBGHZ Periheral Clock Reset */
  __HAL_RCC_SUBGHZ_FORCE_RESET();

  /* Release the SUBGHZ Periheral Clock Reset */
  __HAL_RCC_SUBGHZ_RELEASE_RESET();

  hsubghz.Init.BaudratePrescaler = SUBGHZSPI_BAUDRATEPRESCALER_2;
  if(HAL_SUBGHZ_Init(&hsubghz) != HAL_OK) {
    while(1)
      ;
  }

  embenetRadioDescr.channelBusyThreshold = LbtChannelBusyThreshold + EMBENET_NODE_BSP_GetRxOffset();

  SUBGRF_Init(NULL);

  // Enable SUBGHZ Radio NVIC Interrupt (it is enabled inside the radio driver, but redo this again with proper priority setting)
  NVIC_SetPriority(SUBGHZ_Radio_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), INTERRUPT_PRIORITY_RADIO, INTERRUPT_SUBPRIORITY_RADIO));
  NVIC_EnableIRQ(SUBGHZ_Radio_IRQn);

  SUBGRF_SetRegulatorMode();
  SUBGRF_SetBufferBaseAddress(0x00, 0x00);
  SUBGRF_SetTxParams(RFO_LP, 0, RADIO_RAMP_200_US);
  SUBGRF_SetDioIrqParams(IRQ_TX_DONE | IRQ_RX_DONE | IRQ_SYNCWORD_VALID | IRQ_CRC_ERROR,
                         IRQ_TX_DONE | IRQ_RX_DONE | IRQ_SYNCWORD_VALID | IRQ_CRC_ERROR, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
  SUBGRF_SetStopRxTimerOnPreambleDetect(false);

  embenetRadioDescr.packetParams.PacketType = PACKET_TYPE_GFSK;

  SUBGRF_SetPacketType(embenetRadioDescr.packetParams.PacketType);

  uint8_t syncWordBuffer[8] = {(uint8_t)(SyncWord >> 8U), (uint8_t)(SyncWord)};
  SUBGRF_SetSyncWord(syncWordBuffer);

  ModulationParams_t modulationParams;
  modulationParams.PacketType = PACKET_TYPE_GFSK;
  modulationParams.Params.Gfsk.BitRate = Bitrate;
  modulationParams.Params.Gfsk.ModulationShaping = MOD_SHAPING_OFF;
  modulationParams.Params.Gfsk.Bandwidth = getFskBandwidthRegValue(Bandwidth);
  modulationParams.Params.Gfsk.Fdev = FrequencyDeviation;
  SUBGRF_SetModulationParams(&modulationParams);

  embenetRadioDescr.packetParams.Params.Gfsk.PreambleLength = PreambleLengthBits;
  embenetRadioDescr.packetParams.Params.Gfsk.PreambleMinDetect = RADIO_PREAMBLE_DETECTOR_16_BITS;
  embenetRadioDescr.packetParams.Params.Gfsk.SyncWordLength = SyncwordLengthBits;
  embenetRadioDescr.packetParams.Params.Gfsk.AddrComp = RADIO_ADDRESSCOMP_FILT_OFF;
  embenetRadioDescr.packetParams.Params.Gfsk.HeaderType = RADIO_PACKET_VARIABLE_LENGTH;
  embenetRadioDescr.packetParams.Params.Gfsk.PayloadLength = EMBENET_RADIO_MAX_PSDU_LENGTH;
  embenetRadioDescr.packetParams.Params.Gfsk.CrcLength = RADIO_CRC_2_BYTES_CCIT;
  embenetRadioDescr.packetParams.Params.Gfsk.DcFree = RADIO_DC_FREEWHITENING;
  SUBGRF_SetPacketParams(&embenetRadioDescr.packetParams);

  // Fall-back mode after successfull packet transmission or packet reception
  // 0x20 - Standby mode entry (default)
  // 0x30 - Standby with HSE32 enabled mode entry
  // 0x40 - FS mode entry
  SUBGRF_SetRxTxFallbackMode(0x40);

  EMBENET_RADIO_Idle();

  return EMBENET_RADIO_STATUS_SUCCESS;
}

void EMBENET_RADIO_SetCallbacks(EMBENET_RADIO_CaptureCbt onStartFrame, EMBENET_RADIO_CaptureCbt onEndFrame, void *cbtContext) {
  embenetRadioDescr.onFrameStart = onStartFrame;
  embenetRadioDescr.onFrameEnd = onEndFrame;
  embenetRadioDescr.handlersContext = cbtContext;
}

void EMBENET_RADIO_Deinit(void) {
  embenetRadioDescr.onFrameStart = NULL;
  embenetRadioDescr.onFrameEnd = NULL;
}

EMBENET_RADIO_Status EMBENET_RADIO_Idle(void) {
  SUBGRF_SetStandby(STDBY_XOSC);
  state = StateIdle;
  return EMBENET_RADIO_STATUS_SUCCESS;
}

EMBENET_RADIO_Status EMBENET_RADIO_TxEnable(EMBENET_RADIO_Channel channel, EMBENET_RADIO_Power txp, uint8_t const *psdu, size_t psduLen) {
  // whole 530us
  embenetRadioDescr.txSwitch = setPower(txp); // 90us
  setFrequency(channel);                      // 70us
  embenetRadioDescr.packetParams.Params.Gfsk.PayloadLength = (uint8_t)psduLen;
  SUBGRF_SetPacketParams(&embenetRadioDescr.packetParams);     // 80us
  SUBGRF_WriteBuffer(0x00, (uint8_t *)psdu, (uint8_t)psduLen); // 200-300us

  state = StateTxReady;
  return EMBENET_RADIO_STATUS_SUCCESS;
}

EMBENET_RADIO_Status EMBENET_RADIO_TxNow(void) {
  EMBENET_TimeUs timestamp = EMBENET_TIMER_ReadCounter();
  // 80 us
  SUBGRF_SetSwitch(0, RFSWITCH_RX);
  SUBGRF_SetRx(0xFFFFFF);

  // 160 us
  int count = 0;
  int mean = 0;
  while((EMBENET_TimeUs)(EMBENET_TIMER_ReadCounter() - timestamp) < (EMBENET_TimeUs)LbtDuration) {
    int8_t rssi = SUBGRF_GetRssiInst();
    if(rssi > INT8_MIN) {
      ++count;
      mean += rssi;
    }
  }
  if(count) {
    mean /= count;
  }
  if(count && (mean <= embenetRadioDescr.channelBusyThreshold)) {
    SUBGRF_SetSwitch(embenetRadioDescr.txSwitch, RFSWITCH_TX);
    state = StateTx;
    SUBGRF_SetTx(0);
    if(embenetRadioDescr.onFrameStart) {
      embenetRadioDescr.onFrameStart(embenetRadioDescr.handlersContext, timestamp + TxStartOffset);
    }
    return EMBENET_RADIO_STATUS_SUCCESS;

  } else {
    state = StateActive;
    return EMBENET_RADIO_STATUS_CHANNEL_BUSY;
  }
}

EMBENET_RADIO_Status EMBENET_RADIO_RxEnable(EMBENET_RADIO_Channel channel) {
  embenetRadioDescr.frameReady = false;
  embenetRadioDescr.frameCrcIsValid = false;

  embenetRadioDescr.packetParams.Params.Gfsk.PayloadLength = (uint8_t)EMBENET_RADIO_MAX_PSDU_LENGTH;
  SUBGRF_SetPacketParams(&embenetRadioDescr.packetParams);
  SUBGRF_SetSwitch(0, RFSWITCH_RX);
  setFrequency(channel);

  state = StateRxReady;
  return EMBENET_RADIO_STATUS_SUCCESS;
}

EMBENET_RADIO_Status EMBENET_RADIO_RxNow(void) {
  state = StateListen;
  SUBGRF_SetRx(0xFFFFFF);

  return EMBENET_RADIO_STATUS_SUCCESS;
}

EMBENET_RADIO_RxInfo EMBENET_RADIO_GetReceivedFrame(uint8_t *buffer, size_t bufferLength) {
  EMBENET_RADIO_RxInfo info = {.crcValid = false, .lqi = 0, .mpduLength = 0, .rssi = INT8_MIN};
  if(embenetRadioDescr.frameReady) {
    PacketStatus_t radioPktStatus;
    SUBGRF_GetPacketStatus(&radioPktStatus);
    info.lqi = radioPktStatus.Params.Gfsk.RxStatus;
    info.rssi = radioPktStatus.Params.Gfsk.RssiAvg;
    info.crcValid = embenetRadioDescr.frameCrcIsValid;

    uint8_t offset = 0;
    uint8_t length = 0;
    SUBGRF_GetRxBufferStatus(&length, &offset);
    info.mpduLength = length;

    uint8_t bytesCountToCopy = length; // best case, no error
    if(length > EMBENET_RADIO_MAX_PSDU_LENGTH) {
      // frame too long
      info.crcValid = false;
      bytesCountToCopy = EMBENET_RADIO_MAX_PSDU_LENGTH;
    }
    if(length > bufferLength) {
      // frame wont fit into buffer
      info.crcValid = false;
      bytesCountToCopy = (uint8_t)bufferLength; // uint8_t overflow not possible
    }
    if(length < EMBENET_RADIO_MIN_PSDU_LENGTH) {
      // frame too short
      info.crcValid = false;
    }
    SUBGRF_ReadBuffer(offset, buffer, bytesCountToCopy);
    // no buffering, frame may be obtained only once
    embenetRadioDescr.frameReady = false;
    embenetRadioDescr.frameCrcIsValid = false;
  }

  return info;
}

EMBENET_RADIO_Status EMBENET_RADIO_StartContinuousTx(EMBENET_RADIO_ContinuousTxMode mode, EMBENET_RADIO_Channel channel, EMBENET_RADIO_Power txp) {
  EMBENET_RADIO_TxEnable(channel, txp, NULL, 0);
  switch(mode) {
  case EMBENET_RADIO_CONTINUOUS_TX_MODE_CARRIER:
    SUBGRF_SetTxContinuousWave();
    break;
  case EMBENET_RADIO_CONTINUOUS_TX_MODE_PN9: {
    uint8_t pcr1aValue =
        (uint8_t)(SUBGRF_ReadRegister(SUBGHZ_PKTCTL1A) | UINT8_C(0x0C)); // Set Generic packet infinite sequence to PRBS9 instead of PREAMBLE
    SUBGRF_WriteRegister(SUBGHZ_PKTCTL1A, pcr1aValue);                   // select mode pn9 instead of preamble
    SUBGRF_SetTxInfinitePreamble();
  } break;
  default:
    return EMBENET_RADIO_STATUS_GENERAL_ERROR;
  }
  state = StateTx;
  return EMBENET_RADIO_STATUS_SUCCESS;
}

EMBENET_RADIO_Capabilities const *EMBENET_RADIO_GetCapabilities(void) {
  static EMBENET_RADIO_Capabilities timings = {.idleToTxReady = IdleToTxReady,
                                               .idleToRxReady = IdleToRxReady,
                                               .activeToTxReady = ActiveToTxReady,
                                               .activeToRxReady = ActiveToRxReady,
                                               .txDelay = TxDelay,
                                               .rxDelay = RxDelay,
                                               .txRxStartDelay = RxStartOffset,
                                               .sensitivity = Sensitivity,
                                               .maxOutputPower = MaxTxp,
                                               .minOutputPower = MinTxp};
  return &timings;
}

void SUBGHZ_Radio_IRQHandler(void) {
  EMBENET_TimeUs timestamp = EMBENET_TIMER_ReadCounter();

  uint8_t tmpisr[2];
  // Retrieve Interrupts from SUBGHZ Irq Register
  (void)HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_IRQSTATUS, tmpisr, 2);

  uint16_t itsource = (uint16_t)((tmpisr[0] << 8) | tmpisr[1]);
  (void)HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_CLR_IRQSTATUS, tmpisr, 2);

  // sync word detection has an utmost priority
  if((SET == SUBGHZ_CHECK_IT_SOURCE(itsource, SUBGHZ_IT_SYNCWORD_VALID)) && (state == StateListen)) {
    state = StateRx;
    if(embenetRadioDescr.onFrameStart != NULL) {
      timestamp = timestamp - (EMBENET_TimeUs)RxStartOffset;
      embenetRadioDescr.onFrameStart(embenetRadioDescr.handlersContext, timestamp);
    }
  }
  if((SET == SUBGHZ_CHECK_IT_SOURCE(itsource, SUBGHZ_IT_TX_CPLT)) && (state == StateTx)) {
    state = StateActive;
    if(embenetRadioDescr.onFrameEnd != NULL) {
      embenetRadioDescr.onFrameEnd(embenetRadioDescr.handlersContext, timestamp);
    }
  }
  if(((SET == SUBGHZ_CHECK_IT_SOURCE(itsource, SUBGHZ_IT_CRC_ERROR)) || (SET == SUBGHZ_CHECK_IT_SOURCE(itsource, SUBGHZ_IT_RX_CPLT))) &&
     (state == StateRx)) {
    state = StateActive;
    if(false == embenetRadioDescr.frameReady) {
      embenetRadioDescr.frameCrcIsValid = (RESET == SUBGHZ_CHECK_IT_SOURCE(itsource, SUBGHZ_IT_CRC_ERROR));
      embenetRadioDescr.frameReady = true;

      if(embenetRadioDescr.onFrameEnd != NULL) {
        timestamp = timestamp - (EMBENET_TimeUs)RxEndOffset;
        embenetRadioDescr.onFrameEnd(embenetRadioDescr.handlersContext, timestamp);
      }
      // WORKAROUND - Implicit Header Mode Timeout Behavior, see DS_SX1261-2_V1.2
      // datasheet chapter 15.3 RegRtcControl = @address 0x0902
      SUBGRF_WriteRegister(0x0902, 0x00);
      // RegEventMask = @address 0x0944
      SUBGRF_WriteRegister(0x0944, SUBGRF_ReadRegister(0x0944) | (1 << 1));
      // WORKAROUND END
    }
  }
}
