/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION

 @file submodule.hpp
 @brief Submodule declaration.
*/

#include "agent-framework/module/submodule.hpp"
#include "agent-framework/logger_ext.hpp"

#include "json/json.hpp"

#include <safe-string/safe_lib.hpp>

#include <algorithm>
#include <exception>

using namespace agent_framework::generic;

Submodule::~Submodule(){}

void Submodule::read_configuration(const json::Value& submod_configuration) {
    try {

        log_debug(GET_LOGGER("module"), "Reading blade configuration.");
        m_configured_port = m_port = submod_configuration["port"].as_uint();

        if (submod_configuration["type"].is_string()) {
            m_type = submod_configuration["type"].as_string();
        }

        if (submod_configuration["slot"].is_number()) {
            m_slot = submod_configuration["slot"].as_uint();
        }

        if (submod_configuration["storageControllers"].is_array()) {
            m_read_storage_controllers_configuration(
                submod_configuration["storageControllers"]);
        }

        if (submod_configuration["networkInterfaces"].is_array()) {
            m_read_network_interfaces_configuration(
                submod_configuration["networkInterfaces"]);
        }

        if (submod_configuration["iscsi"].is_object()) {
            m_read_iscsi_configuration(submod_configuration["iscsi"]);
        }

        if (submod_configuration["ports"].is_array()) {
            m_read_ports(submod_configuration["ports"]);
        }

        if (submod_configuration["vlans"].is_array()) {
            m_read_vlans(submod_configuration["vlans"]);
            m_read_vlanports(submod_configuration["vlans"]);
        }
        if (submod_configuration["public_vlans"].is_array()) {
            m_read_public_vlans(submod_configuration["public_vlans"]);
        }
    }
    catch (const json::Value::Exception& e) {
        log_warning(GET_LOGGER("module"), "Invalid/missing submodule configuration member: "
                  << e.what());
    }
    catch (...) {
        log_alert(GET_LOGGER("module"), "Unknown error in submodule section");
    }
}

void Submodule::m_read_storage_controllers_configuration(
    const json::Value& controllers_configuration) {

    for (const auto& controller_json : controllers_configuration.as_array()) {
        auto controller = StorageController::make_storage_controller();
        controller->read_configuration(controller_json);
        add_storage_controller(std::move(controller));
    }
}

void Submodule::m_read_network_interfaces_configuration(
    const json::Value& interfaces_configuration) {

    for (const auto& interface_json : interfaces_configuration.as_array()) {
        auto interface = NetworkInterface::make_network_interface();
        interface->read_configuration(interface_json);
        add_network_interface(std::move(interface));
    }
}

void Submodule::m_read_iscsi_configuration(
                                    const json::Value& iscsi_configuration) {
    try {
        IscsiData iscsi_data{};
        iscsi_data.set_portal_ip(m_get_interface_ip(
                iscsi_configuration["portal-interface"].as_string()));
        iscsi_data.set_portal_port(iscsi_configuration["port"].as_uint());
        iscsi_data.set_username(iscsi_configuration["username"].as_string());
        iscsi_data.set_password(iscsi_configuration["password"].as_string());
        iscsi_data.set_initiator(iscsi_configuration["initiator"].as_string());
        iscsi_data.set_configuration_path(iscsi_configuration["config-path"].as_string());

        set_iscsi_data(std::move(iscsi_data));

    } catch (const json::Value::Exception& error) {
        log_error(GET_LOGGER("module"),
                "Invalid iscsi configuration data!" << error.what());
    }
}

void Submodule::m_read_ports(const json::Value& ports_configuration) {
    for(const auto& port_json : ports_configuration.as_array()) {
        auto port = Port::make_port();
        port->read_configuration(port_json);
        add_switch_port(std::move(port));
    }
}

void Submodule::m_read_vlans(const json::Value& vlans_configuration) {
    for (const auto& vlan_json : vlans_configuration.as_array()){
        auto vlan = Vlan::make_vlan();
        vlan->read_configuration(vlan_json);
        add_vlan(std::move(vlan));
    }
}

void Submodule::m_read_public_vlans(const json::Value& public_vlans_configuration) {
    for (size_t i =0; i < public_vlans_configuration.size(); ++i){
        m_public_vlans.push_back(public_vlans_configuration[i].as_uint());
    }
}

void Submodule::m_read_vlanports(const json::Value& vlans_configuration) {
    for (const auto& vlan_json : vlans_configuration.as_array()) {
        auto vlan = m_get_vlan_by_id(vlan_json["id"].as_uint());

        if (nullptr != vlan) {
            log_debug(GET_LOGGER("module"), "Reading Vlanport configuration.");
            // Read tagged ports
            for (const auto& tagged_port : vlan_json["tagged_ports"].as_array()) {
                auto port = m_get_port_by_id(tagged_port.as_uint());
                auto vlanport = VlanPort::make_vlanport();
                vlanport->attach_port(port);
                vlanport->attach_vlan(vlan);
                vlanport->set_tagging(true);
                add_vlanport(std::move(vlanport));
            }

            // Untagged ports
            for (const auto& untagged_port : vlan_json["untagged_ports"].as_array()) {
                auto port = m_get_port_by_id(untagged_port.as_uint());
                auto vlanport = VlanPort::make_vlanport();
                vlanport->attach_port(port);
                vlanport->attach_vlan(vlan);
                vlanport->set_tagging(false);
                add_vlanport(std::move(vlanport));
            }
        }
    }
}

