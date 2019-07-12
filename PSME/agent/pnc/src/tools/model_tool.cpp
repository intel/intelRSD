/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file model_tool.cpp
 * @brief ModelTool implementation
 * */

#include "tools/model_tool.hpp"
#include "agent-framework/eventing/utils.hpp"

#include <set>



using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::module;
using namespace agent_framework::eventing;
using namespace agent::pnc::tools;

namespace {

template<typename RESOURCE>
std::string generic_get_uuid(const std::string& resource_name) {
    auto keys = get_manager<RESOURCE>().get_keys();
    if (0 == keys.size()) {
        throw std::runtime_error(std::string{"Unable to get "} + resource_name + std::string{": none found!"});
    }
    if (keys.size() > 1) {
        throw std::runtime_error(std::string{"Unable to get "} + resource_name + std::string{": too many entries!"});
    }
    return keys.front();
}

}


ModelTool::~ModelTool() {}


bool ModelTool::get_port_by_phys_id(Port& port, const std::uint32_t phy_port_id, bool is_temporary) const {
    auto key = get_manager<Port>().get_keys(
        [phy_port_id, is_temporary](const Port& p) -> bool {
            return (phy_port_id == p.get_phys_port_id()
                    && ((is_temporary && p.get_parent_uuid() == "") || (!is_temporary && p.get_parent_uuid() != "")));
        });

    if (1 == key.size()) {
        port = get_manager<Port>().get_entry(key.front());
        return true;
    }
    if (is_temporary) {
        log_error("model-tool", "PCIePort Twi access data for port " + std::to_string(phy_port_id)
            << " is not specified in configuration file.");
        return false;
    }
    else {
        log_error("model-tool", "PCIePort with physical port Id " + std::to_string(phy_port_id)
            << " not found");
        return false;
    }
}


bool ModelTool::get_zone_by_id(Zone& zone, const std::string& switch_uuid, const std::uint32_t zone_id) const {
    auto key = get_manager<Zone>().get_keys(
        [zone_id, switch_uuid](const Zone& z) -> bool {
            return (zone_id == z.get_zone_id() && switch_uuid == z.get_switch_uuid());
        });

    if (1 == key.size()) {
        zone = get_manager<Zone>().get_entry(key.front());
        return true;
    }
    else {
        log_error("model-tool", "Cannot find zone");
        log_debug("model-tool", "Cannot find zone with id " << zone_id << " for switch " << switch_uuid);
        return false;
    }
}


agent_framework::model::MetricDefinition ModelTool::get_health_metric_definition() const {
    auto& definition_manager = get_manager<MetricDefinition>();
    auto keys = definition_manager.get_keys([](const MetricDefinition& definition) {
        return definition.get_metric_jsonptr() == "/Health";
    });
    if (0 == keys.size()) {
        throw std::runtime_error(std::string{"Unable to get metric definition: none found!"});
    }
    if (keys.size() > 1) {
        throw std::runtime_error(std::string{"Unable to get metric definition: too many entries!"});
    }
    return definition_manager.get_entry(keys.front());
}


std::string ModelTool::get_manager_uuid() const {
    return generic_get_uuid<Manager>("manager");
}


std::string ModelTool::get_chassis_uuid() const {
    return generic_get_uuid<Chassis>("chassis");
}


std::string ModelTool::get_fabric_uuid() const {
    return generic_get_uuid<Fabric>("fabric");
}


std::string ModelTool::get_system_uuid() const {
    return generic_get_uuid<System>("system");
}


std::string ModelTool::get_storage_uuid() const {
    return generic_get_uuid<StorageSubsystem>("storage subsystem");
}


std::string ModelTool::get_switch_uuid() const {
    return generic_get_uuid<Switch>("switch");
}


void ModelTool::update_drive_status(const std::string& drive_uuid, const attribute::Status& status,
                                    const int media_life_left) const {

    auto drive = get_manager<Drive>().get_entry_reference(drive_uuid);
    drive->set_last_smart_health(status.get_health());
    if (drive->get_is_being_erased() || drive->get_is_in_critical_discovery_state()) {
        // we should not overwrite this states
        return;
    }

    bool changed = false;

    if (!drive->get_predicted_media_life_left().has_value()
        || media_life_left != drive->get_predicted_media_life_left()) {

        log_debug("agent", "Drive (" << drive_uuid << ") status changed: " << "Life left: "
                                     << media_life_left << "%");
        drive->set_predicted_media_life_left(media_life_left);
        drive->set_failure_predicted(media_life_left <= 0);
        changed = true;
    }

    attribute::Status prev_status = drive->get_status();
    if (!drive->get_is_in_warning_state() && (prev_status.get_health() != status.get_health()
                                              || prev_status.get_state() != status.get_state())) {

        log_debug("agent", "Drive (" << drive_uuid << ") status changed: "
                                     << status.get_health() << ", " << status.get_state());
        drive->set_status(status);
        changed = true;
    }

    if (changed) {
        send_event(drive_uuid, enums::Component::Drive, enums::Notification::Update, drive->get_parent_uuid());
    }
}


