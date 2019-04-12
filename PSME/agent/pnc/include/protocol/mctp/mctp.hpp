/*!
 * @brief MCTP protocol constants definition
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
 * @file mctp.hpp
*/


#pragma once

namespace agent {
namespace pnc {
namespace mctp {

namespace header {

static constexpr uint8_t RSVD = 0x00;
static constexpr uint8_t HEADER_VERSION = 0x01;
static constexpr uint8_t DESTINATION_ENDPOINT_ID = 0x00;
static constexpr uint8_t SOURCE_ENDPOINT_ID = 0x00;
static constexpr uint8_t SOM_BIT = 0x80;
static constexpr uint8_t EOM_BIT = 0x40;
static constexpr uint8_t PKT_SEQ = 0x00;
static constexpr uint8_t TO_BIT = 0x08;
static constexpr uint8_t MSG_TAG = 0x00;

}

static constexpr uint8_t MCTP_OVER_SMBUS_BINDING_BYTE = 0x01;

static constexpr uint32_t MCTP_HEADER_SIZE_BYTES = 4;

static constexpr uint32_t MCTP_OVER_SMBUS_HEADER_SIZE_BYTES = MCTP_HEADER_SIZE_BYTES + 1;

enum MctpIntegrityCheck : uint8_t {
    NO_INTEGRITY_CHECK = 0x00,
    INTEGRITY_CHECK = 0x80
};

enum MessageType : uint8_t {
    PLDM = 0x01
};

}
}
}