VlanSharedPtr Submodule::m_get_vlan_by_id(const uint32_t vlan_id) {
        auto vlans_compare_func = [vlan_id] (const VlanSharedPtr vlan) {
            return vlan_id == vlan->get_id();
        };
        auto vlan_ptr = std::find_if(m_vlans.cbegin(), m_vlans.cend(), vlans_compare_func);

        if (m_vlans.cend() == vlan_ptr) {
            THROW(::agent_framework::exceptions::InvalidParameters, "rpc",
                "Vlan with id = '" + std::to_string(vlan_id) + "' not found.");
        }
        return *vlan_ptr;
}

PortSharedPtr Submodule::m_get_port_by_id(const uint32_t port_id) {
        auto ports_compare_func = [port_id] (const PortSharedPtr port) {
            return port_id == port->get_id();
        };
        auto port_ptr = std::find_if(m_ports.cbegin(), m_ports.cend(), ports_compare_func);

        if (m_ports.cend() == port_ptr) {
            THROW(::agent_framework::exceptions::InvalidParameters, "rpc",
                "Port with id = '" + std::to_string(port_id) + "' not found.");
        }
        return *port_ptr;
}

std::vector<HardDriveSharedPtr> Submodule::get_hard_drives() const {
    std::vector<HardDriveSharedPtr> hard_drives;
    const auto& storage_controllers = get_storage_controllers();
    for (const auto& storage_controller : storage_controllers) {
        const auto& controller_drives = storage_controller->get_hard_drives();
        hard_drives.reserve(hard_drives.size() + controller_drives.size());
        std::copy(controller_drives.cbegin(), controller_drives.cend(),
                  std::back_inserter(hard_drives));
    }
    return hard_drives;
}

const std::vector<LogicalDriveSharedPtr> Submodule::get_logical_drives() const {

    std::vector<LogicalDriveSharedPtr> result;

    std::function<void(LogicalDriveSharedPtr)> get_all_drives;
    get_all_drives = [&](LogicalDriveSharedPtr drive)
    {
        result.push_back(drive);
        auto first = std::begin(drive->get_logical_drives());
        auto last = std::end(drive->get_logical_drives());
        std::for_each(first, last, get_all_drives);
    };

    std::for_each(std::begin(m_logical_drives), std::end(m_logical_drives), get_all_drives);

    return result;
}

HardDriveWeakPtr Submodule::find_hard_drive(const std::string& uuid) const {

    const auto& storage_controllers = get_storage_controllers();
    for (const auto& storage_controller : storage_controllers) {
        const auto& hard_drive = storage_controller->find_hard_drive(uuid);
        if (!hard_drive.expired()) {
            return hard_drive;
        }
    }
    return {};
}

LogicalDriveWeakPtr Submodule::find_logical_drive(const std::string& uuid) const {
    for (const auto& logical_drive : get_logical_drives()) {
        if (logical_drive->get_uuid() == uuid) {
            return logical_drive;
        }
        else {
            auto sub_logical_drive = logical_drive->find_logical_drive(uuid);
            if (!sub_logical_drive.expired()) {
                return sub_logical_drive;
            }
        }
    }
    return {};
}

Target::TargetWeakPtr Submodule::find_target(const std::string& uuid) const {
    return get_target_manager().find_by_uuid(uuid);
}

const std::string Submodule::m_get_interface_ip(const std::string& interface) {
    struct ifaddrs *ifah = nullptr;
    struct sockaddr_in sockaddress;
    char *addr = nullptr;

    if(0 != getifaddrs(&ifah)){
        log_error(GET_LOGGER("module"),
            "Could not read IP address from interface " << interface << ".");
        return addr;
    }

    for (struct ifaddrs *ifa = ifah; ifa; ifa = ifa->ifa_next) {
        if (AF_INET == ifa->ifa_addr->sa_family) {
            if (0 != interface.compare(ifa->ifa_name)) continue;

            if (EOK != memcpy_s(&sockaddress, sizeof(sockaddress),
                                ifa->ifa_addr, sizeof(*(ifa->ifa_addr)))) {
                freeifaddrs(ifah);
                throw std::runtime_error("cannot copy memory");
            }

            addr = inet_ntoa(sockaddress.sin_addr);
            if(nullptr == addr) break;
            log_debug(GET_LOGGER("module"),
                    "Read portal IP address " << addr << ", from interface " << interface << ".");
            break;
        }
    }

    freeifaddrs(ifah);
    if (nullptr == addr) log_error(GET_LOGGER("module"),
            "Could not read IP address from interface " << interface << ".");
    return addr;
}
