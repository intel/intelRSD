/*!
 * @brief Definition of function processing Set Component Attributes command on Network Device Function
 *
 * @copyright Copyright (c) 2017 Intel Corporation
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
 * @header{Files}
 * @file set_network_device_function_attributes.cpp
 */

#include "utils/compute_conversions.hpp"
#include "agent-framework/module/requests/validation/compute.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "command/set_network_device_function_attributes.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/set_oob_control_boot_options.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/set_oob_heap_boot_options.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/set_oob_initiator_boot_options.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/set_oob_nic_boot_options.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/set_oob_target_boot_options.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/set_oob_reset_boot_options.hpp"
#include "ipmi/command/generic/enums.hpp"
#include "ipmi/utils/sdv/mdr_region_accessor.hpp"

#include "iscsi/structs/iscsi_mdr_header.hpp"
#include "iscsi/structs/iscsi_mdr_target.hpp"

#include "iscsi/iscsi_mdr_builder.hpp"

#include "json-wrapper/json-wrapper.hpp"

#include <functional>

using namespace agent_framework::exceptions;
using namespace agent_framework::model;
using namespace agent_framework::model::requests::validation;
using namespace agent_framework::module;
using namespace ipmi::manager::ipmitool;
using namespace ipmi::command::sdv;
using Heap = std::vector<std::uint8_t>;


namespace {

struct DeviceFunctionContext final {

    /*!
     * @brief Constructor
     */

    DeviceFunctionContext(const NetworkDeviceFunction& network_device_function) :
        m_net_dev{network_device_function} {
    }

    /*!
     * @brief Deleted copy constructor.
     */
    DeviceFunctionContext(const DeviceFunctionContext&) = delete;

    /*!
     * @brief Deleted move constructor.
     */
    DeviceFunctionContext(DeviceFunctionContext&&) = delete;

    /*!
     * @brief Deleted copy assignment.
     */
    DeviceFunctionContext& operator=(const DeviceFunctionContext&) = delete;

    /*!
     * @brief Deleted move assignment.
     */
    DeviceFunctionContext& operator=(DeviceFunctionContext&&) = delete;

    /*!
     * @brief Default destructor.
     */
    ~DeviceFunctionContext() = default;

    /*!
     * @brief Convert model IPAddressType to its iSCSI MDR counterpart.
     *
     * @param at the model IP address type.
     * @return iSCSI MDR IP address type.
     */
    iscsi::structs::IpAddressType get_ip_type(const agent_framework::model::enums::IPAddressType& at) const {
        return agent::compute::utils::IscsiMdrConverter::get_ip_type(at);
    }

    /*!
     * @brief Convert model DHCP status to its iSCSI MDR counterpart.
     *
     * @param enabled the DHCP status.
     * @return iSCSI MDR DHCP status.
     */
    iscsi::structs::TargetDhcpStatus get_dhcp_enabled(bool enabled) const {
        return agent::compute::utils::IscsiMdrConverter::get_dhcp_enabled(enabled);
    }

    /*!
     * @brief Convert model VLAN status to its iSCSI MDR counterpart.
     *
     * @param enabled the VLAN status.
     * @return iSCSI MDR VLAN status.
     */
    iscsi::structs::VlanStatus get_vlan_enabled(bool enabled) const {
        return agent::compute::utils::IscsiMdrConverter::get_vlan_enabled(enabled);
    }

    /*!
     * @brief Convert model router advertisement status to its iSCSI MDR counterpart.
     *
     * @param enabled the router advertisement status.
     * @return iSCSI MDR router advertisement status.
     */
    iscsi::structs::RouterAdvertisement get_router_advertisment(bool enabled) const {
        return agent::compute::utils::IscsiMdrConverter::get_router_advertisment(enabled);
    }

    /*!
     * @brief Convert model authentication method to its iSCSI MDR counterpart.
     *
     * @param am the model authentication method.
     * @return iSCSI MDR authentication method.
     */
    iscsi::structs::AuthenticationMethod get_authentication_method(
        const agent_framework::model::enums::FunctionAuthenticationMethod& am) const {
        return agent::compute::utils::IscsiMdrConverter::get_authentication_method(am);
    }

