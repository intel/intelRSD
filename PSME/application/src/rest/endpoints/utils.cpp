/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * */

#include "psme/rest/server/parameters.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/utils.hpp"
#include "psme/rest/utils/mapper.hpp"
#include "psme/rest/model/finder.hpp"

#include "agent-framework/module/model/model_chassis.hpp"
#include "agent-framework/module/model/model_compute.hpp"
#include "agent-framework/module/model/model_network.hpp"
#include "agent-framework/module/model/model_storage.hpp"
#include "agent-framework/module/model/model_pnc.hpp"

#include <regex>
#include <cmath>



using namespace psme::rest::constants::Common;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;

namespace {

std::regex make_regex(const std::string& str) {
    return std::regex("\\{" + str + "\\}");
}


bool exists_drawer_or_enclosure_with_parent(GenericManager<agent_framework::model::Chassis>& container,
                                            const std::string& manager_uuid) {
    // lambda used for filtering chassis entries in container
    auto is_drawer_or_enclosure = [](const agent_framework::model::Chassis& chassis) {
        return (chassis.get_type() == enums::ChassisType::Drawer) ||
               (chassis.get_type() == enums::ChassisType::Enclosure);
    };
    auto keys = container.get_keys(manager_uuid, is_drawer_or_enclosure);
    return !keys.empty();
}

}

