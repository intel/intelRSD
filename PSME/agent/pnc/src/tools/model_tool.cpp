/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 *
 * @brief ModelTool implementation
 * */

#include "tools/model_tool.hpp"

#include <set>

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::pnc::tools;
using namespace agent_framework::eventing;

namespace {

template <typename RESOURCE>
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
        log_error(GET_LOGGER("model-tool"), "PCIePort Twi access data for port " + std::to_string(phy_port_id)
            << " is not specified in configuration file.");
        return false;
    }
    else {
        log_error(GET_LOGGER("model-tool"), "PCIePort with physical port Id " + std::to_string(phy_port_id)
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
        log_error(GET_LOGGER("model-tool"), "Cannot find zone");
        log_debug(GET_LOGGER("model-tool"), "Cannot find zone with id " << zone_id << " for switch " << switch_uuid);
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

void ModelTool::send_event(const std::string& parent, const std::string& uuid,
        enums::Component component, Notification type) const {
    EventData edat{};
    edat.set_parent(parent);
    edat.set_component(uuid);
    edat.set_type(component);
    edat.set_notification(type);
    EventsQueue::get_instance()->push_back(edat);
    log_info(GET_LOGGER("agent"), "Sending an event for component "
        << edat.get_component() << "," << "(" << edat.get_type() << ")"
        << ", notification type: " << edat.get_notification());
}

void ModelTool::update_drive_status(const std::string& drive_uuid, const attribute::Status& status,
        const int media_life_left) const {

    auto drive = get_manager<Drive>().get_entry_reference(drive_uuid);
    if (drive->get_is_being_erased() || drive->get_is_in_critical_discovery_state()) {
        // we should not overwrite this states
        return;
    }

    bool changed = false;

    if (!drive->get_predicted_media_life_left().has_value()
        || media_life_left != drive->get_predicted_media_life_left()) {

        log_debug(GET_LOGGER("agent"), "Drive (" << drive_uuid << ") status changed: " << "Life left: "
            << media_life_left << "%");
        drive->set_predicted_media_life_left(media_life_left);
        drive->set_failure_predicted(media_life_left <= 0);
        changed = true;
    }

    attribute::Status prev_status = drive->get_status();
    if (!drive->get_is_in_warning_state() && (prev_status.get_health() != status.get_health()
        || prev_status.get_state() != status.get_state())) {

            log_debug(GET_LOGGER("agent"), "Drive (" << drive_uuid << ") status changed: "
                << status.get_health() << ", " << status.get_state());
            drive->set_status(status);
            changed = true;
    }

    if (changed) {
        this->send_event(drive->get_parent_uuid(), drive_uuid, enums::Component::Drive, Notification::Update);
    }
}

bool ModelTool::set_status_offline(Resource& resource) const {
    return set_status(resource, {enums::State::StandbyOffline, enums::Health::Critical});
}

bool ModelTool::set_status(Resource& resource, ::agent_framework::model::attribute::Status status) const {
    bool changed = false;
    attribute::Status prev_status = resource.get_status();
    if ((prev_status.get_health() != status.get_health() || prev_status.get_state() != status.get_state())) {

        log_debug(GET_LOGGER("agent"), "Resource (" << resource.get_id() << ") status has changed: "
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
        log_debug(GET_LOGGER("agent"), "Port (" << port.get_port_id() << ") width has changed: " << width);
        port.set_width(width);
        changed = true;
    }
    if (!port.get_max_width().has_value() || max_width != port.get_max_width()) {
        log_debug(GET_LOGGER("agent"), "Port (" << port.get_port_id() << ") max width has changed: " << max_width);
        port.set_max_width(max_width);
        changed = true;
    }
    if (!port.get_speed_gbps().has_value() || speed != port.get_speed_gbps()) {
        log_debug(GET_LOGGER("agent"), "Port (" << port.get_port_id() << ") speed has changed: " << speed);
        port.set_speed_gbps(speed);
        changed = true;
    }
    if (!port.get_max_speed_gbps().has_value() || max_speed != port.get_max_speed_gbps()) {
        log_debug(GET_LOGGER("agent"), "Port (" << port.get_port_id() << ") max speed has changed: " << max_speed);
        port.set_max_speed_gbps(max_speed);
        changed = true;
    }
    return changed;
}


std::vector<std::string> ModelTool::get_drives_by_dsp_port_uuid(const std::string& port_uuid) const {
    auto is_drive_on_dsp_port = [&port_uuid](const Drive& drive) -> bool {
        const auto& dsp_ports = drive.get_dsp_port_uuids();
        return (std::find(dsp_ports.begin(), dsp_ports.end(), port_uuid) != dsp_ports.end());
    };

    return get_manager<Drive>().get_keys(is_drive_on_dsp_port);
}

std::vector<std::string> ModelTool::get_functions_by_dsp_port_uuid(const std::string& port_uuid) const {
    auto is_function_on_dsp_port = [&port_uuid] (const PcieFunction& f) -> bool {
        return f.get_dsp_port_uuid() == port_uuid; };

    return get_manager<PcieFunction>().get_keys(is_function_on_dsp_port);
}

std::vector<std::string> ModelTool::get_endpoints_by_drive_uuid(const std::string& drive_uuid) const {
    auto does_endpoint_use_drive = [&drive_uuid] (const Endpoint& e) -> bool {
        for (const auto& entity : e.get_connected_entities()) {
            if (entity.get_entity().has_value() && entity.get_entity() == drive_uuid) {
                return true;
            }
        }
        return false;
    };

    return get_manager<Endpoint>().get_keys(does_endpoint_use_drive);
}

void ModelTool::degenerate_endpoint(const std::string& endpoint_uuid, const std::vector<std::string>& drive_list) const {
    auto endpoint = get_manager<Endpoint>().get_entry_reference(endpoint_uuid);
    attribute::Status status = endpoint->get_status();
    status.set_state(enums::State::Disabled);
    status.set_health(enums::Health::OK);
    endpoint->set_status(std::move(status));

    auto entities = endpoint->get_connected_entities();
    for (auto& entity : entities) {
        if (0 != std::count(drive_list.begin(), drive_list.end(), entity.get_entity())) {
            entity.set_entity(OptionalField<std::string>{});
        }
    }
    endpoint->set_connected_entities(std::move(entities));
    this->send_event(endpoint->get_parent_uuid(), endpoint_uuid, enums::Component::Endpoint, Notification::Update);
}

void ModelTool::regenerate_endpoint(const std::string& endpoint_uuid, const std::string& drive_uuid) const {
    auto endpoint = get_manager<Endpoint>().get_entry_reference(endpoint_uuid);
    attribute::Status status = endpoint->get_status();
    status.set_state(enums::State::Enabled);
    status.set_health(enums::Health::OK);
    endpoint->set_status(std::move(status));

    auto entities = endpoint->get_connected_entities();
    for (auto& entity : entities) {
        if (!entity.get_entity().has_value()
            && entity.get_entity_role().has_value() && entity.get_entity_role() == enums::EntityRole::Target
            && entity.get_entity_type().has_value() && entity.get_entity_type() == enums::EntityType::Drive)
        entity.set_entity(drive_uuid);
    }
    endpoint->set_connected_entities(entities);
    this->send_event(endpoint->get_parent_uuid(), endpoint_uuid, enums::Component::Endpoint, Notification::Update);
}

std::vector<std::string> ModelTool::get_ports_by_drive_uuid(const std::string& drive_uuid) const {
    auto functions = get_m2m_manager<Drive, PcieFunction>().get_children(drive_uuid);
    std::set<std::string> ports{};
    for (const auto& function_uuid : functions) {
        ports.insert(get_manager<PcieFunction>().get_entry(function_uuid).get_dsp_port_uuid());
    }
    std::vector<std::string> res{};
    std::copy(ports.begin(), ports.end(), std::back_inserter(res));
    return res;
}

std::string ModelTool::get_switch_for_drive_uuid(const std::string& drive_uuid) const {
    // currently we support only one switch
    auto ports = this->get_ports_by_drive_uuid(drive_uuid);
    if (ports.empty()) {
        log_error(GET_LOGGER("pnc-gami"), "Drive does not have any pcie function connected to it");
        throw std::runtime_error("No pcie functions for a drive");
    }
    return get_manager<Port>().get_entry(ports.front()).get_parent_uuid();
}

void ModelTool::set_drive_is_in_warning_state(const std::string& drive_uuid, bool is_in_warning_state) const {
    get_manager<Drive>().get_entry_reference(drive_uuid)->set_is_in_warning_state(is_in_warning_state);
}

void ModelTool::set_drive_is_being_erased(const std::string& drive_uuid, bool is_being_erased) const {
    get_manager<Drive>().get_entry_reference(drive_uuid)->set_is_being_erased(is_being_erased);
}

void ModelTool::set_drive_is_being_discovered(const std::string& drive_uuid, bool is_being_discovered) const {
    get_manager<Drive>().get_entry_reference(drive_uuid)->set_is_being_discovered(is_being_discovered);
}

void ModelTool::set_drive_status(const std::string& drive_uuid,
        const agent_framework::model::attribute::Status& status) const {
    update_drive_status(drive_uuid, status,
        get_manager<Drive>().get_entry(drive_uuid).get_predicted_media_life_left());
}
