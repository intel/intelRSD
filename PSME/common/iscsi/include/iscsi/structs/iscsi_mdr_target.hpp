/*!
 * @brief MDR region iSCSI Boot Options Target structure definition.
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
 * @file iscsi_mdr_target.hpp
 */

#pragma once

#include "iscsi/structs/iscsi_mdr_header.hpp"

namespace iscsi {
namespace structs {

enum class AuthenticationMethod : uint8_t {
    NO_CHAP = 0x0,
    CHAP = 0x1,
    MUTUAL_CHAP = 0x2
};

enum class TargetDhcpStatus : uint8_t {
    DISABLED = 0,
    ENABLED = 1
};

enum class VlanStatus : uint8_t {
    DISABLED = 0,
    ENABLED = 1
};

enum class RouterAdvertisement : uint8_t {
    DISABLED = 0,
    ENABLED = 1
};

#pragma pack(push, 1)

struct ISCSI_MDR_TARGET_DATA {
    TargetDhcpStatus target_dhcp_enabled;
    uint8_t target_name;
    IpAddressType ip_address_type;
    uint8_t target_ip_address[IP_ADDRESS_SIZE];
    uint16_t target_port;
    uint16_t target_lun;
    VlanStatus vlan_enabled;
    uint16_t vlan_id;
    RouterAdvertisement router_advertisement;
    AuthenticationMethod authentication_method;
    uint8_t chap_user_name;
    uint8_t chap_secret;
    uint8_t mutual_chap_name;
    uint8_t mutual_chap_secret;
};

struct ISCSI_MDR_TARGET {
    static constexpr MdrStructType ID = MdrStructType::TARGET;
    ISCSI_MDR_HEADER header;
    ISCSI_MDR_TARGET_DATA data;
};

#pragma pack(pop)

std::ostream& operator<<(std::ostream& os, const AuthenticationMethod& ids);
std::ostream& operator<<(std::ostream& os, const TargetDhcpStatus& ids);
std::ostream& operator<<(std::ostream& os, const VlanStatus& ids);
std::ostream& operator<<(std::ostream& os, const RouterAdvertisement& ids);

}  // namespace structs
}  // namespace iscsi
