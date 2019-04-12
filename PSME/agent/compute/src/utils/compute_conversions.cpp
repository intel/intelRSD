/*!
 * @brief Compute model enum conversions.
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
 * @file compute.cpp
 */

#include "utils/compute_conversions.hpp"

using namespace agent_framework::model::enums;
namespace agent {
namespace compute {
namespace utils {

iscsi::structs::IpAddressType IscsiMdrConverter::get_ip_type(const IPAddressType& at) {
    switch (at) {
        case IPAddressType::IPv4:
            return iscsi::structs::IpAddressType::IPv4;
        case IPAddressType::IPv6:
            return iscsi::structs::IpAddressType::IPv6;
            // the model does not support AUTO type
        default:
            throw std::runtime_error("IscsiMdrConverter: Invalid IP type.");
    }
}

iscsi::structs::TargetDhcpStatus IscsiMdrConverter::get_dhcp_enabled(bool enabled) {
    return enabled ? iscsi::structs::TargetDhcpStatus::ENABLED : iscsi::structs::TargetDhcpStatus::DISABLED;
}

iscsi::structs::VlanStatus IscsiMdrConverter::get_vlan_enabled(bool enabled) {
    return enabled ? iscsi::structs::VlanStatus::ENABLED : iscsi::structs::VlanStatus::DISABLED;
}

iscsi::structs::RouterAdvertisement IscsiMdrConverter::get_router_advertisment(bool enabled) {
    return enabled ? iscsi::structs::RouterAdvertisement::ENABLED : iscsi::structs::RouterAdvertisement::DISABLED;
}

iscsi::structs::AuthenticationMethod IscsiMdrConverter::get_authentication_method(
    const FunctionAuthenticationMethod& am) {
    switch (am) {
        case FunctionAuthenticationMethod::None:
            return iscsi::structs::AuthenticationMethod::NO_CHAP;
        case FunctionAuthenticationMethod::MutualCHAP:
            return iscsi::structs::AuthenticationMethod::MUTUAL_CHAP;
        case FunctionAuthenticationMethod::CHAP:
            return iscsi::structs::AuthenticationMethod::CHAP;
        default:
            throw std::runtime_error("IscsiMdrConverter: Invalid authentication method.");
    }
}

FunctionAuthenticationMethod IscsiMdrConverter::get_authentication_method(
    const iscsi::structs::AuthenticationMethod& am) {
    switch (am) {
        case iscsi::structs::AuthenticationMethod::NO_CHAP:
            return FunctionAuthenticationMethod::None;
        case iscsi::structs::AuthenticationMethod::MUTUAL_CHAP:
            return FunctionAuthenticationMethod::MutualCHAP;
        case iscsi::structs::AuthenticationMethod::CHAP:
            return FunctionAuthenticationMethod::CHAP;
        default:
            throw std::runtime_error("IscsiMdrConverter: Invalid authentication method.");
    }
}

IPAddressType IscsiMdrConverter::get_ip_type(const iscsi::structs::IpAddressType& at) {
    switch (at) {
        case iscsi::structs::IpAddressType::IPv4:
            return IPAddressType::IPv4;
        case iscsi::structs::IpAddressType::IPv6:
            return IPAddressType::IPv6;
            // the model does not support AUTO type
        case iscsi::structs::IpAddressType::AUTO:
        default:
            throw std::runtime_error("IscsiMdrConverter: Invalid IP type.");
    }
}

} // namespace utils
} // namespace compute
} // namespace agent
