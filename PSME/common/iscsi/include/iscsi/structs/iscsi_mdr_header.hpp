/*!
 * @brief MDR region iSCSI Boot Options common header definition.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file iscsi_mdr_header.hpp
 */

#pragma once

#include <cstdint>
#include <iosfwd>

namespace iscsi {
namespace structs {

constexpr std::size_t IP_ADDRESS_SIZE = 16;
constexpr std::size_t MAC_ADDRESS_SIZE = 6;

enum class IpAddressType : uint8_t {
    AUTO = 0x0,
    IPv4 = 0x1,
    IPv6 = 0x2
};

enum class MdrStructType : uint8_t {
    VERSION = 0x1,
    INITIATOR = 0x2,
    TARGET = 0x3,
    ATTEMPT = 0x4
};

#pragma pack(push, 1)

struct ISCSI_MDR_HEADER {
    MdrStructType parameter_id;
    uint16_t length;
    uint16_t handle;
};

#pragma pack(pop)

std::ostream& operator<<(std::ostream& os, const IpAddressType& iat);

}  // namespace structs
}  // namespace iscsi
