/*!
* @brief SMBus protocol constants definition
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
* @file smbus.hpp
*/

#pragma once

namespace agent {
namespace pnc {
namespace smbus {

/*! SMBus command code */
enum CommandCode : uint8_t {
    MCTP = 0x0F
};

/*! SMBus well-known slave addresses based on DMTF DSP0237_1.1.0 */
enum WellKnownSlaveAddresses : uint8_t {
    IPMI_BMC = 0x20
};

/*! Size of the SMBus block write command header in bytes */
static constexpr uint32_t SMBUS_BLOCK_WRITE_HEADER_SIZE_BYTES = 3;

/*! Size of the SMBus block write command header read by Aardvark device in bytes */
static constexpr uint32_t AARDVARK_SMBUS_BLOCK_WRITE_HEADER_SIZE_BYTES = SMBUS_BLOCK_WRITE_HEADER_SIZE_BYTES - 1;

/*! Size of SMBus Packet Error Code in bytes */
static constexpr uint32_t PEC_SIZE_BYTES = 1;

}
}
}