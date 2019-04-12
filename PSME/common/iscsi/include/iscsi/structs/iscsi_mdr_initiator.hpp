/*!
 * @brief MDR region iSCSI Boot Options Initiator structure definition.
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
 * @file iscsi_mdr_initiator.hpp
 */

#pragma once

#include "iscsi/structs/iscsi_mdr_header.hpp"

#include <iosfwd>

namespace iscsi {
namespace structs {

enum class InitiatiorDhcpStatus : uint8_t {
    DISABLED = 0,
    ENABLED = 1
};

#pragma pack(push, 1)

struct ISCSI_MDR_INITIATOR_DATA {
    InitiatiorDhcpStatus initiator_dhcp_enabled;
    uint8_t initiator_name;
    IpAddressType ip_address_type;
    uint8_t initiator_ip_address[IP_ADDRESS_SIZE];
    uint8_t initiator_subnet_mask[IP_ADDRESS_SIZE];
    uint8_t initiator_gateway[IP_ADDRESS_SIZE];
    uint8_t initiator_primary_dns[IP_ADDRESS_SIZE];
    uint8_t initiator_secondary_dns[IP_ADDRESS_SIZE];
    uint8_t nic_mac_address[MAC_ADDRESS_SIZE];
};

struct ISCSI_MDR_INITIATOR {
    static constexpr MdrStructType ID = MdrStructType::INITIATOR;
    ISCSI_MDR_HEADER header;
    ISCSI_MDR_INITIATOR_DATA data;
};

#pragma pack(pop)

std::ostream& operator<<(std::ostream& os, const InitiatiorDhcpStatus& ids);

}  // namespace structs
}  // namespace iscsi
