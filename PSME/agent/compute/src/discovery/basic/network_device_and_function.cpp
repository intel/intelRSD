/*!
 * @brief Definitions of functions creating Network Devices and Functions for Compute agent
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
 * @file network_device_and_function.cpp
 */

#include <limits>
#include <cstdio>

#include "discovery/common.hpp"
#include "discovery/basic/network_device_and_function.hpp"
#include "ipmi/management_controller.hpp"
#include "ipmi/command/sdv/get_oob_control_boot_options.hpp"
#include "ipmi/command/sdv/get_oob_initiator_boot_options.hpp"
#include "ipmi/command/sdv/get_oob_nic_boot_options.hpp"
#include "ipmi/command/sdv/get_oob_target_boot_options.hpp"
#include "ipmi/command/sdv/get_oob_heap_boot_options.hpp"
#include "ipmi/command/sdv/set_oob_heap_boot_options.hpp"

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace ipmi::command::sdv;

namespace {


constexpr const size_t OOB_DISCOVERY_ATTEMPTS{3};


class BadCompletionCodeException final : std::exception {
public:
    BadCompletionCodeException(uint8_t completion_code)
        : m_cc{completion_code},
          what_string{"IPMI: Bad completion code : " + std::to_string(uint32_t(m_cc))} { }

    uint8_t get_completion_code() const { return m_cc ;}