    /*!
     * @brief Get the network device function.
     *
     * @return the network device function.
     */
    const NetworkDeviceFunction& get_network_device_function() const {
        return m_net_dev;
    }

private:
    const NetworkDeviceFunction& m_net_dev;
};

void clear_data(ManagementController& mc) {
    request::SetOobResetBootOptions request{};
    response::SetOobResetBootOptions response{};

    request.set_params_ready(true);
    try {
        mc.send(request, response);
    }
    catch (const ipmi::ResponseError& response_error) {
        THROW(IpmiError, "compute-agent", std::string("Error Sending SetOobClearBootOptions On CC: ")
                                          + response_error.what());
    }
}


void send_control_data(ManagementController& mc, const NetworkDeviceFunction& function) {

    ipmi::command::sdv::request::SetOobControlBootOptions request{};
    ipmi::command::sdv::response::SetOobControlBootOptions response{};

    request.set_bios_enabled(function.get_device_enabled());
    request.set_params_ready(true);
    try {
        mc.send(request, response);
    }
    catch (const ipmi::ResponseError& response_error) {
        THROW(IpmiError, "compute-agent", std::string("Error Sending SetOobControlBootOptions On CC: ")
                                          + response_error.what());
    }
}


void send_initiator_data(ManagementController& mc, const NetworkDeviceFunction& function, Heap& heap) {
    const auto& iscsi_boot = function.get_iscsi_boot();
    request::SetOobInitiatorBootOptions request{};
    response::SetOobInitiatorBootOptions response{};

    const auto& initiator_name = iscsi_boot.get_initiator_name();
    request.set_offset(uint16_t(heap.size()));
    request.set_length(uint16_t(initiator_name.size()));
    heap.insert(heap.end(), initiator_name.begin(), initiator_name.end());

    try {
        mc.send(request, response);
    }
    catch (const ipmi::ResponseError& response_error) {
        THROW(IpmiError, "compute-agent", std::string("Error Sending SetOobInitiatorBootOptions On CC: ")
                                          + response_error.what());
    }
}


void send_nic_data(ManagementController& mc, const NetworkDeviceFunction& function) {
    const auto& iscsi_boot = function.get_iscsi_boot();
    request::SetOobNicBootOptions request{};
    response::SetOobNicBootOptions response{};

    if (iscsi_boot.get_ip_address_type() == enums::IPAddressType::IPv6) {
        request.set_dhcp_ip_version(ipmi::command::sdv::request::SetOobNicBootOptions::IPVersion::IPV6);
    }
    else {
        request.set_dhcp_ip_version(ipmi::command::sdv::request::SetOobNicBootOptions::IPVersion::IPV4);
    }

    const auto dhcp_enabled = iscsi_boot.get_ip_mask_dns_via_dhcp();
    request.set_dhcp_enabled(dhcp_enabled);

    if (!dhcp_enabled) {
        request.set_address(iscsi_boot.get_initiator_address());
        request.set_subnet(iscsi_boot.get_initiator_netmask());

        if (iscsi_boot.get_initiator_default_gateway().has_value()) {
            request.set_gateway(iscsi_boot.get_initiator_default_gateway());
        }
        if (iscsi_boot.get_primary_dns().has_value()) {
            request.set_primary_dns(iscsi_boot.get_primary_dns());
        }
        if (iscsi_boot.get_secondary_dns().has_value()) {
            request.set_secondary_dns(iscsi_boot.get_secondary_dns());
        }
    }

    request.set_vlan_enabled(iscsi_boot.get_primary_vlan_enable());
    if (iscsi_boot.get_primary_vlan_id().has_value()) {
        request.set_vlan(uint16_t(iscsi_boot.get_primary_vlan_id()));
    }
    if (function.get_mac_address().has_value()) {
        request.set_mac(function.get_mac_address());
    }

    try {
        mc.send(request, response);
    }
    catch (const ipmi::ResponseError& response_error) {
        THROW(IpmiError, "compute-agent", std::string("Error Sending SetOobNicBootOptions On CC: ")
                                          + response_error.what());
    }
    catch (std::runtime_error& error) {
        THROW(IpmiError, "compute-agent", error.what());
    }
}


void send_target_data(ManagementController& mc, const NetworkDeviceFunction& function, Heap& heap) {
    const auto& iscsi_boot = function.get_iscsi_boot();
    request::SetOobTargetBootOptions request{};
    response::SetOobTargetBootOptions response{};

    const auto target_via_dhcp = iscsi_boot.get_target_info_via_dhcp();
    request.set_dhcp_enabled(target_via_dhcp);

    if (!target_via_dhcp) {
        if (iscsi_boot.get_primary_target_name().has_value()) {
            const auto& target_name = iscsi_boot.get_primary_target_name().value();
            request.set_name_offset(uint16_t(heap.size()));
            request.set_name_length(uint16_t(target_name.size()));
            heap.insert(heap.end(), target_name.begin(), target_name.end());
        }
        if (iscsi_boot.get_primary_target_address().has_value()) {
            request.set_address(iscsi_boot.get_primary_target_address());
        }
        if (iscsi_boot.get_primary_target_port().has_value()) {
            request.set_port(uint16_t(iscsi_boot.get_primary_target_port()));
        }
        if (iscsi_boot.get_primary_lun().has_value()) {
            request.set_lun(iscsi_boot.get_primary_lun());
        }
    }

    const auto chap_type = iscsi_boot.get_authentication_method();
    using ChapType = request::SetOobTargetBootOptions::ChapType;
    switch (chap_type) {
        case enums::FunctionAuthenticationMethod::None : request.set_chap_type(ChapType::None); break;
        case enums::FunctionAuthenticationMethod::CHAP : request.set_chap_type(ChapType::OneWayChap); break;
        case enums::FunctionAuthenticationMethod::MutualCHAP : request.set_chap_type(ChapType::MutualChap); break;
        default:
            throw std::runtime_error{"Tried to send an unknown authentication type " + std::to_string(chap_type)};
    }

    if (iscsi_boot.get_chap_username().has_value()) {
        const auto& chap_username = iscsi_boot.get_chap_username().value();
        request.set_chap_name_offset(uint16_t(heap.size()));
        request.set_chap_name_length(uint16_t(chap_username.size()));
        heap.insert(heap.end(), chap_username.begin(), chap_username.end());
    }

    if (iscsi_boot.get_chap_secret().has_value()) {
        const auto& chap_secret = iscsi_boot.get_chap_secret().value();
        request.set_chap_secret_offset(uint16_t(heap.size()));
        request.set_chap_secret_length(uint16_t(chap_secret.size()));
        heap.insert(heap.end(), chap_secret.begin(), chap_secret.end());
    }

    if (iscsi_boot.get_mutual_chap_username().has_value()) {
        const auto& mutual_chap_username = iscsi_boot.get_mutual_chap_username().value();
        request.set_mutual_chap_name_offset(uint16_t(heap.size()));
        request.set_mutual_chap_name_length(uint16_t(mutual_chap_username.size()));
        heap.insert(heap.end(), mutual_chap_username.begin(), mutual_chap_username.end());
    }

    if (iscsi_boot.get_mutual_chap_secret().has_value()) {
        const auto& mutual_chap_secret = iscsi_boot.get_mutual_chap_secret().value();
        request.set_mutual_chap_secret_offset(uint16_t(heap.size()));
        request.set_mutual_chap_secret_length(uint16_t(mutual_chap_secret.size()));
        heap.insert(heap.end(), mutual_chap_secret.begin(), mutual_chap_secret.end());
    }

    try {
        mc.send(request, response);
    }
    catch (const ipmi::ResponseError& response_error) {
        THROW(IpmiError, "compute-agent", std::string("Error Sending SetOobTargetBootOptions On CC: ")
                                          + response_error.what());
    }
    catch (std::runtime_error& error) {
        THROW(IpmiError, "compute-agent", error.what());
    }
}


void send_heap_data(ManagementController& mc, const NetworkDeviceFunction&, Heap& heap) {
    int MAX_BLOCK_SIZE = ipmi::command::sdv::request::SetOobHeapBootOptions::MAX_BLOCK_SIZE;
    auto send_oob_heap_chunk = [&](uint16_t offset, uint16_t length) {
        request::SetOobHeapBootOptions request{};
        response::SetOobHeapBootOptions response{};
        request.set_offset(offset);
        request.set_data({heap.begin() + offset, heap.begin() + offset + length});

        try {
            mc.send(request, response);
        }
        catch (const ipmi::ResponseError& response_error) {
            THROW(IpmiError, "compute-agent", std::string("Error Sending SetOobHeapBootOptions On CC: ")
                                              + response_error.what());
        }
    };

    if (heap.empty()) {
        return;
    }

    size_t blocks = heap.size() / MAX_BLOCK_SIZE;
    size_t delta = heap.size() % MAX_BLOCK_SIZE;

    for (size_t block = 0; block < blocks; block++) {
        auto offset = block * MAX_BLOCK_SIZE;
        send_oob_heap_chunk(uint16_t(offset), uint16_t(MAX_BLOCK_SIZE));
    }

    if (delta > 0) {
        auto offset = blocks * MAX_BLOCK_SIZE;
        send_oob_heap_chunk(uint16_t(offset), uint16_t(heap.size() - offset));
    }
}


bool is_unsupported(const std::string& attribute) {
    static std::set<std::string> unsupported{
        literals::IscsiBoot::SECONDARY_TARGET_NAME,
        literals::IscsiBoot::SECONDARY_TARGET_IP_ADDRESS,
        literals::IscsiBoot::SECONDARY_TARGET_TCP_PORT,
        literals::IscsiBoot::SECONDARY_LUN,
        literals::IscsiBoot::SECONDARY_VLAN_ENABLE,
        literals::IscsiBoot::SECONDARY_VLAN_ID,
        literals::IscsiBoot::ROUTER_ADVERTISEMENT_ENABLED
    };
    return unsupported.count(attribute) > 0;
}


void process_iscsi_boot(NetworkDeviceFunction& function, const json::Json& iscsi_boot,
                        responses::SetComponentAttributes& response) {
    auto boot = function.get_iscsi_boot();
    for (auto it = iscsi_boot.begin(); it != iscsi_boot.end(); it++) {
        const auto& value = it.value();
        try {
            if (is_unsupported(it.key())) {
                THROW(UnsupportedField, "compute-agent", "Setting attribute is not supported.", it.key(), value);
            }
            else {
                if (literals::IscsiBoot::IP_ADDRESS_TYPE == it.key()) {
                    boot.set_ip_address_type(enums::IPAddressType::from_string((value.get<std::string>())));
                }
                else if (literals::IscsiBoot::INITIATOR_IP_ADDRESS == it.key()) {
                    boot.set_initiator_address(value.get<std::string>());
                }
                else if (literals::IscsiBoot::INITIATOR_NAME == it.key()) {
                    boot.set_initiator_name(value.get<std::string>());
                }
                else if (literals::IscsiBoot::INITIATOR_DEFAULT_GATEWAY == it.key()) {
                    boot.set_initiator_default_gateway(value);
                }
                else if (literals::IscsiBoot::INITIATOR_NETMASK == it.key()) {
                    boot.set_initiator_netmask(value.get<std::string>());
                }
                else if (literals::IscsiBoot::TARGET_INFO_VIA_DHCP == it.key()) {
                    boot.set_target_info_via_dhcp(value.get<bool>());
                }
                else if (literals::IscsiBoot::PRIMARY_TARGET_IP_ADDRESS == it.key()) {
                    boot.set_primary_target_address(value);
                }
                else if (literals::IscsiBoot::PRIMARY_TARGET_NAME == it.key()) {
                    boot.set_primary_target_name(value);
                }
                else if (literals::IscsiBoot::PRIMARY_TARGET_TCP_PORT == it.key()) {
                    boot.set_primary_target_port(value);
                }
                else if (literals::IscsiBoot::PRIMARY_LUN == it.key()) {
                    boot.set_primary_lun(value);
                }
                else if (literals::IscsiBoot::PRIMARY_VLAN_ENABLE == it.key()) {
                    boot.set_primary_vlan_enable(value.get<bool>());
                }
                else if (literals::IscsiBoot::PRIMARY_VLAN_ID == it.key()) {
                    boot.set_primary_vlan_id(value);
                }
                else if (literals::IscsiBoot::PRIMARY_DNS == it.key()) {
                    boot.set_primary_dns(value);
                }
                else if (literals::IscsiBoot::SECONDARY_DNS == it.key()) {
                    boot.set_secondary_dns(value);
                }
                else if (literals::IscsiBoot::IP_MASK_DNS_VIA_DHCP == it.key()) {
                    boot.set_ip_mask_dns_via_dhcp(value.get<bool>());
                }
                else if (literals::IscsiBoot::AUTHENTICATION_METHOD == it.key()) {
                    boot.set_authentication_method(enums::FunctionAuthenticationMethod::from_string((value.get<std::string>())));
                }
                else if (literals::IscsiBoot::CHAP_USERNAME == it.key()) {
                    boot.set_chap_username(value);
                }
                else if (literals::IscsiBoot::CHAP_SECRET == it.key()) {
                    boot.set_chap_secret(value);
                }
                else if (literals::IscsiBoot::MUTUAL_CHAP_USERNAME == it.key()) {
                    boot.set_mutual_chap_username(value);
                }
                else if (literals::IscsiBoot::MUTUAL_CHAP_SECRET == it.key()) {
                    boot.set_mutual_chap_secret(value);
                }
            }
        }
        catch (const GamiException& ex) {
            response.add_status({std::string(literals::NetworkDeviceFunction::ISCSI_BOOT) + "/" + it.key(),
                                 ex.get_error_code(), ex.get_message()});
        }
    }

    function.set_iscsi_boot(boot);
}


enums::PlatformType get_platform_from_chassis(const std::string& network_device_uuid) {
    const auto device = get_manager<NetworkDevice>().get_entry(network_device_uuid);
    const auto system = get_manager<System>().get_entry(device.get_parent_uuid());
    const auto chassis = get_manager<Chassis>().get_entry(system.get_chassis());
    return chassis.get_platform();
}

void send_iscsi_oob_parameters(const NetworkDeviceFunction& function, ManagementController& mc) {
    const enums::PlatformType platform = get_platform_from_chassis(function.get_parent_uuid());

    if (platform == enums::PlatformType::GRANTLEY) {
        Heap heap{};
        clear_data(mc);
        send_initiator_data(mc, function, heap);
        send_nic_data(mc, function);
        send_target_data(mc, function, heap);
        send_heap_data(mc, function, heap);
        send_control_data(mc, function);
    }
    else if (platform == enums::PlatformType::PURLEY) {
        ipmi::IpmiInterface::ByteBuffer data{};
        iscsi::builder::IscsiMdrBuilder::build(data, DeviceFunctionContext(function),
            request::SetOobInitiatorBootOptions::PURLEY_DEFAULT_WAIT_TIME,
            request::SetOobInitiatorBootOptions::DEFAULT_RETRY_COUNT);

        try {
            std::shared_ptr<ipmi::sdv::MdrRegionAccessor> accessor = ipmi::sdv::MdrRegionAccessorFactory().create(
                ipmi::command::generic::ProductId::PRODUCT_ID_INTEL_XEON_PURLEY, mc,
                ipmi::command::sdv::DataRegionId::ISCSI_BOOT_OPTIONS);
            accessor->write_mdr_region(data);
        } catch (std::runtime_error& re) {
            THROW(IscsiError, "compute-agent", re.what());
        }
    }
    else {
        THROW(InvalidValue, "compute-agent", "Could not set iSCSI OOB parameters: unsupported platform.");
    }
}

void send_clear_iscsi_oob_parameters(const NetworkDeviceFunction& function, ManagementController& mc) {
    const enums::PlatformType platform = get_platform_from_chassis(function.get_parent_uuid());

    if (platform == enums::PlatformType::GRANTLEY) {
        clear_data(mc);
    }
    else if (platform == enums::PlatformType::PURLEY) {
        // writing only the version structure clears iSCSI boot options
        ipmi::IpmiInterface::ByteBuffer data{};
        iscsi::builder::IscsiMdrBuilder::clear(data, DeviceFunctionContext(function));

        try {
            std::shared_ptr<ipmi::sdv::MdrRegionAccessor> accessor = ipmi::sdv::MdrRegionAccessorFactory().create(
                ipmi::command::generic::ProductId::PRODUCT_ID_INTEL_XEON_PURLEY, mc,
                ipmi::command::sdv::DataRegionId::ISCSI_BOOT_OPTIONS);
            accessor->write_mdr_region(data);
        } catch (std::runtime_error& re) {
            THROW(IscsiError, "compute-agent", re.what());
        }
    }
    else {
        THROW(InvalidValue, "compute-agent", "Could not set iSCSI OOB parameters: unsupported platform.");
    }
}

}