bool ModelTool::set_status_offline(Resource& resource) const {
    return set_status(resource, {enums::State::StandbyOffline, enums::Health::Critical});
}


bool ModelTool::set_status(Resource& resource, ::agent_framework::model::attribute::Status status) const {
    bool changed = false;
    attribute::Status prev_status = resource.get_status();
    if ((prev_status.get_health() != status.get_health() || prev_status.get_state() != status.get_state())) {

        log_debug("agent", "Resource (" << resource.get_id() << ") status has changed: "
                                        << status.get_health() << ", " << status.get_state());
        resource.set_status(status);
        changed = true;
    }
    return changed;
}


bool ModelTool::set_port_width_and_speed(Port& port,
                                         uint32_t width, uint32_t max_width,
                                         double speed, double max_speed) const {

    bool changed = false;
    if (!port.get_width().has_value() || width != port.get_width()) {
        log_debug("agent", "Port (" << port.get_port_id() << ") width has changed: " << width);
        port.set_width(width);
        changed = true;
    }
    if (!port.get_max_width().has_value() || max_width != port.get_max_width()) {
        log_debug("agent", "Port (" << port.get_port_id() << ") max width has changed: " << max_width);
        port.set_max_width(max_width);
        changed = true;
    }
    if (!port.get_speed_gbps().has_value() || speed != port.get_speed_gbps()) {
        log_debug("agent", "Port (" << port.get_port_id() << ") speed has changed: " << speed);
        port.set_speed_gbps(speed);
        changed = true;
    }
    if (!port.get_max_speed_gbps().has_value() || max_speed != port.get_max_speed_gbps()) {
        log_debug("agent", "Port (" << port.get_port_id() << ") max speed has changed: " << max_speed);
        port.set_max_speed_gbps(max_speed);
        changed = true;
    }
    return changed;
}


std::vector<std::string> ModelTool::get_functions_by_dsp_port_uuid(const std::string& port_uuid) const {
    auto is_function_on_dsp_port = [&port_uuid](const PcieFunction& f) -> bool {
        return f.get_dsp_port_uuid() == port_uuid;
    };

    return get_manager<PcieFunction>().get_keys(is_function_on_dsp_port);
}


std::vector<Uuid> ModelTool::get_endpoints_by_device_uuid(const Uuid& device_uuid) const {
    auto does_endpoint_use_device = [&device_uuid](const Endpoint& e) -> bool {
        for (const auto& entity : e.get_connected_entities()) {
            if (entity.get_entity().has_value() && entity.get_entity() == device_uuid) {
                return true;
            }
        }
        return false;
    };

    return get_manager<Endpoint>().get_keys(does_endpoint_use_device);
}


void ModelTool::degenerate_endpoint(const Uuid& endpoint_uuid, const std::vector<Uuid>& device_list) const {
    auto endpoint = get_manager<Endpoint>().get_entry_reference(endpoint_uuid);
    attribute::Status status = endpoint->get_status();
    status.set_state(enums::State::Disabled);
    status.set_health(enums::Health::OK);
    endpoint->set_status(std::move(status));

    auto entities = endpoint->get_connected_entities();
    for (auto& entity : entities) {
        if (0 != std::count(device_list.begin(), device_list.end(), entity.get_entity())) {
            entity.set_entity(OptionalField<std::string>{});
        }
    }
    endpoint->set_connected_entities(std::move(entities));
    send_event(endpoint_uuid, enums::Component::Endpoint, enums::Notification::Update, endpoint->get_parent_uuid());
}


void ModelTool::regenerate_endpoint(const std::string& endpoint_uuid, const std::string& drive_uuid) const {
    auto endpoint = get_manager<Endpoint>().get_entry_reference(endpoint_uuid);
    attribute::Status status = endpoint->get_status();
    status.set_state(enums::State::Enabled);
    status.set_health(enums::Health::OK);
    endpoint->set_status(std::move(status));

    auto entities = endpoint->get_connected_entities();
    for (auto& entity : entities) {
        if (!entity.get_entity().has_value() && entity.get_entity_role() == enums::EntityRole::Target) {
            entity.set_entity(drive_uuid);
        }
    }
    endpoint->set_connected_entities(entities);
    send_event(endpoint_uuid, enums::Component::Endpoint, enums::Notification::Update, endpoint->get_parent_uuid());
}


std::vector<std::string> ModelTool::get_ports_by_device_uuid(const Uuid& device_uuid) const {
    auto functions_drive = get_m2m_manager<Drive, PcieFunction>().get_children(device_uuid);
    auto functions_processor = get_m2m_manager<Processor, PcieFunction>().get_children(device_uuid);
    std::set<std::string> ports{};
    for (const auto& function_uuid : functions_drive) {
        ports.insert(get_manager<PcieFunction>().get_entry(function_uuid).get_dsp_port_uuid());
    }
    for (const auto& function_uuid : functions_processor) {
        ports.insert(get_manager<PcieFunction>().get_entry(function_uuid).get_dsp_port_uuid());
    }
    std::vector<std::string> res{};
    std::copy(ports.begin(), ports.end(), std::back_inserter(res));
    return res;
}


