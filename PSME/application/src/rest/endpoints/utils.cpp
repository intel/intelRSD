/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
#include "psme/rest/server/multiplexer.hpp"
#include "psme/rest/model/find.hpp"
#include "psme/rest/server/utils.hpp"

#include "agent-framework/module/model/model_chassis.hpp"
#include "agent-framework/module/model/model_compute.hpp"
#include "agent-framework/module/model/model_network.hpp"
#include "agent-framework/module/model/model_storage.hpp"
#include "agent-framework/module/model/model_pnc.hpp"

#include <regex>
#include <cmath>
#include <iterator>



using namespace psme::rest::constants::Common;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;

namespace {
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
        auto params = psme::rest::server::Multiplexer::get_instance()->
            get_params(port_url, constants::Routes::ETHERNET_SWITCH_PORT_PATH);
        return psme::rest::model::find<agent_framework::model::EthernetSwitch, agent_framework::model::EthernetSwitchPort>(
            params).get_uuid();
    }
    catch (agent_framework::exceptions::NotFound&) {
        THROW(agent_framework::exceptions::InvalidValue, "rest", "Could not find port with URI: '" + port_url + "'.");
    }
}


std::uint64_t id_to_uint64(const std::string& id_as_string) {
    if (!std::all_of(id_as_string.begin(), id_as_string.end(), ::isdigit)) {
        // A string containing non-digit characters is NOT supposed to reach this method,
        // because the rest server only allows ID strings of the form [0-9]+. So, we log an error.
        log_error("rest", "String " + id_as_string +
                          " which contains not-digit characters was received for conversion to REST ID!");

        throw agent_framework::exceptions::NotFound(
            "ID '" + id_as_string + "' could not be converted to REST ID, because it's not a number.");
    }

    std::uint64_t id{};
    try {
        id = std::stoull(id_as_string, NULL);
    }
    catch (const std::out_of_range&) {
        throw agent_framework::exceptions::NotFound(
            "ID '" + id_as_string + "' could not be converted to REST ID, because it's too big.");
    }
    catch (const std::invalid_argument&) {
        // Occures when ID is empty string
        throw agent_framework::exceptions::NotFound(
            "ID '" + id_as_string + "' could not be converted to REST ID, because it's not a proper ID.");
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
    path.append(collection_literal).append(agent_framework::module::get_manager<M>().uuid_to_rest_id(uuid));
}


template<typename M>
void build_child_path(endpoint::PathBuilder& path, const std::string& uuid, const std::string& collection_literal) {
    const auto& resource = agent_framework::module::get_manager<M>().get_entry(uuid);
    get_component_url_recursive(path, resource.get_parent_type(), resource.get_parent_uuid());
    path.append(collection_literal).append(resource.get_id());
}


template<typename M>
void build_array_member_path(endpoint::PathBuilder& path, const std::string& uuid, const std::string& array_name) {
    const auto& resource = agent_framework::module::get_manager<M>().get_entry(uuid);
    get_component_url_recursive(path, resource.get_parent_type(), resource.get_parent_uuid());

    // find member's index in children array
    const auto array_members = agent_framework::module::get_manager<M>().get_keys(resource.get_parent_uuid());
    size_t index = std::distance(
        array_members.begin(),
        std::find(array_members.begin(), array_members.end(), uuid)
    );
    if (index >= array_members.size()) {
        log_error("rest", "Could not build url for resource " + uuid + ".");
        THROW(agent_framework::exceptions::NotFound, "rest",
              std::string("Component of type ") + M::get_component().to_string() +
              " was removed before it's URL could be created.");
    }
    path.append_jsonptr(constants::PathParam::PATH_SEP + array_name).append(index);
}


size_t find_metric_index(const agent_framework::model::Metric& metric) {
    // find metric's index among other metrics for the same component and with the same name
    const auto array_members = agent_framework::module::get_manager<agent_framework::model::Metric>().get_keys(
        [&](const agent_framework::model::Metric& m) -> bool {
            return m.get_component_type() == metric.get_component_type()
                   && m.get_component_uuid() == metric.get_component_uuid()
                   && m.get_name() == metric.get_name();
        }
    );
    size_t index = std::distance(
        array_members.begin(),
        std::find(array_members.begin(), array_members.end(), metric.get_uuid())
    );
    if (index >= array_members.size()) {
        log_error("rest", "Could not build url for resource " + metric.get_uuid() + ".");
        THROW(agent_framework::exceptions::NotFound, "rest",
              "Component of type Metric was removed before it's URL could be created.");
    }
    return index;
}


void build_metric_path(endpoint::PathBuilder& path, const std::string& uuid) {
    using namespace psme::rest;
    const auto metric = agent_framework::module::get_manager<agent_framework::model::Metric>().get_entry(uuid);
    try {
        get_component_url_recursive(path, metric.get_component_type(), metric.get_component_uuid());
    }
    catch (agent_framework::exceptions::InvalidUuid& e) {
        THROW(agent_framework::exceptions::NotFound, "rest", std::string("Failed to build metric path: ") + e.what());
    }

    // handle exceptions in URL building for Power and Thermal metrics
    if (metric.get_name() == constants::PowerZone::POWER_CONSUMED_WATTS_PATH) {
        path.append_jsonptr(std::string{constants::PathParam::PATH_SEP} + constants::PowerZone::POWER_CONTROL)
            .append(0) // if this metric exists, it's PowerZone is the only member in PowerControl array
            .append(constants::PowerZone::POWER_CONSUMED_WATTS);
    }
    else if (metric.get_name() == constants::PowerZone::INPUT_AC_POWER_WATTS_PATH) {
        path.append_jsonptr(constants::PowerZone::INPUT_AC_POWER_WATTS_PATH);
    }
    else if (metric.get_name() == constants::ChassisSensor::READING_VOLTS_PATH) {
        // find metrics's index in Voltages array
        auto index = find_metric_index(metric);
        path.append_jsonptr(std::string{constants::PathParam::PATH_SEP} + constants::PowerZone::VOLTAGES)
            .append(index)
            .append(constants::ChassisSensor::READING_VOLTS);
    }
    else if (metric.get_name() == constants::ThermalZone::VOLUMETRIC_AIRFLOW_CFM_PATH) {
        path.append_jsonptr(constants::ThermalZone::VOLUMETRIC_AIRFLOW_CFM_PATH);
    }
    else if (metric.get_name() == constants::ThermalZone::READING_PATH) {
        path.append(constants::ThermalZone::READING);
    }
    else if (metric.get_name() == constants::ChassisSensor::READING_CELSIUS_PATH) {
        // find metrics's index in Temperatures array
        auto index = find_metric_index(metric);
        path.append_jsonptr(std::string{constants::PathParam::PATH_SEP} + constants::ThermalZone::TEMPERATURES)
            .append(index)
            .append(constants::ChassisSensor::READING_CELSIUS);
    }
    else if (metric.get_component_type() == Component::Processor) {
        path.append(constants::PathParam::OEM_INTEL_RACKSCALE).append(constants::Common::METRICS).append_jsonptr(metric.get_name());
    }
    else {
        path.append(constants::Common::METRICS).append_jsonptr(metric.get_name());
    }
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
            path.append(constants::TaskService::TASK_SERVICE).append(constants::TaskService::TASKS)
                .append(module::get_manager<agent_framework::model::Task>().uuid_to_rest_id(uuid));
            break;
        case Component::System:
            build_parent_path<agent_framework::model::System>(path, uuid, constants::Root::SYSTEMS);
            break;
        case Component::StorageSubsystem:
            build_child_path<agent_framework::model::StorageSubsystem>(path, uuid, constants::System::STORAGE);
            break;
        case Component::StorageService:
            build_parent_path<agent_framework::model::StorageService>(path, uuid, constants::Root::STORAGE_SERVICES);
            break;
        case Component::EthernetSwitch:
            build_parent_path<agent_framework::model::EthernetSwitch>(path, uuid, constants::Root::ETHERNET_SWITCHES);
            break;
        case Component::Drive:
            build_child_path<agent_framework::model::Drive>(path, uuid, constants::Chassis::DRIVES);
            break;
        case Component::Memory:
            build_child_path<agent_framework::model::Memory>(path, uuid, constants::System::MEMORY);
            break;
        case Component::Processor:
            build_child_path<agent_framework::model::Processor>(path, uuid, constants::System::PROCESSORS);
            break;
        case Component::NetworkInterface: {
            const auto& nic = module::get_manager<agent_framework::model::NetworkInterface>().get_entry(uuid);
            if (nic.get_parent_type() == Component::Manager) {
                build_child_path<agent_framework::model::NetworkInterface>(path, uuid,
                                                                           constants::Manager::ETHERNET_INTERFACES);
            }
            else if (nic.get_parent_type() == Component::System) {
                build_child_path<agent_framework::model::NetworkInterface>(path, uuid,
                                                                           constants::System::ETHERNET_INTERFACES);
            }
            else {
                THROW(agent_framework::exceptions::NotFound, "rest", "Could not get URL for Network Interface!");
            }
            break;
        }
        case Component::StorageController:
            build_array_member_path<agent_framework::model::StorageController>(path, uuid,
                                                                               constants::StorageSubsystem::STORAGE_CONTROLLERS);
            break;
        case Component::EthernetSwitchPort:
            build_child_path<agent_framework::model::EthernetSwitchPort>(path, uuid, constants::EthernetSwitch::PORTS);
            break;
        case Component::EthernetSwitchPortVlan:
            build_child_path<agent_framework::model::EthernetSwitchPortVlan>(path, uuid,
                                                                             constants::EthernetSwitchPort::VLANS);
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
                log_error("rest",
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
        case Component::PowerZone: {
            const auto& power_zone = module::get_manager<agent_framework::model::PowerZone>().get_entry(uuid);
            get_component_url_recursive(path, Component::Chassis, power_zone.get_parent_uuid());
            path.append(constants::Chassis::POWER);
            break;
        }
        case Component::ThermalZone: {
            const auto& thermal_zone = module::get_manager<agent_framework::model::ThermalZone>().get_entry(uuid);
            get_component_url_recursive(path, Component::Chassis, thermal_zone.get_parent_uuid());
            path.append(constants::Chassis::THERMAL);
            break;
        }
        case Component::NetworkDevice:
            build_child_path<agent_framework::model::NetworkDevice>(path, uuid, constants::System::NETWORK_INTERFACES);
            break;
        case Component::NetworkDeviceFunction:
            build_child_path<agent_framework::model::NetworkDeviceFunction>(path, uuid,
                                                                            constants::NetworkInterface::NETWORK_DEVICE_FUNCTIONS);
            break;
        case Component::PSU: {
            /* PSU is an array item in Power property */
            build_array_member_path<agent_framework::model::Psu>(path, uuid, constants::PowerZone::POWER_SUPPLIES);
            break;
        }
        case Component::Fan: {
            /* Fan is an array item in Thermal property */
            build_array_member_path<agent_framework::model::Fan>(path, uuid, constants::ThermalZone::FANS);
            break;
        }
        case Component::MetricDefinition:
            path.append(constants::Root::TELEMETRY_SERVICE).append(constants::TelemetryService::METRIC_DEFINITIONS)
                .append(module::get_manager<agent_framework::model::MetricDefinition>().uuid_to_rest_id(uuid));
            break;
        case Component::Metric: {
            build_metric_path(path, uuid);
            break;
        }
        case Component::TrustedModule:
            build_array_member_path<agent_framework::model::TrustedModule>(path, uuid,
                                                                           constants::System::TRUSTED_MODULES);
            break;
        case Component::Volume:
            build_child_path<agent_framework::model::Volume>(path, uuid, constants::StorageService::VOLUMES);
            break;
        case Component::StoragePool:
            build_child_path<agent_framework::model::StoragePool>(path, uuid, constants::StorageService::STORAGE_POOLS);
            break;
        case Component::AuthorizationCertificate:
        case Component::Vlan:
        case Component::NeighborSwitch:
        case Component::PortMember:
        case Component::RemoteEthernetSwitch:
        case Component::ChassisSensor:
        case Component::MemoryDomain:
        case Component::MemoryChunks:
        case Component::None:
        case Component::Root:
        default:
            THROW(agent_framework::exceptions::NotFound, "rest",
                  "Could not get URL for component type: " + std::string(type.to_string()));
            break;
    }
}


std::string get_component_url(enums::Component type, const std::string& uuid) {
    endpoint::PathBuilder path_builder(constants::Routes::ROOT_PATH);
    get_component_url_recursive(path_builder, type, uuid);
    return path_builder.build();
}


void set_location_header(const server::Request& request, server::Response& response, const std::string& path) {
    const bool& scheme_secure = request.is_secure();
    const std::string& absolute_location_path = psme::rest::server::build_location_header(request, scheme_secure, path);
    response.set_header(LOCATION, absolute_location_path);
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


std::int64_t gb_to_b(double number) {
    // Casting to int will round the value to max of int if value exceeds max int
    return std::int64_t(number * gb_to_b_factor);
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


void string_array_to_json(json::Json& json, const agent_framework::model::attribute::Array<std::string>& array) {
    if (!json.is_array()) {
        json = json::Json::value_t::array;
    }
    for (const auto& str : array) {
        json.push_back(str);
    }
}


void populate_metrics(json::Json& component_json, const std::string& component_uuid) {
    auto metrics = agent_framework::module::get_manager<Metric>().get_entries([&component_uuid](const Metric& m) {
        return m.get_component_uuid() == component_uuid;
    });
    populate_metrics(component_json, metrics);
}


void populate_metrics(json::Json& component_json, const std::vector<agent_framework::model::Metric>& metrics) {
    for (const auto& metric: metrics) {
        try {
            auto ptr = json::Json::json_pointer(metric.get_name());
            component_json[ptr] = metric.get_value();
        }
        catch (const std::exception& e) {
            log_error("rest", "Populate metric " << metric.get_name() << " failed: " << e.what());
        }
    }
}

}
}
}
}
