/*!
 * @brief Intel Optane DC Persistent Memory FW Commands structs
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file dcpmem_structs.hpp
 */

#pragma once

#include "ipmi/utils/sdv/dcpmem_commands.hpp"
#include <cstdint>


#pragma pack(push, 1)

namespace pmem {
namespace structs {

struct IDENTIFY_DIMM {
    uint16_t VendorID;
    uint16_t DeviceID;
    uint16_t RevisionID;
    uint16_t InterfaceFormatCode;
    uint8_t  FirmwareRevision[5];
    uint8_t  Rsvd0;
    uint8_t  FeatureSwRequiredMask;
    uint8_t  Rsvd1[17];
    uint32_t RawCapacity;
    uint16_t Manufacturer;
    uint32_t SerialNumber;
    uint8_t  PartNumber[20];
    uint32_t DimmSku;
    uint8_t  Rsvd2[2];
    uint16_t ApiVersion;
    uint8_t  DimmUniqueID[9];
    uint8_t  Rsvd3[49];
};

struct SECURITY_STATE {
    uint8_t SecurityStatus;
    uint8_t Rsvd[63];
};

struct POWER_MANAGEMENT_POLICY {
    uint8_t  Rsvd0;
    uint16_t PeakPowerBudget;
    uint16_t AveragePowerBudget;
};

struct DIMM_PARTITION_INFO {
    uint32_t VolatileCapacity;
    uint8_t  Rsvd0[4];
    uint64_t VolatileStart;
    uint32_t PersistentCapacity;
    uint8_t  Rsvd1[4];
    uint64_t PersistentStart;
    uint32_t RawCapacity;
    uint8_t  Rsvd2[28];
};

struct SMART_INFO {
    uint32_t ValidationFlags;
    uint8_t  Rsvd0[4];
    uint8_t  HealthStatus;
    uint8_t  PercentageRemaining;
    uint8_t  PercentageUsed;
    uint8_t  AlarmTrips;
    uint16_t MediaTemperature;
    uint16_t ControllerTemperature;
    uint32_t DirtyShutdownCount;
    uint8_t  AitDramStatus;
    uint16_t HealthStatusReason;
    uint8_t  Rsvd1[8];
    uint8_t  LastShutdownStatus;
    uint32_t VendorSpecificDataSize;
    uint64_t PowerCycles;
    uint64_t PowerOnTime;
    uint64_t Uptime;
    uint32_t UnlatchedDirtyShutdowns;
    uint8_t  LastShutdownStatusDetails;
    uint64_t LastShutdownTime;
    uint8_t  LastShutdownStatusExtendedDetails[3];
    uint8_t  Rsvd2[52];
};

#define SMART_INFO_LAST_SHUTDOWN_STATUS_CLEAN 0       // SMART_INFO::LastShutdownStatus: 00h - Clean Shutdown
#define SMART_INFO_ALARM_TRIPS_MEDIA_TEMPERATURE 0x02 // SMART_INFO::AlarmTrips: [BIT1] - Media Temperature Trip
#define SMART_INFO_ALARM_TRIPS_CORE_TEMPERATURE 0x04  // SMART_INFO::AlarmTrips: [BIT2] - Core Temperature Trip
#define SMART_INFO_TEMPERATURE(t) (t >> 4)            // SMART_INFO::Temperature in Celsius with 0.0625C resolution (value/0x10)

struct MEMORY_INFO {
    uint8_t MediaReads[16];
    uint8_t MediaWrites[16];
    uint8_t ReadRequests[16];
    uint8_t WriteRequests[16];
};

using DcpmemCommandId = ipmi::command::sdv::DcpmemCommandType;

struct DCPMEM_RESPONSE_HEADER {
    DcpmemCommandId type;
    uint8_t length;
};

/*!
 * @brief IDENTIFY_DIMM Command (0x01/0x00)
 */
struct DCPMEM_IDENTIFY_DIMM_DATA {
    static constexpr DcpmemCommandId ID = DcpmemCommandId::IDENTIFY_DIMM;
    DCPMEM_RESPONSE_HEADER header;
    IDENTIFY_DIMM data;
};

/*!
 * @brief GET_SECURITY_STATE Command (0x02/0x00)
 */
struct DCPMEM_GET_SECURITY_STATE_DATA {
    static constexpr DcpmemCommandId ID = DcpmemCommandId::GET_SECURITY_STATE;
    DCPMEM_RESPONSE_HEADER header;
    SECURITY_STATE data;
};

/*!
 * @brief GET_POWER_MANAGEMENT_POLICY Command (0x04/0x02)
 */
struct DCPMEM_GET_POWER_MANAGEMENT_POLICY_DATA {
    static constexpr DcpmemCommandId ID = DcpmemCommandId::GET_POWER_MANAGEMENT_POLICY;
    DCPMEM_RESPONSE_HEADER header;
    POWER_MANAGEMENT_POLICY data;
};

/*!
 * @brief GET_DIMM_PARTITION_INFO Command (0x06/0x02)
 */
struct DCPMEM_GET_DIMM_PARTITION_INFO_DATA {
    static constexpr DcpmemCommandId ID = DcpmemCommandId::GET_DIMM_PARTITION_INFO;
    DCPMEM_RESPONSE_HEADER header;
    DIMM_PARTITION_INFO data;
};

/*!
 * @brief GET_SMART_INFO Command (0x08/0x00)
 */
struct DCPMEM_GET_SMART_INFO_DATA {
    static constexpr DcpmemCommandId ID = DcpmemCommandId::GET_SMART_INFO;
    DCPMEM_RESPONSE_HEADER header;
    SMART_INFO data;
};

/*!
 * @brief GET_MEMORY_INFO Command (0x08/0x03)
 */
struct DCPMEM_GET_MEMORY_INFO_DATA {
    static constexpr DcpmemCommandId ID = DcpmemCommandId::GET_MEMORY_INFO;
    DCPMEM_RESPONSE_HEADER header;
    MEMORY_INFO data;
};

#pragma pack(pop)

}
}