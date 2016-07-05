/*!
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
 * */

#include "psme/rest-ref/server/parameters.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/endpoints/utils.hpp"
#include "psme/rest-ref/utils/mapper.hpp"
#include "psme/rest-ref/model/finder.hpp"

#include <regex>

using namespace app::rest::constants::Common;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;

namespace {
std::regex make_regex(const std::string& str) {
    return std::regex("\\{" + str + "\\}");
}
}

namespace psme {
namespace rest {
namespace endpoint {
namespace utils {


std::string get_port_uuid_from_url(const std::string& port_url){
    try {
        auto params = psme::rest::model::Mapper::get_params(port_url);
        auto port_uuid =
            psme::rest::model::Find<agent_framework::model::SwitchPort>(
            params[app::rest::constants::PathParam::SWITCH_PORT_ID])
            .via<agent_framework::model::Switch>(
            params[app::rest::constants::PathParam::ETHERNET_SWITCH_ID])
            .get().get_uuid();
        return port_uuid;
    } catch (agent_framework::exceptions::NotFound& e) {
        log_error(GET_LOGGER("rest"), "Could not find port: " + port_url);
        throw(agent_framework::exceptions::InvalidParameters(e.what()));
    }
}

std::string make_endpoint_context(const psme::rest::server::Parameters& params,
                                  std::string link) {
    for (const auto& param : params) {
        link = std::regex_replace(link, ::make_regex(param.first), param.second);
    }
    return link;
}

void status_to_json(const Resource& res, json::Value& v) {
    auto& status = res.get_status();
    v[STATUS][STATE] = status.get_state().to_string();
    v[STATUS][HEALTH] = status.get_health().to_string();
}

std::uint64_t id_to_uint64(const std::string& id_as_string) {
    if (!std::all_of(id_as_string.begin(), id_as_string.end(), ::isdigit)) {
        // A string containing non-digit characters is NOT supposed to reach this method,
        // because the rest server only allows id strings of the form [0-9]+. So, we log an error.
        log_error(GET_LOGGER("rest"), "String " + id_as_string + ", which contains not-digit characters"
            "was received for conversion to rest id!");
        THROW(::agent_framework::exceptions::NotFound, "rest",
              id_as_string + " could not be converted to rest id, because it's not a number string.");
    }

    std::uint64_t id;
    try {
        id = std::stoull(id_as_string, NULL);
    }
    catch (const std::out_of_range&) {
        THROW(::agent_framework::exceptions::NotFound, "rest",
              id_as_string + " could not be converted to rest id, because it's too big.");
    }
    catch (const std::invalid_argument&) {
        // This is NOT supposed to happen, because the input check at the beginning of this method.
        log_error(GET_LOGGER("rest"), "Argument different than a string of digits received to convert to rest id!");
        THROW(::agent_framework::exceptions::NotFound, "rest",
              id_as_string + " could not be converted to rest id, because it's not a proper id.");
    }
    return id;
}

namespace {
bool try_find_manager_by_id(GenericManager<Manager>& container, std::uint64_t id, Manager& manager) {
    try {
        const std::string& uuid =  container.rest_id_to_uuid(id);
        manager = container.get_entry(uuid);
        return true;
    } catch (agent_framework::exceptions::InvalidParameters&) {
        return false;
    }
}
}

Manager get_manager(const std::string& id_as_string) {
    std::uint64_t id = id_to_uint64(id_as_string);

    Manager manager{};
    if(try_find_manager_by_id(
        agent_framework::module::ComputeManager::get_instance()->
            get_module_manager(), id, manager)) {
        return manager;
    } else if(try_find_manager_by_id(
        agent_framework::module::NetworkManager::get_instance()->
            get_module_manager(), id, manager)) {
        return manager;
    } else if(try_find_manager_by_id(
        agent_framework::module::ChassisManager::get_instance()->
            get_module_manager(), id, manager)) {
        return manager;
    } else if(try_find_manager_by_id(
        agent_framework::module::StorageManager::get_instance()->
            get_module_manager(), id, manager)) {
        return manager;
    }

    else {
        THROW(agent_framework::exceptions::InvalidParameters, "rpc",
              "Manager entry not found (id= " + id_as_string + ").");
    }
}

namespace {
bool try_find_manager_by_uuid(GenericManager<Manager>& container, const std::string uuid, Manager& manager) {
    try {
        manager = container.get_entry(uuid);
        return true;
    } catch (agent_framework::exceptions::InvalidParameters&) {
        return false;
    }
}
}

Manager get_manager_by_uuid(const std::string& uuid){
    Manager manager{};
    if(try_find_manager_by_uuid(
        agent_framework::module::ComputeManager::get_instance()->
            get_module_manager(), uuid, manager)) {
        return manager;
    } else if(try_find_manager_by_uuid(
        agent_framework::module::NetworkManager::get_instance()->
            get_module_manager(), uuid, manager)) {
        return manager;
    } else if(try_find_manager_by_uuid(
        agent_framework::module::ChassisManager::get_instance()->
            get_module_manager(), uuid, manager)) {
        return manager;
    } else if(try_find_manager_by_uuid(
        agent_framework::module::StorageManager::get_instance()->
            get_module_manager(), uuid, manager)) {
        return manager;
    } else {
        THROW(agent_framework::exceptions::InvalidParameters, "rpc",
              "Manager entry not found,  invalid uuid= " + uuid + ".");
    }
}

namespace {
bool try_find_chassis_by_id(GenericManager<Chassis>& container, std::uint64_t id, Chassis& chassis) {
    try {
        const std::string& uuid =  container.rest_id_to_uuid(id);
        chassis = container.get_entry(uuid);
        return true;
    } catch (agent_framework::exceptions::InvalidParameters&) {
        return false;
    }
}
}

Chassis get_chassis(const std::string& id_as_string){
    std::uint64_t id = id_to_uint64(id_as_string);

    Chassis chassis{};
    if(try_find_chassis_by_id(
        agent_framework::module::ComputeManager::get_instance()->
            get_chassis_manager(), id, chassis)) {
        return chassis;
    } else if(try_find_chassis_by_id(
        agent_framework::module::NetworkManager::get_instance()->
            get_chassis_manager(), id, chassis)) {
        return chassis;
    } else if(try_find_chassis_by_id(
        agent_framework::module::ChassisManager::get_instance()->
            get_chassis_manager(), id, chassis)) {
        return chassis;
    }
    // TO DO : add search in StorageManager (when it's been refactored)
    else {
        THROW(agent_framework::exceptions::InvalidParameters, "rpc",
              "Chassis entry not found (id= " + id_as_string + ").");
    }
}

namespace {
bool try_find_chassis_by_uuid(GenericManager<Chassis>& container, const std::string& uuid, Chassis& chassis) {
    try {
        chassis = container.get_entry(uuid);
        return true;
    } catch (agent_framework::exceptions::InvalidParameters&) {
        return false;
    }
}
}

Chassis get_chassis_by_uuid(const std::string& uuid){
    Chassis chassis{};
    if(try_find_chassis_by_uuid(
        agent_framework::module::ComputeManager::get_instance()->
            get_chassis_manager(), uuid, chassis)) {
        return chassis;
    } else if(try_find_chassis_by_uuid(
        agent_framework::module::NetworkManager::get_instance()->
            get_chassis_manager(), uuid, chassis)) {
        return chassis;
    } else if(try_find_chassis_by_uuid(
        agent_framework::module::ChassisManager::get_instance()->
            get_chassis_manager(), uuid, chassis)) {
        return chassis;
    }
    // TO DO : add search in StorageManager (when it's been refactored)
    else {
        THROW(agent_framework::exceptions::InvalidParameters, "rpc",
              "Chassis entry not found,  invalid uuid= " + uuid + ".");
    }
}


std::vector<std::uint64_t> get_chassis_ids_by_parent(const std::string& manager_uuid) {
    auto chassis_ids = agent_framework::module::ComputeManager::get_instance() -> get_chassis_manager().get_ids(manager_uuid);
    auto network_chassis_ids = agent_framework::module::NetworkManager::get_instance() -> get_chassis_manager().get_ids(manager_uuid);
    chassis_ids.insert(chassis_ids.end(), network_chassis_ids.begin(), network_chassis_ids.end());
    auto chassis_chassis_ids = agent_framework::module::ChassisManager::get_instance() -> get_chassis_manager().get_ids(manager_uuid);
    chassis_ids.insert(chassis_ids.end(), chassis_chassis_ids.begin(), chassis_chassis_ids.end());
    return chassis_ids;
}

namespace {
bool exists_drawer_with_parent(GenericManager<Chassis>& container, const std::string& manager_uuid) {
    // lambda used for filtering chassis entries in container
    auto is_drawer = [](const Chassis& chassis) -> bool{return chassis.get_type() == enums::ChassisType::Drawer;};
    auto keys = container.get_keys(manager_uuid, is_drawer);
    return !keys.empty();
}
}

bool is_drawer_manager(const std::string& manager_uuid) {
    return
        exists_drawer_with_parent(
            agent_framework::module::ComputeManager::get_instance()->
                get_chassis_manager(), manager_uuid) |
        exists_drawer_with_parent(
            agent_framework::module::ChassisManager::get_instance()->
                get_chassis_manager(), manager_uuid) |
        exists_drawer_with_parent(
            agent_framework::module::NetworkManager::get_instance()->
                get_chassis_manager(), manager_uuid);
        // add search in StorageManager (when it's ready)
}

std::string get_component_url_recursive(enums::Component type, const
    std::string& uuid) {
    using Component = enums::Component;
    using namespace app::rest;
    using namespace agent_framework;

    std::string url = "";

    switch(type) {
        case Component::Chassis:
            url += std::string(constants::Root::CHASSIS) + "/" +
                std::to_string(module::get_manager<Chassis>().
                uuid_to_rest_id(uuid));
            break;
        case Component::Manager:
            url += std::string(constants::Root::MANAGERS) + "/" +
                std::to_string(module::get_manager<Manager>().
                uuid_to_rest_id(uuid));
            break;
        case Component::System:
            url += std::string(constants::Root::SYSTEMS) + "/" +
                std::to_string(module::get_manager<System>().
                uuid_to_rest_id(uuid));
            break;
        case Component::StorageServices:
            url += std::string(constants::Root::SERVICES) + "/" +
                std::to_string(module::get_manager<StorageServices>().
                uuid_to_rest_id(uuid));
            break;
        case Component::Switch:
            url += std::string(constants::Root::ETHERNET_SWITCHES) + "/" +
                std::to_string(module::get_manager<Switch>().
                uuid_to_rest_id(uuid));
            break;
        case Component::Drive: {
            const auto& drive = module::get_manager<StorageController>().get_entry(uuid);
            url += get_component_url_recursive(drive.get_parent_type(),
                drive.get_parent_uuid()) + "/" +
                std::string(constants::StorageAdapter::DEVICES) + "/" +
                std::to_string(drive.get_id());
            break;
        }
        case Component::PhysicalDrive: {
            const auto& drive = module::get_manager<PhysicalDrive>().get_entry(uuid);
            url += get_component_url_recursive(drive.get_parent_type(),
                drive.get_parent_uuid()) + "/" +
                std::string(constants::StorageService::DRIVES) + "/" +
                std::to_string(drive.get_id());
            break;
        }
        case Component::LogicalDrive: {
            const auto& drive = module::get_manager<LogicalDrive>().get_entry(uuid);
            url += get_component_url_recursive(drive.get_parent_type(),
                drive.get_parent_uuid()) + "/" +
                std::string(constants::StorageService::LOGICAL_DRIVES) + "/" +
                std::to_string(drive.get_id());
            break;
        }
        case Component::IscsiTarget: {
            const auto& target = module::get_manager<IscsiTarget>().get_entry(uuid);
            url += get_component_url_recursive(target.get_parent_type(),
                target.get_parent_uuid()) + "/" +
                std::string(constants::StorageService::TARGETS) + "/" +
                std::to_string(target.get_id());
            break;
        }
        case Component::Dimm: {
            const auto& memory = module::get_manager<Dimm>().get_entry(uuid);
            url += get_component_url_recursive(memory.get_parent_type(),
                memory.get_parent_uuid()) + "/" +
                std::string(constants::System::DIMM_CONFIG) + "/" +
                std::to_string(memory.get_id());
            break;
        }
        case Component::MemoryChunk: {
            const auto& chunk = module::get_manager<MemoryChunk>().get_entry(uuid);
            url += get_component_url_recursive(chunk.get_parent_type(),
                chunk.get_parent_uuid()) + "/" +
                std::string(constants::System::MEMORY_CHUNKS) + "/" +
                std::to_string(chunk.get_id());
            break;
        }
        case Component::Processor: {
            const auto& processor = module::get_manager<Processor>().get_entry(uuid);
            url += get_component_url_recursive(processor.get_parent_type(),
                processor.get_parent_uuid()) + "/" +
                std::string(constants::System::PROCESSORS) + "/" +
                std::to_string(processor.get_id());
            break;
        }
        case Component::NetworkInterface: {
            const auto& iface = module::get_manager<NetworkInterface>().get_entry(uuid);
            url += get_component_url_recursive(iface.get_parent_type(),
                iface.get_parent_uuid()) + "/" +
                std::string(constants::System::ETHERNET_INTERFACES) + "/" +
                std::to_string(iface.get_id());
            break;
        }
        case Component::StorageController: {
            const auto& adapter = module::get_manager<StorageController>().get_entry(uuid);
            url += get_component_url_recursive(adapter.get_parent_type(),
                adapter.get_parent_uuid()) + "/" +
                std::string(constants::System::STORAGE_ADAPTERS) + "/" +
                std::to_string(adapter.get_id());
            break;
        }
        case Component::SwitchPort: {
            const auto& port = module::get_manager<SwitchPort>().get_entry(uuid);
            url += get_component_url_recursive(port.get_parent_type(),
                port.get_parent_uuid()) + "/" +
                std::string(constants::Switch::PORTS) + "/" +
                std::to_string(port.get_id());
            break;
        }
        case Component::PortVlan: {
            const auto& vlan = module::get_manager<PortVlan>().get_entry(uuid);
            url += get_component_url_recursive(vlan.get_parent_type(),
                vlan.get_parent_uuid()) + "/" +
                std::string(constants::SwitchPort::VLANS) + "/" +
                std::to_string(vlan.get_id());
            break;
        }
        case Component::Acl: {
            const auto& acl = module::get_manager<Acl>().get_entry(uuid);
            url += get_component_url_recursive(acl.get_parent_type(),
                acl.get_parent_uuid()) + "/" +
                std::string(constants::Switch::ACLS) + "/" +
                std::to_string(acl.get_id());
            break;
        }
        case Component::AclRule: {
            const auto& rule = module::get_manager<AclRule>().get_entry(uuid);
            url += get_component_url_recursive(rule.get_parent_type(),
                rule.get_parent_uuid()) + "/" +
                std::string(constants::Acl::RULES) + "/" +
                std::to_string(rule.get_id());
            break;
        }
        case Component::StaticMac: {
            const auto& static_mac = module::get_manager<StaticMac>().get_entry(uuid);
            url += get_component_url_recursive(static_mac.get_parent_type(),
                static_mac.get_parent_uuid()) + "/" +
                std::string(constants::SwitchPort::STATIC_MACS) + "/" +
                std::to_string(static_mac.get_id());
            break;
        }
        case Component::AuthorizationCertificate:
        case Component::Vlan:
        case Component::PSU:
        case Component::Fan:
        case Component::NeighborSwitch:
        case Component::PortMember:
        case Component::RemoteSwitch:
        case Component::PowerZone:
        case Component::ThermalZone:
        case Component::None:
        case Component::Root:
        default:
            THROW(::agent_framework::exceptions::InvalidParameters, "rest",
                "Could not get url for component type: " + std::string(type.to_string()));
            break;
    }
    return url;
}

std::string get_component_url(enums::Component type, const std::string& uuid) {
    const std::string base_url = "/redfish/v1/";
    return base_url + get_component_url_recursive(type, uuid);
}

void set_location_header(server::Response& res, const server::Request&,
        const std::string& path) {
    res.set_header(LOCATION, path);
}

Health general_health_rollup(const std::vector<Health>& healths){
    Health rollup{Health::OK};
    for(const auto& health : healths){
        // select worst health of all healths
        // Critical < Warning < OK
        if (health == Health::Critical ||
            (rollup == Health::OK && health == Health::Warning)) {
            rollup = health;
        }
    }
    return rollup;
}

namespace {
    /*!
     * @brief recursively fill a vector with the health of a logical drive's
     * children and their children
     *
     * @param healths the container for Healths
     * @param uuid of the root Logical Drive
     */
void get_drive_children_health(std::vector<Health>& healths, const std::string& uuid) {
    // get many-to-many-manager for physical drives
    auto& physical_members_manager =
        agent_framework::module::StorageManager::get_instance()
            ->get_physical_drive_members_manager();

    // get healths of physical drive children of current drive
    if (physical_members_manager.parent_exists(uuid)) {
        auto phys_members = physical_members_manager.get_children(uuid);
        for (const auto& phys_member : phys_members) {
            try {
                healths.emplace_back(
                    agent_framework::module::get_manager<PhysicalDrive>()
                        .get_entry(phys_member).get_status().get_health());
            }
            catch (agent_framework::exceptions::InvalidParameters&) {
                log_error(GET_LOGGER("rest"), "PhysicalDrive "
                    << phys_member << " is present in the PhysicalDrivesMembers"
                        "table but it does not exist as a resource");
            }
        }
    }

    // get many-to-many-manager for logical drives
    auto& logical_members_manager =
        agent_framework::module::StorageManager::get_instance()
            ->get_logical_drive_members_manager();

    // get healths of logical drive children and their children of current drive
    if (logical_members_manager.parent_exists(uuid)) {
        auto log_members = logical_members_manager.get_children(uuid);
        for (const auto& log_member : log_members) {
            try {
                healths.emplace_back(
                    agent_framework::module::get_manager<LogicalDrive>()
                        .get_entry(log_member).get_status().get_health());
                get_drive_children_health(healths, log_member);
            }
            catch (agent_framework::exceptions::InvalidParameters&) {
                log_error(GET_LOGGER("rest"), "PhysicalDrive "
                    << log_member << " is present in the PhysicalDrivesMembers"
                        "table but it does not exist as a resource");
            }
        }
    }

}
}

Health rollup_logical_drive_health(const std::string& uuid) {
    std::vector<Health> healths;
    // get healths of children drives
    get_drive_children_health(healths, uuid);
    // get this drive's health
    healths.emplace_back(
        agent_framework::module::get_manager<LogicalDrive>()
                .get_entry(uuid).get_status().get_health());
    return general_health_rollup(healths);
}

Health rollup_remote_target_health(const std::string& uuid) {
    std::vector<Health> healths;
    auto target = agent_framework::module::get_manager<IscsiTarget>()
        .get_entry(uuid);
    // get this target's health
    healths.emplace_back(target.get_status().get_health());
    //get this target's logical drives' health
    for(const auto& target_lun: target.get_target_lun()){
        try {
            healths.emplace_back(
                rollup_logical_drive_health(
                    target_lun.get_logical_drive()));
        }
        catch (agent_framework::exceptions::InvalidParameters&) {
            log_error(GET_LOGGER("rest"), "Logical Drive "
                << target_lun.get_logical_drive() << ", used by "
                << " an iSCSITarget (" << target.get_uuid() << "), does not exist");
        }
    }
    return general_health_rollup(healths);
}

Health rollup_storage_service_health(const std::string& uuid) {
    std::vector<Health> healths;
    auto storage = agent_framework::module::StorageManager::get_instance()
        ->get_storage_services_manager().get_entry(uuid);
    // get the health of this storage service and it's remote targets
    healths.emplace_back(
        rollup_health<StorageServices, agent_framework::model::IscsiTarget>(storage));
    healths.emplace_back(
        // logical drives
        rollup_children_health<StorageServices, agent_framework::model::LogicalDrive>(storage));
    healths.emplace_back(
        // physical drives
        rollup_children_health<StorageServices, agent_framework::model::PhysicalDrive>(storage));
    return general_health_rollup(healths);
}

Health rollup_system_health(const std::string& uuid) {
    auto system = agent_framework::module::ComputeManager::get_instance()
        ->get_system_manager().get_entry(uuid);
    // rollup the health of this system, it's StorageAdapters and their Devices
    auto healths = std::vector<Health>{rollup_health<System, StorageController, agent_framework::model::Drive> (system)};
    healths.emplace_back(
        // processors
        rollup_children_health<System, agent_framework::model::Processor>(system));
    healths.emplace_back(
        // dimms
        rollup_children_health<System, agent_framework::model::Dimm> (system));
    healths.emplace_back(
        // network interfaces
        rollup_children_health<System, agent_framework::model::NetworkInterface> (system));

    return general_health_rollup(healths);
}

namespace {
void get_port_children_health(std::vector<Health>& healths, const std::string& uuid) {
    auto port = agent_framework::module::get_manager<SwitchPort>()
        .get_entry(uuid);
    // get this port's vlans' health
    healths.emplace_back(
        rollup_children_health<SwitchPort, PortVlan>(port));

    // get this port's member ports' health
    const auto& port_members_member =
        agent_framework::module::NetworkManager::get_instance()->get_port_members_manager();
    if (port_members_member.parent_exists(uuid)) {
        const auto& port_manager = agent_framework::module::NetworkManager::get_instance()->get_port_manager();
        auto children = port_members_member.get_children(uuid);

        for (const auto& child : children) {
            try {
                healths.emplace_back(
                    port_manager.get_entry(child).get_status().get_health());
                get_port_children_health(healths, child);
            }
            catch (const agent_framework::exceptions::InvalidParameters&) {
                log_error(GET_LOGGER("rest"), "Port " << child <<
                    " is present in the portMembers field of port " << uuid <<", but it does not exist as a resource");
            }
        }
    }
}
}

Health rollup_switch_port_health(const std::string& uuid) {
    std::vector<Health> healths;
    // get this port's children's health
    get_port_children_health(healths, uuid);
    // get this port's health
    healths.emplace_back(
        agent_framework::module::get_manager<SwitchPort>()
            .get_entry(uuid).get_status().get_health());
    return general_health_rollup(healths);
}

Health rollup_switch_health(const std::string& uuid) {
    auto swtch = agent_framework::module::NetworkManager::get_instance()
        ->get_switch_manager().get_entry(uuid);
    // rollup the health of this switch, it's Ports and their PortVlans
    return rollup_health<Switch, SwitchPort, PortVlan> (swtch);
}

Health rollup_manager_health(const std::string& uuid) {
    auto manager = agent_framework::module::get_manager<Manager>().get_entry(uuid);

    auto get_switch_health = [](const Switch& sw){return rollup_switch_health(sw.get_uuid());};
    // get healths of this manager's switches
    auto healths = query_entries<Switch, Health>(uuid, get_switch_health);

    auto get_system_health = [](const System& s){return rollup_system_health(s.get_uuid());};
    // get rolled-up health of this manager's systems
    healths.emplace_back(general_health_rollup(
        query_entries<System, Health>(uuid, get_system_health)));

    auto get_storage_health = [](const StorageServices& ss){return rollup_storage_service_health(ss.get_uuid());};
    // get rolled-up health of this manager's storage services
    healths.emplace_back(general_health_rollup(
        query_entries<StorageServices, Health>(uuid, get_storage_health)));

    // get rolled-up health of this manager and all it's chassis
    healths.emplace_back(rollup_health<Manager,Chassis>(manager));

    // manager's Network Interface's health is not taken into the roll-up

    return general_health_rollup(healths);
}

namespace {
    static constexpr const double gb_to_gib_factor = 1000.0 * 1000.0 * 1000.0 / (1024.0 * 1024.0 * 1024.0);
    static constexpr const double mb_to_mib_factor = 1000.0 * 1000.0 / (1024.0 * 1024.0);
    static constexpr const double mb_to_gib_factor = 1000.0 * 1000.0 / (1024.0 * 1024.0 * 1024.0);
}

double gb_to_gib(double number){
    return number * gb_to_gib_factor;
}

double gib_to_gb(double number){
    return number / gb_to_gib_factor;
}

double mb_to_mib(double number){
    return number * mb_to_mib_factor;
}

double mb_to_gib(double number){
    return number * mb_to_gib_factor;
}

uint32_t mb_to_mib(uint32_t number){
    return uint32_t(number * mb_to_mib_factor);
}

}
}
}
}
