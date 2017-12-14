/*!
 * @brief Grantley network device function builder class implementation.
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @header{Filesystem}
 * @file grantley_network_device_function_builder.hpp
 */

#include "discovery/builders/platform_specific/grantley/grantley_network_device_function_builder.hpp"



using namespace agent::compute::discovery;
using namespace agent_framework::model;
using namespace ipmi::command::sdv;

namespace {

enums::FunctionAuthenticationMethod chap_type_to_enum(response::GetOobTargetBootOptions::ChapType type) {
    switch (type) {
        case response::GetOobTargetBootOptions::ChapType::None :
            return enums::FunctionAuthenticationMethod::None;
        case response::GetOobTargetBootOptions::ChapType::OneWayChap :
            return enums::FunctionAuthenticationMethod::CHAP;
        case response::GetOobTargetBootOptions::ChapType::MutualChap :
            return enums::FunctionAuthenticationMethod::MutualCHAP;
        default:
            throw std::runtime_error{"Read an unknown authentication type " + std::to_string(uint32_t(type))};
    }
}


bool is_not_specified(const std::string& ip_address) {
    // in IPMI OOB options this is null value for IP addresses
    return ip_address == "0.0.0.0";
}


bool is_default_nic(const std::string& mac_address) {
    // in IPMI OOB options this mac address means default NIC
    return mac_address == "00:00:00:00:00:00";
}

}


void GrantleyNetworkDeviceFunctionBuilder::update_oob_control_boot_options(
    agent_framework::model::NetworkDeviceFunction& network_device_function,
    const ipmi::command::sdv::response::GetOobControlBootOptions& get_oob_control_boot_options_response) {
    network_device_function.set_device_enabled(get_oob_control_boot_options_response.get_bios_enabled());
}


void GrantleyNetworkDeviceFunctionBuilder::update_initator_name(
    agent_framework::model::NetworkDeviceFunction& network_device_function,
    const std::string& initiator_name) {

    auto boot = network_device_function.get_iscsi_boot();

    boot.set_initiator_name(initiator_name);

    network_device_function.set_iscsi_boot(boot);
}


void GrantleyNetworkDeviceFunctionBuilder::update_oob_nic_boot_options(
    agent_framework::model::NetworkDeviceFunction& network_device_function,
    const ipmi::command::sdv::response::GetOobNicBootOptions& get_oob_nic_boot_options_response) {
    if (!get_oob_nic_boot_options_response.get_mac_address().empty() &&
        !is_default_nic(get_oob_nic_boot_options_response.get_mac_address())) {
        network_device_function.set_mac_address(get_oob_nic_boot_options_response.get_mac_address());
    }

    attribute::IscsiBoot boot;
    if (get_oob_nic_boot_options_response.get_dhcp_version() ==
        response::GetOobBootOptions::IPVersion::IPV6) { // no equivalent for AUTO value in metadata enum
        boot.set_ip_address_type(enums::IPAddressType::IPv6);
    }

    const auto& initiator_address = get_oob_nic_boot_options_response.get_address();
    boot.set_initiator_address(is_not_specified(initiator_address) ? "" : initiator_address);

    const auto& gateway = get_oob_nic_boot_options_response.get_gateway();
    if (!gateway.empty() && !is_not_specified(gateway)) {
        boot.set_initiator_default_gateway(gateway);
    }

    const auto& initiator_netmask = get_oob_nic_boot_options_response.get_subnet();
    boot.set_initiator_netmask(is_not_specified(initiator_netmask) ? "" : initiator_netmask);

    bool primary_vlan_enabled = get_oob_nic_boot_options_response.get_vlan_enabled();
    boot.set_primary_vlan_enable(primary_vlan_enabled);
    if (primary_vlan_enabled) {
        boot.set_primary_vlan_id(get_oob_nic_boot_options_response.get_vlan_id());
    }

    // DHCP can supply primary and secondary DNS addresses
    bool ip_mask_dns_via_dhcp = get_oob_nic_boot_options_response.get_dhcp_enabled();
    boot.set_ip_mask_dns_via_dhcp(ip_mask_dns_via_dhcp);

    if (!ip_mask_dns_via_dhcp) {
        const auto& primary_dns = get_oob_nic_boot_options_response.get_primary_dns();
        if (!primary_dns.empty() && !is_not_specified(primary_dns)) {
            boot.set_primary_dns(primary_dns);
        }

        const auto& secondary_dns = get_oob_nic_boot_options_response.get_secondary_dns();
        if (!secondary_dns.empty() && !is_not_specified(secondary_dns)) {
            boot.set_secondary_dns(secondary_dns);
        }
    }
}


void GrantleyNetworkDeviceFunctionBuilder::update_oob_target_boot_options(
    agent_framework::model::NetworkDeviceFunction& network_device_function,
    const ipmi::command::sdv::response::GetOobTargetBootOptions& get_oob_target_boot_options,
    const std::string& primary_target_name,
    const std::string& chap_username,
    const std::string& chap_secret,
    const std::string& mutual_chap_username,
    const std::string& mutual_chap_secret) {
    auto boot = network_device_function.get_iscsi_boot();

    // DHCP can supply target's name, address, port and LUN
    bool target_info_via_dhcp = get_oob_target_boot_options.get_dhcp_enabled();
    boot.set_target_info_via_dhcp(target_info_via_dhcp);

    if (!target_info_via_dhcp) {
        if (!primary_target_name.empty()) {
            boot.set_primary_target_name(primary_target_name);
        }

        const auto& primary_target_address = get_oob_target_boot_options.get_address();
        if (!primary_target_address.empty() && !is_not_specified(primary_target_address)) {
            boot.set_primary_target_address(primary_target_address);
        }

        boot.set_primary_target_port(get_oob_target_boot_options.get_port());

        if (get_oob_target_boot_options.get_lun() > std::numeric_limits<uint32_t>::max()) {
            throw std::runtime_error{"Target LUN is out of accepted range."};
        }
        boot.set_primary_lun(uint32_t(get_oob_target_boot_options.get_lun()));
    }

    boot.set_authentication_method(chap_type_to_enum(get_oob_target_boot_options.get_chap_type()));

    if (!chap_username.empty()) {
        boot.set_chap_username(chap_username);
    }

    if (!chap_secret.empty()) {
        boot.set_chap_secret(chap_secret);
    }

    if (!mutual_chap_username.empty()) {
        boot.set_mutual_chap_username(mutual_chap_username);
    }

    if (!mutual_chap_secret.empty()) {
        boot.set_mutual_chap_secret(mutual_chap_secret);
    }
}