namespace psme {
namespace rest {
namespace endpoint {
namespace utils {

std::string get_port_uuid_from_url(const std::string& port_url) {
    try {
        auto params = psme::rest::model::Mapper::get_params(port_url, constants::Routes::ETHERNET_SWITCH_PORT_PATH);
        return psme::rest::model::Find<agent_framework::model::EthernetSwitchPort>(
            params[constants::PathParam::SWITCH_PORT_ID])
            .via<agent_framework::model::EthernetSwitch>(params[constants::PathParam::ETHERNET_SWITCH_ID])
            .get_uuid();
    }
    catch (agent_framework::exceptions::NotFound&) {
        THROW(agent_framework::exceptions::InvalidValue, "rest", "Could not find port with URI: " + port_url);
    }
}


std::string make_endpoint_context(const psme::rest::server::Parameters& params, std::string link) {
    for (const auto& param : params) {
        link = std::regex_replace(link, ::make_regex(param.first), param.second);
    }
    return link;
}


std::uint64_t id_to_uint64(const std::string& id_as_string) {
    if (!std::all_of(id_as_string.begin(), id_as_string.end(), ::isdigit)) {
        // A string containing non-digit characters is NOT supposed to reach this method,
        // because the rest server only allows ID strings of the form [0-9]+. So, we log an error.
        log_error(GET_LOGGER("rest"), "String " + id_as_string +
                                      " which contains not-digit characters was received for conversion to REST ID!");

        THROW(agent_framework::exceptions::NotFound, "rest",
              id_as_string + " could not be converted to REST ID, because it's not a number string.");
    }

    std::uint64_t id;
    try {
        id = std::stoull(id_as_string, NULL);
    }
    catch (const std::out_of_range&) {
        THROW(agent_framework::exceptions::NotFound, "rest",
              id_as_string + " could not be converted to REST ID, because it's too big.");
    }
    catch (const std::invalid_argument&) {
        // This is NOT supposed to happen, because the input check at the beginning of this method.
        THROW(agent_framework::exceptions::NotFound, "rest",
              id_as_string + " could not be converted to REST ID, because it's not a proper ID.");
    }
    return id;
}


bool is_manager_for_drawer_or_enclosure(const std::string& manager_uuid) {
    return exists_drawer_or_enclosure_with_parent(
        agent_framework::module::get_manager<agent_framework::model::Chassis>(), manager_uuid);
}


namespace {
template<typename M>
void build_parent_path(endpoint::PathBuilder& path, const std::string& uuid, const std::string& collection_literal) {
    path.append(collection_literal).append(agent_framework::module::get_manager<M>().
        uuid_to_rest_id(uuid));
}

template<typename M>
void build_child_path(endpoint::PathBuilder& path, const std::string& uuid, const std::string& collection_literal) {
    const auto& resource = agent_framework::module::get_manager<M>().get_entry(uuid);
    get_component_url_recursive(path, resource.get_parent_type(),
                                resource.get_parent_uuid());
    path.append(collection_literal).append(resource.get_id());
}
}


void get_component_url_recursive(endpoint::PathBuilder& path, enums::Component type, const std::string& uuid) {
    using Component = enums::Component;
    using namespace psme::rest;
    using namespace agent_framework;

    switch (type) {
        case Component::Chassis:
            build_parent_path<agent_framework::model::Chassis>(path, uuid, constants::Common::CHASSIS);
            break;
        case Component::Manager:
            build_parent_path<agent_framework::model::Manager>(path, uuid, constants::Root::MANAGERS);
            break;
        case Component::Task:
            path.append(constants::TaskService::TASK_SERVICE).append(constants::TaskService::TASKS).append(
                module::get_manager<agent_framework::model::Task>().
                    uuid_to_rest_id(uuid));
            break;
        case Component::System:
            build_parent_path<agent_framework::model::System>(path, uuid, constants::Root::SYSTEMS);
            break;
        case Component::StorageSubsystem:
            build_child_path<agent_framework::model::StorageSubsystem>(path, uuid, constants::System::STORAGE);
            break;
        case Component::StorageServices:
            build_parent_path<agent_framework::model::StorageServices>(path, uuid, constants::Root::SERVICES);
            break;
        case Component::EthernetSwitch:
            build_parent_path<agent_framework::model::EthernetSwitch>(path, uuid, constants::Root::ETHERNET_SWITCHES);
            break;
        case Component::Drive:
            build_child_path<agent_framework::model::Drive>(path, uuid, constants::Chassis::DRIVES);
            break;
        case Component::PhysicalDrive:
            build_child_path<agent_framework::model::PhysicalDrive>(path, uuid, constants::StorageService::DRIVES);
            break;
        case Component::LogicalDrive:
            build_child_path<agent_framework::model::LogicalDrive>(path, uuid, constants::StorageService::LOGICAL_DRIVES);
            break;
        case Component::IscsiTarget:
            build_child_path<agent_framework::model::IscsiTarget>(path, uuid, constants::StorageService::TARGETS);
            break;
        case Component::Memory:
            build_child_path<agent_framework::model::Memory>(path, uuid, constants::System::MEMORY);
            break;
        case Component::Processor:
            build_child_path<agent_framework::model::Processor>(path, uuid, constants::System::PROCESSORS);
            break;
        case Component::NetworkInterface:
            // Warning: the following assumes that no agent has EthernetInterfaces under Managers.
            // It's compliant for our implementation, but not in all cases allowed by GAMI.
            build_child_path<agent_framework::model::NetworkInterface>(path, uuid, constants::System::ETHERNET_INTERFACES);
            break;
        case Component::StorageController:
            build_child_path<agent_framework::model::StorageController>(path, uuid, constants::StorageSubsystem::STORAGE_CONTROLLERS);
            break;
        case Component::EthernetSwitchPort:
            build_child_path<agent_framework::model::EthernetSwitchPort>(path, uuid, constants::EthernetSwitch::PORTS);
            break;
        case Component::EthernetSwitchPortVlan:
            build_child_path<agent_framework::model::EthernetSwitchPortVlan>(path, uuid, constants::EthernetSwitchPort::VLANS);
            break;
        case Component::Zone:
            build_child_path<agent_framework::model::Zone>(path, uuid, constants::Fabric::ZONES);
            break;
        case Component::PcieFunction:
            build_child_path<agent_framework::model::PcieFunction>(path, uuid, constants::PathParam::FUNCTIONS);
            break;
        case Component::PcieDevice: {
            const auto& pcie_device = module::get_manager<agent_framework::model::PcieDevice>().get_entry(uuid);
            // in GAMI the devices are under the a Manager
            // the same manager should have one Chassis child
            // that we use to build the path
            const auto manager_uuid = pcie_device.get_parent_uuid();
            auto& chassis_manager = agent_framework::module::get_manager<agent_framework::model::Chassis>();
            const auto chassis_uuids = chassis_manager.get_keys(manager_uuid);
            if (chassis_uuids.size() != 1) {
                log_error(GET_LOGGER("rest"),
                          "PNC Manager should have precisely one Chassis child!"
                              " Server is unable to build a path to PCIeDevice component.");
                break;
            }
            get_component_url_recursive(path, enums::Component::Chassis, chassis_uuids.front());
            path.append(constants::Chassis::PCIE_DEVICES).append(pcie_device.get_id());
            break;
        }
        case Component::Port:
            build_child_path<agent_framework::model::Port>(path, uuid, constants::Switch::PORTS);
            break;
        case Component::Switch:
            build_child_path<agent_framework::model::Switch>(path, uuid, constants::Fabric::SWITCHES);
            break;
        case Component::Acl:
            build_child_path<agent_framework::model::Acl>(path, uuid, constants::EthernetSwitch::ACLS);
            break;
        case Component::AclRule:
            build_child_path<agent_framework::model::AclRule>(path, uuid, constants::Acl::RULES);
            break;
        case Component::StaticMac:
            build_child_path<agent_framework::model::StaticMac>(path, uuid, constants::EthernetSwitchPort::STATIC_MACS);
            break;
        case Component::Fabric:
            build_parent_path<agent_framework::model::Fabric>(path, uuid, constants::Root::FABRICS);
            break;
        case Component::Endpoint:
            build_child_path<agent_framework::model::Endpoint>(path, uuid, constants::Fabric::ENDPOINTS);
            break;
        case Component::NetworkDevice:
            build_child_path<agent_framework::model::NetworkDevice>(path, uuid, constants::System::NETWORK_INTERFACES);
            break;
        case Component::NetworkDeviceFunction:
            build_child_path<agent_framework::model::NetworkDeviceFunction>(path, uuid, constants::NetworkInterface::NETWORK_DEVICE_FUNCTIONS);
            break;
        case Component::AuthorizationCertificate:
        case Component::Vlan:
        case Component::PSU:
        case Component::Fan:
        case Component::NeighborSwitch:
        case Component::PortMember:
        case Component::RemoteEthernetSwitch:
        case Component::PowerZone:
        case Component::ThermalZone:
        case Component::None:
        case Component::Root:
        default:
            THROW(agent_framework::exceptions::NotFound, "rest",
                  "Could not get URL for component type: " + std::string(type.to_string()));
            break;
    }
}


std::string get_component_url(enums::Component type, const std::string& uuid) {
    endpoint::PathBuilder pb(constants::Routes::ROOT_PATH);
    get_component_url_recursive(pb, type, uuid);
    return pb.build();
}


void set_location_header(server::Response& res, const std::string& path) {
    res.set_header(LOCATION, path);
}


double round(double number, int digits) {
    double precision = pow(10.0, digits);
    return std::round(number * precision) / precision;
}


namespace {
static constexpr const double gb_to_gib_factor = 1000.0 * 1000.0 * 1000.0 / (1024.0 * 1024.0 * 1024.0);
static constexpr const double mb_to_mib_factor = 1000.0 * 1000.0 / (1024.0 * 1024.0);
static constexpr const double mb_to_gib_factor = 1000.0 * 1000.0 / (1024.0 * 1024.0 * 1024.0);
static constexpr const double gb_to_b_factor = 1000.0 * 1000.0 * 1000.0;
}


double gb_to_gib(double number) {
    return number * gb_to_gib_factor;
}


double gib_to_gb(double number) {
    return number / gb_to_gib_factor;
}


double mb_to_mib(double number) {
    return number * mb_to_mib_factor;
}


double mb_to_gib(double number) {
    return number * mb_to_gib_factor;
}


std::uint32_t mb_to_mib(std::uint32_t number) {
    return std::uint32_t(number * mb_to_mib_factor);
}


std::uint64_t gb_to_b(double number) {
    return std::uint64_t(number * gb_to_b_factor);
}


void children_to_add_to_remove(agent_framework::module::managers::ManyToManyManager& manager,
                               const std::string& parent_uuid,
                               const std::vector<std::string>& requested_children,
                               std::vector<std::string>& children_to_add,
                               std::vector<std::string>& children_to_remove) {
    children_to_remove = manager.get_children(parent_uuid);

    for (auto it = requested_children.cbegin(); it != requested_children.end(); ++it) {
        auto iterator = std::remove(children_to_remove.begin(),
                                    children_to_remove.end(),
                                    *it);
        if (iterator != children_to_remove.end()) {
            children_to_remove.erase(iterator, children_to_remove.end());
        }
        else {
            children_to_add.emplace_back(*it);
        }
    }
}

}
}
}
}
