/*!
* @brief PLDM protocol constants and data structures definition
*
* @copyright Copyright (c) 2019 Intel Corporation.
*
* Licensed under the Apache License, Version 2.0 (the "License") override;
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License
*
* @file pldm.hpp
*/

#pragma once



#include "agent-framework/module/enum/enum_builder.hpp"
#include "protocol/mctp/mctp.hpp"



namespace agent {
namespace pnc {
namespace pldm {

using PldmCompletionCode = uint8_t;
using RecordHandle = uint32_t;
using DataTransferHandle = uint32_t;
using PldmVersionDataIntegrityChecksum = uint32_t;
using PldmTerminusId = uint8_t;
using RecordChangeNumber = std::uint16_t;
using ContainerId = std::uint16_t;

/*! PLDM over MCTP binding byte */
static constexpr uint8_t PLDM_OVER_MCTP_BINDING_BYTE =
    mctp::MessageType::PLDM | mctp::MctpIntegrityCheck::NO_INTEGRITY_CHECK;

/*! PLDM header size in bytes */
static constexpr uint32_t PLDM_HEADER_SIZE_BYTES = 3;

/*! PLDM over MCTP header size in bytes */
static constexpr uint32_t PLDM_OVER_MCTP_HEADER_SIZE_BYTES = PLDM_HEADER_SIZE_BYTES + 1;

/*! Default Data Transfer Handle value */
static constexpr uint32_t DEFAULT_DATA_TRANSFER_HANDLE = 0;

/*! PLDM type */
enum PldmType : std::uint8_t {
    PldmMessagingControlAndDiscovery = 0x00,
    PldmForPlatformMonitoringAndControlSpecification = 0x02,
    PldmForFruData = 0x04
};

/*! PLDM command */
enum PldmCommand : std::uint8_t {
    SetTid = 0x01,
    GetTid = 0x02,
    GetTerminusUid = 0x03,
    GetPldmVersion = 0x03,
    GetPldmTypes = 0x04,
    GetPldmCommands = 0x05,

    SetNumericSensorEnable = 0x10,
    GetSensorReading = 0x11,
    GetSensorThresholds = 0x12,
    SetSensorThresholds = 0x13,
    RestoreSensorThresholds = 0x14,
    GetSensorHysteresis = 0x15,
    SetSensorHysteresis = 0x16,
    GetPdrRepositoryInfo = 0x50,
    GetPdr = 0x51
};

/*! PLDM base completion codes */
enum PldmCompletionBaseCode : PldmCompletionCode {
    SUCCESS = 0x00,
    ERROR = 0x01,
    ERROR_INVALID_DATA = 0x02,
    ERROR_INVALID_LENGTH = 0x03,
    ERROR_NOT_READY = 0x04,
    ERROR_UNSUPPORTED_PLDM_CMD = 0x05,
    ERROR_INVALID_PLDM_TYPE = 0x20
};

/*! PLDM transfer flag */
enum TransferFlag : uint8_t {
    START = 0x01,
    MIDDLE = 0x02,
    END = 0x04,
    START_AND_END = 0x05
};

/*! PLDM transfer operational flag */
enum TransferOperationalFlag : uint8_t {
    GET_NEXT_PART = 0x00,
    GET_FIRST_PART = 0x01
};

#pragma pack(push, 1)
/*! Timestamp104 datatype format */
struct Timestamp104 {
    int16_t utc_offset;                     //UTC offset in minutes
    uint8_t microsecond[3];
    uint8_t seconds;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t utc_and_time_resolution;
};

/*! Ver32 datatype format */
struct Ver32 {
    uint8_t major_version_number;
    uint8_t minor_version_number;
    uint8_t update_version_number;
    uint8_t alpha;
};

/*! Common PDR header format */
struct CommonPdrHeader {
    uint32_t record_handle;
    uint8_t pdr_header_version;
    uint8_t pdr_type;
    uint16_t record_change_number;
    uint16_t data_length;
};

/*! Common GetPDR request header format */
struct CommonGetPdrRequestHeader {
    RecordHandle record_handle;
    DataTransferHandle data_transfer_handle;
    TransferOperationalFlag transfer_operational_flag;
    uint16_t request_count;
    RecordChangeNumber record_change_number;
};

/*! Common GetPDR response header format */
struct CommonGetPdrResponseHeader {
    RecordHandle next_record_handle;
    DataTransferHandle next_data_transfer_handle;
    TransferFlag transfer_flag;
    uint16_t response_count;
};

/*! PLDM UUID format */
struct PldmUuid {
    uint8_t uuid[16];
};

#pragma pack(pop)

}
}
}