std::vector<Uuid> ModelTool::get_ports_uuids_by_endpoint_uuid(const Uuid& endpoint_uuid) const {

    std::vector<Uuid> endpoint_ports_uuids{};

    for (const auto& port_uuid : get_m2m_manager<Endpoint, Port>().get_children(endpoint_uuid)) {

        endpoint_ports_uuids.push_back(port_uuid);
    }

    return endpoint_ports_uuids;
}


std::string ModelTool::get_switch_for_device_uuid(const std::string& uuid) const {
    // currently we support only one switch
    auto ports = this->get_ports_by_device_uuid(uuid);
    if (ports.empty()) {
        log_error("pnc-gami", "Device does not have any pcie function connected to it");
        throw std::runtime_error("No pcie functions for a device");
    }
    return get_manager<Port>().get_entry(ports.front()).get_parent_uuid();
}


void ModelTool::set_drive_status(const std::string& drive_uuid,
                                 const agent_framework::model::attribute::Status& status) const {
    update_drive_status(drive_uuid, status,
                        get_manager<Drive>().get_entry(drive_uuid).get_predicted_media_life_left());
}


namespace agent {
namespace pnc {
namespace tools {

template<>
void ModelTool::set_device_status<agent_framework::model::Drive>(const Uuid& uuid,
                                                                 const agent_framework::model::attribute::Status& status) const {
    update_drive_status(uuid, status,
                        agent_framework::module::get_manager<agent_framework::model::Drive>().get_entry(
                            uuid).get_predicted_media_life_left());
}

}
}
}


std::vector<Uuid>
ModelTool::get_endpoints_with_role(const attribute::Array<Uuid>& endpoints_uuids, const EntityRole role) {
    std::vector<Uuid> filtered_endpoints_uuids;
    std::copy_if(endpoints_uuids.begin(), endpoints_uuids.end(), std::back_inserter(filtered_endpoints_uuids),
                 [role](Uuid endpoint_uuid) {
                     Endpoint endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
                     if (endpoint.get_connected_entities().size() > 0) {
                         return endpoint.get_connected_entities()[0].get_entity_role() == role;
                     }
                     return false;
                 });
    return filtered_endpoints_uuids;
}


void ModelTool::remove_undiscovered_devices_from_db() {

    std::vector<Uuid> uuids_to_remove_from_db;

    /* Remove undiscovered processors from database */
    auto processor_uuids = get_manager<Processor>().get_keys();
    auto system_uuid = get_dry_stabilized_system_uuid();
    auto db_processor_uuids = ModelDatabase<System>(system_uuid).get_multiple_values(
        db_keys::DEVICES_PROPERTY<Processor>);
    std::sort(std::begin(processor_uuids), std::end(processor_uuids));
    std::sort(std::begin(db_processor_uuids), std::end(db_processor_uuids));
    std::set_difference(std::begin(db_processor_uuids), std::end(db_processor_uuids), std::begin(processor_uuids),
                        std::end(processor_uuids), std::back_inserter(uuids_to_remove_from_db));
    std::for_each(std::begin(uuids_to_remove_from_db), std::end(uuids_to_remove_from_db),
                  [&system_uuid, this](const auto& uuid) {
                      this->remove_device_from_db<Processor, System>(uuid, system_uuid);
                  });
    log_debug("pnc-agent",
              "Removed " + std::to_string(uuids_to_remove_from_db.size()) + " undiscovered processors from database");

    /* Remove undiscovered drives from database */
    auto drive_uuids = get_manager<Drive>().get_keys();
    auto chassis_uuid = get_dry_stabilized_chassis_uuid();
    auto db_drive_uuids = ModelDatabase<Chassis>(chassis_uuid).get_multiple_values(db_keys::DEVICES_PROPERTY<Drive>);
    std::sort(std::begin(drive_uuids), std::end(drive_uuids));
    std::sort(std::begin(db_drive_uuids), std::end(db_drive_uuids));
    uuids_to_remove_from_db.clear();
    std::set_difference(std::begin(db_drive_uuids), std::end(db_drive_uuids), std::begin(drive_uuids),
                        std::end(drive_uuids), std::back_inserter(uuids_to_remove_from_db));
    std::for_each(std::begin(uuids_to_remove_from_db), std::end(uuids_to_remove_from_db),
                  [&chassis_uuid, this](const auto& uuid) {
                      this->remove_device_from_db<Drive, Chassis>(uuid, chassis_uuid);
                  });
    log_debug("pnc-agent",
              "Removed " + std::to_string(uuids_to_remove_from_db.size()) + " undiscovered drives from database");
}
