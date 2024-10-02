/**
@file
@license   Commercial
@copyright (c) 2023 EMBETECH SP. Z O.O. All rights reserved.
@version   1.1.4417
@purpose   embeNET Node port
@brief     Definition of port capabilities for embeNET Node

Contains set of parameters for DEMO and FULL version separately. Define EMBENET_NODE_DEMO_MODE=1 to set DEMO values.
*/

#include "embenet_port_capabilities.h"
#include "embenet_timer.h"

const EMBENET_MAC_Timings embenetMacTimings = {
    .TsTxOffsetUs     = 2000,       //
    .TsTxAckDelayUs   = 2200,       //
    .TsLongGTUs       = (2000 / 2), //
    .TsShortGTUs      = (300 / 2),  //
    .TsSlotDurationUs = 35000,      //
    .wdRadioTxUs      = 2000,       //
    .wdDataDurationUs = 30000,      //
    .wdAckDurationUs  = 8000        //
};

const size_t  embenetMacChannelListSize = 69;
const uint8_t embenetMacChannelList[]   = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
                                           35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68};

#if defined(EMBENET_NODE_DEMO_MODE) && EMBENET_NODE_DEMO_MODE == 1
/// fast mode refers to configuration in which node performance is much faster (in cost of expected network scalability)
const size_t  embenetMacAdvChannelListSize = 3;
const uint8_t embenetMacAdvChannelList[]   = {10, 33, 62};

const EMBENET_TimeUs embenetMacTimeCorrectionGuardUs = 500;
const uint32_t       embenetMacKaPeriodSlots         = ((10000000 / embenetMacTimings.TsSlotDurationUs) + 1);
const uint32_t       embenetMacDesyncTimeoutSlots    = ((30000000 / embenetMacTimings.TsSlotDurationUs) + 1);
#else
const size_t  embenetMacAdvChannelListSize = 3;
const uint8_t embenetMacAdvChannelList[]   = {13, 27, 53};

const EMBENET_TimeUs embenetMacTimeCorrectionGuardUs = 500;
const uint32_t       embenetMacKaPeriodSlots         = ((15000000 / embenetMacTimings.TsSlotDurationUs) + 1);
const uint32_t       embenetMacDesyncTimeoutSlots    = ((180000000 / embenetMacTimings.TsSlotDurationUs) + 1);
#endif

const bool                      embenetMacTopologyActive = false;
const EMBENET_MAC_TopologyEntry embenetMacTopologyList[] = {{0}};

const size_t embenetMacTopologyListEntriesCount = sizeof(embenetMacTopologyList) / sizeof(EMBENET_MAC_TopologyEntry);