    const char* what() const noexcept {
        return what_string.c_str();
    }
private:
    uint8_t m_cc;
    std::string what_string;
};


void check_completion_code(const ipmi::Response& response) {
    const auto cc = response.get_completion_code();
    if (ipmi::Response::COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
        throw BadCompletionCodeException(cc);
    }
}

std::string read_from_heap(ipmi::ManagementController& mc, int off, int len) {
    uint16_t offset = uint16_t(off);
    uint16_t length = uint16_t(len);
    ipmi::command::sdv::request::GetOobHeapBootOptions request{offset, length};
    ipmi::command::sdv::response::GetOobHeapBootOptions response{};

    mc.send(request, response);
    check_completion_code(response);

    std::vector<uint8_t> heap_string = response.get_heap();

    if (heap_string.size() != length) {
        std::string message{
            "Unable to fetch Heap string from OOB Boot Options. Expected " + std::to_string(uint32_t(length))
            + " bytes but received " + std::to_string(heap_string.size()) + " bytes."};
        log_error(GET_LOGGER("agent"), message);
        throw std::runtime_error{message};
    }

    return std::string(heap_string.begin(), heap_string.end());
}

std::string get_string_from_heap(ipmi::ManagementController& mc, uint16_t offset, uint16_t length) {
    int MAX_BLOCK_SIZE = ipmi::command::sdv::request::SetOobHeapBootOptions::MAX_BLOCK_SIZE;
    std::stringstream ss;
    int blocks = length / MAX_BLOCK_SIZE;
    int delta = length % MAX_BLOCK_SIZE;

    for (uint16_t block = 0; block < blocks; ++block) {
        ss << read_from_heap(mc, offset + block * MAX_BLOCK_SIZE, MAX_BLOCK_SIZE);
    }
    if (delta != 0) {
        ss << read_from_heap(mc, offset + blocks * MAX_BLOCK_SIZE, delta);
    }

    return ss.str();
}

void send_valid_flag(ipmi::ManagementController& mc) {
    // To move the BIOS out of UPDATE_IN_PROGRESS state, send a request that carries no data,
    // just the validity flag, which enables reading BIOS again
    request::SetOobHeapBootOptions request{};
    request.set_params_ready(true);

    response::SetOobHeapBootOptions response{};
    mc.send(request, response);
    check_completion_code(response);
}

enums::FunctionAuthenticationMethod chap_type_to_enum(response::GetOobTargetBootOptions::ChapType type) {
    switch (type) {
        case response::GetOobTargetBootOptions::ChapType::None : return enums::FunctionAuthenticationMethod::None;
        case response::GetOobTargetBootOptions::ChapType::OneWayChap : return enums::FunctionAuthenticationMethod::CHAP;
        case response::GetOobTargetBootOptions::ChapType::MutualChap : return enums::FunctionAuthenticationMethod::MutualCHAP;
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

void discover_function(ipmi::ManagementController& mc, NetworkDeviceFunction& function) {
    request::GetOobControlBootOptions control_request{};
    response::GetOobControlBootOptions control_response{};
    request::GetOobInitiatorBootOptions initiator_request{};
    response::GetOobInitiatorBootOptions initiator_response{};
    request::GetOobNicBootOptions nic_request{};
    response::GetOobNicBootOptions nic_response{};
    request::GetOobTargetBootOptions target_request{};
    response::GetOobTargetBootOptions target_response{};
    mc.send(control_request, control_response);
    check_completion_code(control_response);
    mc.send(initiator_request, initiator_response);
    check_completion_code(initiator_response);
    mc.send(nic_request, nic_response);
    check_completion_code(nic_response);
    mc.send(target_request, target_response);
    check_completion_code(target_response);

    function.set_device_enabled(control_response.get_bios_enabled());
    if (!nic_response.get_mac_address().empty() && !is_default_nic(nic_response.get_mac_address())) {
        function.set_mac_address(nic_response.get_mac_address());
    }

    attribute::IscsiBoot boot;
    if (nic_response.get_dhcp_version() == response::GetOobBootOptions::IPVersion::IPV6) { // no equivalent for AUTO value in metadata enum
        boot.set_ip_address_type(enums::IPAddressType::IPv6);
    }

    const auto& initiator_address = nic_response.get_address();
    boot.set_initiator_address(is_not_specified(initiator_address) ? "" : initiator_address);

    if (initiator_response.get_name_length() != 0) {
        boot.set_initiator_name(
            get_string_from_heap(mc, initiator_response.get_name_offset(), initiator_response.get_name_length()));
    }

    const auto& gateway = nic_response.get_gateway();
    if (!gateway.empty() && !is_not_specified(gateway)) {
        boot.set_initiator_default_gateway(gateway);
    }

    const auto& initiator_netmask = nic_response.get_subnet();
    boot.set_initiator_netmask(is_not_specified(initiator_netmask) ? "" : initiator_netmask);

    // DHCP can supply target's name, address, port and LUN
    bool target_info_via_dhcp = target_response.get_dhcp_enabled();
    boot.set_target_info_via_dhcp(target_info_via_dhcp);

    if (!target_info_via_dhcp) {
        if (target_response.get_target_name_length() != 0) {
            boot.set_primary_target_name(
                get_string_from_heap(mc, target_response.get_target_name_offset(),
                                     target_response.get_target_name_length()));
        }

        const auto& primary_target_address = target_response.get_address();
        if (!primary_target_address.empty() && !is_not_specified(primary_target_address)) {
            boot.set_primary_target_address(primary_target_address);
        }

        boot.set_primary_target_port(target_response.get_port());

        if (target_response.get_lun() > std::numeric_limits<uint32_t>::max()) {
            throw std::runtime_error{"Target LUN is out of accepted range."};
        }
        boot.set_primary_lun(uint32_t(target_response.get_lun()));
    }

    bool primary_vlan_enabled = nic_response.get_vlan_enabled();
    boot.set_primary_vlan_enable(primary_vlan_enabled);
    if (primary_vlan_enabled) {
        boot.set_primary_vlan_id(nic_response.get_vlan_id());
    }

    // DHCP can supply primary and secondary DNS addresses
    bool ip_mask_dns_via_dhcp = nic_response.get_dhcp_enabled();
    boot.set_ip_mask_dns_via_dhcp(ip_mask_dns_via_dhcp);

    if (!ip_mask_dns_via_dhcp) {
        const auto& primary_dns = nic_response.get_primary_dns();
        if (!primary_dns.empty() && !is_not_specified(primary_dns)) {
            boot.set_primary_dns(primary_dns);
        }

        const auto& secondary_dns = nic_response.get_secondary_dns();
        if (!secondary_dns.empty() && !is_not_specified(secondary_dns)) {
            boot.set_secondary_dns(secondary_dns);
        }
    }

    boot.set_authentication_method(chap_type_to_enum(target_response.get_chap_type()));

    if (target_response.get_chapname_length() != 0) {
        boot.set_chap_username(get_string_from_heap(mc, target_response.get_chapname_offset(), target_response.get_chapname_length()));
    }

    if (target_response.get_chapsecret_length() != 0) {
        boot.set_chap_secret(get_string_from_heap(mc, target_response.get_chapsecret_offset(), target_response.get_chapsecret_length()));
    }

    if (target_response.get_mutual_chapname_length() != 0) {
        boot.set_mutual_chap_username(get_string_from_heap(mc, target_response.get_mutual_chapname_offset(), target_response.get_mutual_chapname_length()));
    }

    if (target_response.get_mutual_chapsecret_length() != 0) {
        boot.set_mutual_chap_secret(get_string_from_heap(mc, target_response.get_mutual_chapsecret_offset(), target_response.get_mutual_chapsecret_length()));
    }

    function.set_iscsi_boot(boot);
}

std::tuple<bool, agent_framework::model::enums::Health> discover_network_device_function(
    ipmi::ManagementController& mc, const std::string& network_device_uuid) {

    NetworkDeviceFunction function{network_device_uuid};

    enums::Health health = enums::Health::OK;

    try {
        bool discovery_success = false;
        for(size_t i = 0; i < OOB_DISCOVERY_ATTEMPTS; ++i) {
            try {
                discover_function(mc, function);
                discovery_success = true;
            }
            catch (const BadCompletionCodeException& ex) {
                if (ipmi::Response::COMPLETION_CODE::COMPLETION_CODE_UPDATE_IN_PROGRESS == ex.get_completion_code()) {
                    log_debug(GET_LOGGER("basic-discovery"), "COMPLETION_CODE_UPDATE_IN_PROGRESS.\n"
                        "Network device function discovery failed. Attempting to set validity flag...");
                    try {
                        send_valid_flag(mc);
                    }
                    catch (const BadCompletionCodeException&) { } // Do nothing, just make another attempt at discovery.
                } else { // other completion codes
                    log_error(GET_LOGGER("agent"), ex.what());
                    throw;
                }
            }
            if (discovery_success) {
                break;
            }
        }
        // check if all attempts failed
        if (!discovery_success) {
            throw std::runtime_error("Failed to discover network device function in "
                                     + std::to_string(OOB_DISCOVERY_ATTEMPTS) + " attempts!");
        }
    }
    catch (const std::exception& e) {
        health = enums::Health::Warning;
        log_error(GET_LOGGER("basic-discovery"), "Could not discover iSCSI OOB attributes for function "
            << function.get_uuid() << ". Exception: " << e.what());
    }

    function.set_status({agent_framework::model::enums::State::Enabled, health});
    ComputeComponents::get_instance()->
        get_network_device_function_manager().add_entry(function);
    log_debug(GET_LOGGER("basic-discovery"), "Adding network device function: " << function.get_uuid());
    log_info(GET_LOGGER("basic-discovery"), "iSCSI OOB Discovery was successful.");
    return std::make_tuple(enums::Health::OK == health ? true : false, health);
}

}

std::tuple<bool, agent_framework::model::enums::Health> agent::compute::discovery::basic::discover_network_device(
    ipmi::ManagementController& mc, agent_framework::model::System& system) {
    bool status{false};
    enums::Health health = enums::Health::OK;

    NetworkDevice device{system.get_uuid()};
    device.set_status({
                          agent_framework::model::enums::State::Enabled,
                          agent_framework::model::enums::Health::OK
                      });
    device.add_collection(attribute::Collection(
        enums::CollectionName::NetworkDeviceFunctions,
        enums::CollectionType::NetworkDeviceFunctions,
        ""
    ));

    ComputeComponents::get_instance()->
        get_network_device_manager().add_entry(device);
    log_debug(GET_LOGGER("basic-discovery"), "Adding network device: " << device.get_uuid());

    std::tie(status, health) = discover_network_device_function(mc, device.get_uuid());
    update_health(device, health);

    return std::make_tuple(enums::Health::OK == health ? true : false, health);

}