void agent::compute::process_network_device_function(const std::string& uuid,
                                                     const attribute::Attributes& attributes,
                                                     responses::SetComponentAttributes& response) {

    ComputeValidator::validate_set_network_device_function_attributes(attributes);
    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug(GET_LOGGER("compute-agent"), "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }

    auto locked_function = get_manager<NetworkDeviceFunction>().get_entry_reference(uuid);
    auto& function = locked_function.get_raw_ref();

    for (const auto& name : attribute_names) {
        try {
            const auto& value = attributes.get_value(name);
            if (literals::NetworkDeviceFunction::ETHERNET == name) {
                auto it_mac = value.find(literals::NetworkDeviceFunction::MAC_ADDRESS);
                if (it_mac != value.end()) {
                    function.set_mac_address(*it_mac);
                }
            }
            else if (literals::NetworkDeviceFunction::ISCSI_BOOT == name) {
                process_iscsi_boot(function, value, response);
            }
            else if (literals::NetworkDeviceFunction::OEM == name) {
                THROW(UnsupportedField, "compute-agent", "Setting attribute is not supported.", name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }

    if (function.get_device_enabled()) {
        const auto system_uuid = get_manager<NetworkDevice>()
            .get_entry_reference(function.get_parent_uuid())->get_parent_uuid();
        const auto manager_uuid = get_manager<System>().get_entry_reference(system_uuid)->get_parent_uuid();
        auto connection_data = get_manager<Manager>().get_entry_reference(manager_uuid)->get_connection_data();

        ManagementController mc{};
        mc.set_ip(connection_data.get_ip_address());
        mc.set_port(connection_data.get_port());
        mc.set_username(connection_data.get_username());
        mc.set_password(connection_data.get_password());

        log_info(GET_LOGGER("compute-agent"), "Updating iSCSI OOB parameters over IPMI!");
        send_iscsi_oob_parameters(function, mc);
    }
}


void agent::compute::set_iscsi_oob_parameters(const System& system, ManagementController& mc) {
    const auto network_device_uuids = get_manager<NetworkDevice>().get_keys(system.get_uuid());
    if (network_device_uuids.empty() || network_device_uuids.size() > 1) {
        THROW(InvalidUuid, "compute-agent",
              "Invalid number of Network Devices for System: '" + system.get_uuid() + "'!");
    }
    const auto network_function_uuids = get_manager<NetworkDeviceFunction>().get_keys(network_device_uuids.front());
    if (network_function_uuids.empty() || network_function_uuids.size() > 1) {
        THROW(InvalidUuid, "compute-agent",
              "Invalid number of Network Device Functions for Device: '" + network_device_uuids.front() + "'!");
    }
    auto locked_function = get_manager<NetworkDeviceFunction>().get_entry_reference(network_function_uuids.front());
    auto& function = locked_function.get_raw_ref();

    if (!function.get_device_enabled()) {
        log_info(GET_LOGGER("compute-agent"), "Setting iSCSI OOB parameters over IPMI!");
        function.set_device_enabled(true);
        send_iscsi_oob_parameters(function, mc);
    }
}


void agent::compute::clear_iscsi_oob_parameters(const System& system, ManagementController& mc) {
    const auto network_device_uuids = get_manager<NetworkDevice>().get_keys(system.get_uuid());
    if (network_device_uuids.empty() || network_device_uuids.size() > 1) {
        THROW(InvalidUuid, "compute-agent",
              "Invalid number of Network Devices for System: '" + system.get_uuid() + "'!");
    }
    const auto network_function_uuids = get_manager<NetworkDeviceFunction>().get_keys(network_device_uuids.front());
    if (network_function_uuids.empty() || network_function_uuids.size() > 1) {
        THROW(InvalidUuid, "compute-agent",
              "Invalid number of Network Device Functions for Device: '" + network_device_uuids.front() + "'!");
    }
    auto function = get_manager<NetworkDeviceFunction>().get_entry_reference(network_function_uuids.front());

    if (function->get_device_enabled()) {
        log_info(GET_LOGGER("compute-agent"), "Clearing iSCSI OOB parameters over IPMI!");
        function->set_device_enabled(false);
        send_clear_iscsi_oob_parameters(function.get_raw_ref(), mc);
    }
}


bool agent::compute::get_iscsi_enabled(const System& system) {
    const auto network_device_uuids = get_manager<NetworkDevice>().get_keys(system.get_uuid());
    if (network_device_uuids.empty() || network_device_uuids.size() > 1) {
        log_error(GET_LOGGER("compute-agent"),
                  "Invalid number of Network Devices for System: '" + system.get_uuid() + "'!");
        return false;
    }

    const auto network_function_uuids = get_manager<NetworkDeviceFunction>().get_keys(network_device_uuids.front());
    if (network_function_uuids.empty() || network_function_uuids.size() > 1) {
        log_error(GET_LOGGER("compute-agent"),
                  "Invalid number of Network Device Functions for Device: '" + network_device_uuids.front() + "'!");
        return false;
    }

    return get_manager<NetworkDeviceFunction>()
        .get_entry_reference(network_function_uuids.front())->get_device_enabled();
}
