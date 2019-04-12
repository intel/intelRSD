/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file rmm/command/set_component_attributes.cpp
 */

#include "ipmi/command/sdv/rmm/reset.hpp"
#include "ipmi/command/sdv/rmm/send_psu_command.hpp"
#include "ipmi/command/generic/warm_reset.hpp"
#include "ipmi/command/sdv/rmm/set_id_field.hpp"
#include "ipmi/command/sdv/rmm/set_pwm.hpp"

#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/requests/validation/common.hpp"
#include "agent-framework/module/requests/validation/rmm.hpp"
#include "agent-framework/command/rmm_commands.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/eventing/events_queue.hpp"

#include "loader/ipmi_config.hpp"
#include "database/persistent_attributes.hpp"

#include <thread>
#include <unistd.h>
#include <signal.h>
#include <cstdio>



using namespace database;
using namespace generic;
using namespace agent::rmm::loader;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent_framework::command;
using namespace agent_framework::exceptions;
using namespace agent_framework::eventing;


namespace {

static constexpr pid_t LINUX_INIT_PROCESS = 1;
static constexpr unsigned REBOOT_SLEEP_TIME_MS = 5000;
static constexpr unsigned CM_RESTART_TIME_MS = 1000;

using ChassisMemberMethod = void (Chassis::*)(const OptionalField<std::string>&);

/* Forward declaration: all attributes are set via set component attributes */
void set_component_attributes(const SetComponentAttributes::Request&, SetComponentAttributes::Response&);


void perform_rack_factory_defaults(Manager& rack) {
    log_debug("rmm-agent", "Remove all user settings from RMM [UUID = " << rack.get_uuid() << "]");
    for (const auto& uuid_attribute : PersistentAttributes::get_instance().get_persistent_settings()) {
        log_debug("rmm-agent", "Clear attribute " << uuid_attribute.second << " in " << uuid_attribute.first);

        agent_framework::model::attribute::Attributes attributes{};
        json::Json null_value = json::Json();
        attributes.set_value(uuid_attribute.second, null_value);
        try {
            SetComponentAttributes::Request request{uuid_attribute.first, attributes};
            SetComponentAttributes::Response response{};
            set_component_attributes(request, response);
        }
        catch (std::exception& e) {
            log_debug("rmm-agent", "No " << uuid_attribute.second
                                         << " found for " << uuid_attribute.first
                                         << ": " << e.what());
            PersistentAttributes::get_instance().remove_value(uuid_attribute.first, uuid_attribute.second);
        }
    }
}

/* Set a property persistently */
void set_chassis_attribute(Chassis& chassis, const std::string& name,
                           const json::Json& attribute, ChassisMemberMethod setter) {

    log_debug("rmm-agent", "Set " << name << " = " << attribute.dump()
                                  << ", chassis " << chassis.get_uuid() << ", slot="
                                  << static_cast<unsigned>(chassis.get_slot_id()));

    (chassis.*setter)(attribute);
    if (attribute.is_null()) {
        /* Remove entry from the database */
        PersistentAttributes::get_instance().remove_value(chassis.get_uuid(), name);
    }
    else {
        PersistentAttributes::get_instance().set_value(chassis.get_uuid(), name, attribute.get<std::string>());
    }
}

void perform_thermal_zone_update(ThermalZone& thermal, unsigned pwm) {
    thermal.set_desired_speed_pwm(pwm);

    auto zone = get_manager<Chassis>().get_entry(thermal.get_parent_uuid());
    ipmi::IpmiController::Ptr ipmi = IpmiConfig::get_instance().get_controller(zone.get_slot_id());

    for (const auto& fan : get_manager<Fan>().get_entries(thermal.get_uuid())) {
        ipmi::command::sdv::request::SetPwm request{};
        ipmi::command::sdv::response::SetPwm response{};
        request.set_asset_index(fan.get_slot_id());
        request.set_pwm_value(static_cast<std::uint8_t>(pwm));
        ipmi->send(request, response);
        log_debug("rmm-agent", "Updated Fan [UUID = " + fan.get_uuid() + "] PWM: " + std::to_string(pwm));
    }
}


void perform_psu_state_change(Psu& psu, const enums::State& requested_state) {
    auto power_zone = get_manager<PowerZone>().get_entry(psu.get_parent_uuid());
    auto chassis_zone = get_manager<Chassis>().get_entry(power_zone.get_parent_uuid());

    /* get chassis IPMI controller */
    ipmi::IpmiController::Ptr ipmi = IpmiConfig::get_instance().get_controller(chassis_zone.get_slot_id());

    log_debug("rmm-agent", "Running PSU state change to: " << requested_state.to_string()
        << ", psu " << psu.get_uuid() << ", slot = " << static_cast<unsigned>(psu.get_slot_id())
        << ", zone " << chassis_zone.get_uuid() << ", slot = " << static_cast<unsigned>(chassis_zone.get_slot_id()));

    ipmi::command::sdv::request::SendPsuWriteOperation ipmi_request{};
    ipmi::command::sdv::response::SendPsuWriteOperation ipmi_response{};

    if (requested_state == enums::State::Enabled) {
        ipmi_request.set_state(ipmi::command::sdv::request::SendPsuWriteOperation::State::ON);
    }
    else {
        ipmi_request.set_state(ipmi::command::sdv::request::SendPsuWriteOperation::State::IMMEDIATE_OFF);
    }

    ipmi_request.set_psu_id(psu.get_slot_id());
    ipmi->send(ipmi_request, ipmi_response);

    Status updated_status{psu.get_status()};
    updated_status.set_state(requested_state);
    psu.set_status(updated_status);
}


void perform_drawer_soft_reset(Chassis& drawer) {
    /* get chassis IPMI controller */
    auto zone = get_manager<Chassis>().get_entry(drawer.get_parent_chassis());
    ipmi::IpmiController::Ptr ipmi = IpmiConfig::get_instance().get_controller(zone.get_slot_id());

    log_debug("rmm-agent", "Running DRAWER *soft* reset" << ", drawer " << drawer.get_uuid()
                                                         << "/slot=" << static_cast<unsigned>(drawer.get_slot_id())
                                                         << ", zone " << zone.get_uuid()
                                                         << "/slot=" << static_cast<unsigned>(zone.get_slot_id()));

    /* slot id of drawer is identical as in bridging info, 0/2/4/6 values are allowed */
    ipmi::command::generic::request::WarmReset request{};
    ipmi::command::generic::response::WarmReset response{};
    ipmi->send(request, IpmiConfig::get_psme_chassis_bridge(drawer.get_slot_id()), response);
}


void perform_drawer_hard_reset(Chassis& drawer) {
    /* get chassis IPMI controller */
    auto zone = get_manager<Chassis>().get_entry(drawer.get_parent_chassis());
    ipmi::IpmiController::Ptr ipmi = IpmiConfig::get_instance().get_controller(zone.get_slot_id());

    log_debug("rmm-agent", "Running DRAWER *hard* reset" << ", drawer " << drawer.get_uuid()
                                                         << "/slot=" << static_cast<unsigned>(drawer.get_slot_id())
                                                         << ", zone " << zone.get_uuid()
                                                         << "/slot=" << static_cast<unsigned>(zone.get_slot_id()));

    ipmi::command::sdv::request::Reset request{ipmi::command::sdv::request::Reset::ComponentToReset::RRC};
    ipmi::command::sdv::response::Reset response{};
    ipmi->send(request, IpmiConfig::get_mbp_bridge(drawer.get_slot_id()), response);
}


void perform_zone_manager_hard_reset(Manager& zone_manager) {
    auto location = zone_manager.get_location(); // UUID of chassis
    if (!location.has_value()) {
        THROW(InvalidValue, "rmm-agent", "Zone " + zone_manager.get_uuid() + " without physical component.");
    }

    auto zone = get_manager<Chassis>().get_entry(location);
    if (zone.get_type() != enums::ChassisType::Zone) {
        THROW(InvalidValue, "rmm-agent", "Physical component " + zone.get_uuid() + " isn't a zone.");
    }

    log_debug("rmm-agent", "Running ZONE *hard* reset" << ", manager=" << zone_manager.get_uuid()
                                                       << ", zone " << zone.get_uuid()
                                                       << "/slot=" << static_cast<unsigned>(zone.get_slot_id()));

    /*
     * it is necessary to restart both MMP and RRC (and order is important!), otherwise
     * communication over I2C won't work. Reset MMP causes connection is broken, and it is to be
     * re-established after CM is restarted (what takes about second)
     */
    ipmi::IpmiController::Ptr ipmi = IpmiConfig::get_instance().get_controller(zone.get_slot_id());
    ipmi::command::sdv::request::Reset mmp_request{ipmi::command::sdv::request::Reset::ComponentToReset::MMP};
    ipmi::command::sdv::response::Reset mmp_response{};
    ipmi->send(mmp_request, IpmiConfig::get_cm_bridge(), mmp_response);
    ipmi->close();

    std::this_thread::sleep_for(std::chrono::milliseconds{CM_RESTART_TIME_MS});
    ipmi::command::sdv::request::Reset rrc_request{ipmi::command::sdv::request::Reset::ComponentToReset::RRC};
    ipmi::command::sdv::response::Reset rrc_response{};
    ipmi->send(rrc_request, IpmiConfig::get_cm_bridge(), rrc_response);
}


void perform_rack_soft_reset(Manager& rack) {
    log_info("rmm-agent", "Running RACK *soft* reset [UUID = " << rack.get_uuid() << "]");
    /* run it in detached thread */
    std::thread([]() {
        if (system("wall Rack is going to reboot on API request") != 0) {
            /* wall failed? no message on the console */
            log_error("rmm-agent", "Cannot log reboot warning on all consoles.");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds{REBOOT_SLEEP_TIME_MS});

        /* request kernel restart, just by sending SIGINT to init process */
        sync();
        kill(LINUX_INIT_PROCESS, SIGINT);
    }).detach();
}


void set_rack_location_id(Chassis& rack, const json::Json& location_id) {

    /* Update data fields */
    const auto& drawers_and_zones = get_manager<Chassis>().get_keys([](const Chassis& chassis) {
        return enums::ChassisType::Drawer == chassis.get_type() || enums::ChassisType::Zone == chassis.get_type();
    });

    set_chassis_attribute(rack, literals::Chassis::LOCATION_ID, location_id, &Chassis::set_location_id);
    log_info("rmm-agent", "Setting location id to " << location_id.get<std::string>() << " for rack " << rack.get_uuid());

    // Updates "parent ID" on all chassis
    for (const auto& uuid : drawers_and_zones) {
        auto chassis = get_manager<Chassis>().get_entry_reference(uuid);
        chassis->set_parent_id(location_id.get<std::string>());

        EventData event{};
        event.set_component(uuid);
        event.set_parent(chassis->get_parent_uuid());
        event.set_type(enums::Component::Chassis);
        event.set_notification(agent_framework::model::enums::Notification::Update);
        EventsQueue::get_instance()->push_back(event);
    }
}


void process_thermal_zone_attributes(const std::string& uuid, const Attributes& attributes,
                                     SetComponentAttributes::Response& response) {

    log_debug("rmm-agent", "Processing Thermal Zone [UUID = " << uuid << "]");
    requests::validation::RmmValidator::validate_set_thermal_zone_attributes(attributes);
    auto thermal = get_manager<ThermalZone>().get_entry_reference(uuid);

    for (const auto& attribute_name : attributes.get_names()) {
        try {
            const auto& value = attributes.get_value(attribute_name);

            log_debug("rmm-agent", "Attribute name: " << attribute_name);
            log_debug("rmm-agent", "Attribute value: " << value.dump());

            if (literals::ThermalZone::DESIRED_SPEED_PWM == attribute_name) {
                const auto pwm = value.get<unsigned>();
                perform_thermal_zone_update(thermal.get_raw_ref(), pwm);
            }
            else {
                THROW(UnsupportedField, "rmm-agent", "Setting attribute is not supported.", attribute_name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({attribute_name, ex.get_error_code(), ex.get_message()});
        }
    }
}

void process_psu_state_change(const std::string& uuid, const enums::State& requested_state) {
    if (requested_state == enums::State::Enabled || requested_state == enums::State::Disabled) {
        auto psu = get_manager<Psu>().get_entry_reference(uuid);
        perform_psu_state_change(psu.get_raw_ref(), requested_state);
    }
    else {
        THROW(InvalidValue, "rmm-agent", "Not allowed state to change for PSU: " +
            std::string(requested_state.to_string()) + ".");
    }
}

void process_psu_attributes(const std::string& uuid, const Attributes& attributes,
                            SetComponentAttributes::Response& response) {

    log_debug("rmm-agent", "Processing PSU [UUID = " << uuid << "]");
    requests::validation::RmmValidator::validate_set_psu_attributes(attributes);

    for (const auto& attribute_name : attributes.get_names()) {
        try {
            const auto& value = attributes.get_value(attribute_name);

            log_debug("rmm-agent", "Attribute name: " << attribute_name);
            log_debug("rmm-agent", "Attribute value: " << value.dump());

            if (literals::Psu::REQUESTED_STATE == attribute_name) {
                const auto state = enums::State::from_string(value.get<std::string>());
                process_psu_state_change(uuid, state);
            }
            else {
                THROW(UnsupportedField, "rmm-agent", "Setting attribute is not supported.", attribute_name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({attribute_name, ex.get_error_code(), ex.get_message()});
        }
    }
}

void process_setting_rack_location_id(Chassis& chassis, const json::Json& location_id) {
    if (chassis.get_type() == enums::ChassisType::Rack) {
        set_rack_location_id(chassis, location_id);
    }
    else {
        THROW(InvalidValue, "rmm-agent", "Setting location ID can be performed only on Chassis type Rack.");
    }
}

void process_setting_rack_geo_tag(Chassis& chassis, const json::Json& geo_tag) {
    if (chassis.get_type() == enums::ChassisType::Rack) {
        set_chassis_attribute(chassis, literals::Chassis::GEO_TAG, geo_tag, &Chassis::set_geo_tag);
    }
    else {
        THROW(InvalidValue, "rmm-agent", "Setting geo-location can be performed only on Chassis type Rack.");
    }
}

void process_chassis_reset(Chassis& chassis, const enums::ResetType reset) {
    if (chassis.get_type() == enums::ChassisType::Drawer) {
        if (reset == enums::ResetType::GracefulRestart) {
            perform_drawer_soft_reset(chassis);
        }
        else if (reset == enums::ResetType::ForceRestart) {
            perform_drawer_hard_reset(chassis);
        }
        else {
            THROW(UnsupportedValue, "rmm-agent", "Unsupported reset type.");
        }
    }
    else {
        THROW(InvalidValue, "rmm-agent", "Reset can be performed only on Chassis type Drawer.");
    }
}

void process_chassis_attributes(const std::string uuid, const Attributes& attributes,
                               SetComponentAttributes::Response& response) {

    log_debug("rmm-agent", "Processing Chassis [UUID = " << uuid << "]");
    requests::validation::CommonValidator::validate_set_chassis_attributes(attributes);
    auto chassis = get_manager<Chassis>().get_entry_reference(uuid);

    for (const auto& attribute_name : attributes.get_names()) {
        try {
            const auto& value = attributes.get_value(attribute_name);

            log_debug("rmm-agent", "Attribute name: " << attribute_name);
            log_debug("rmm-agent", "Attribute value: " << value.dump());

            if (literals::Chassis::RESET == attribute_name) {
                auto reset = enums::ResetType::from_string(value.get<std::string>());
                process_chassis_reset(chassis.get_raw_ref(), reset);
            }
            else if (literals::Chassis::ASSET_TAG == attribute_name) {
                set_chassis_attribute(chassis.get_raw_ref(), literals::Chassis::ASSET_TAG, value, &Chassis::set_asset_tag);
            }
            else if (literals::Chassis::LOCATION_ID == attribute_name) {
                const auto& location_id = value.get<std::string>();
                process_setting_rack_location_id(chassis.get_raw_ref(), location_id);
            }
            else if (literals::Chassis::GEO_TAG == attribute_name) {
                process_setting_rack_geo_tag(chassis.get_raw_ref(), value);
            }
            else {
                THROW(UnsupportedField, "rmm-agent", "Setting attribute is not supported.", attribute_name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({attribute_name, ex.get_error_code(), ex.get_message()});
        }
    }
}

void process_manager_reset(Manager& manager, const enums::ResetType reset) {
    if (manager.get_manager_type() == enums::ManagerInfoType::RackManager) {
        if (reset == enums::ResetType::GracefulRestart) {
            perform_rack_soft_reset(manager);
        }
        else {
            THROW(UnsupportedValue, "rmm-agent", "Reset type is not allowed: " + std::string(reset.to_string()) + ".");
        }
    }
    else if (manager.get_manager_type() == enums::ManagerInfoType::ManagementController) {
        if (reset == enums::ResetType::ForceRestart) {
            perform_zone_manager_hard_reset(manager);
        }
        else {
            THROW(UnsupportedValue, "rmm-agent", "Reset type is not allowed: " + std::string(reset.to_string()) + ".");
        }
    }
    else {
        THROW(UnsupportedValue, "rmm-agent",
              "Operation not available for " + std::string(manager.get_manager_type()->to_string()) + ".");
    }
}

void process_manager_restore_factory_defaults(Manager& manager, const bool restore_defaults) {
    if (manager.get_manager_type() == enums::ManagerInfoType::RackManager) {
        if (restore_defaults) {
            perform_rack_factory_defaults(manager);
        }
        else {
            log_info("rmm-agent", "Provided restore factory defaults attribute with 'false' value. Skipping.");
        }
    }
    else {
        THROW(UnsupportedValue, "rmm-agent",
              "Operation not available for " + std::string(manager.get_manager_type()->to_string()) + ".");
    }
}

void process_manager_attributes(const std::string uuid, const Attributes& attributes,
                                SetComponentAttributes::Response& response) {

    log_debug("rmm-agent", "Processing Manager [UUID = " << uuid << "]");
    requests::validation::CommonValidator::validate_set_manager_attributes(attributes);
    auto manager = get_manager<Manager>().get_entry_reference(uuid);

    for (const auto& attribute_name : attributes.get_names()) {
        try {
            const auto& value = attributes.get_value(attribute_name);

            log_debug("rmm-agent", "Attribute name: " << attribute_name);
            log_debug("rmm-agent", "Attribute value: " << value.dump());

            if (literals::Manager::RESET == attribute_name) {
                auto reset = enums::ResetType::from_string(value.get<std::string>());
                process_manager_reset(manager.get_raw_ref(), reset);
            }
            else if (literals::Manager::FACTORY_DEFAULTS == attribute_name) {
                auto restore_factory_defaults = value.get<bool>();
                process_manager_restore_factory_defaults(manager.get_raw_ref(), restore_factory_defaults);
            }
            else {
                THROW(UnsupportedField, "rmm-agent", "Setting attribute is not supported.", attribute_name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({attribute_name, ex.get_error_code(), ex.get_message()});
        }
    }
}

bool is_managed_by_rmm(const std::string& uuid) {

    return get_manager<Manager>().entry_exists(uuid) ||
           get_manager<Chassis>().entry_exists(uuid) ||
           get_manager<Fan>().entry_exists(uuid) ||
           get_manager<ThermalZone>().entry_exists(uuid) ||
           get_manager<PowerZone>().entry_exists(uuid) ||
           get_manager<Psu>().entry_exists(uuid);
}

void set_component_attributes(const SetComponentAttributes::Request& request,
                              SetComponentAttributes::Response& response) {

    log_info("rmm-agent", "Executing setComponentAttributes.");

    const auto& uuid = request.get_component();
    const auto& attributes = request.get_attributes();

    if (attributes.empty()) {
        log_info("rmm-agent", "Nothing has been changed (empty request).");
        return;
    }

    if (get_manager<Manager>().entry_exists(uuid)) {
        /* Set Manager Attributes */
        process_manager_attributes(uuid, attributes, response);
    }
    else if (get_manager<Chassis>().entry_exists(uuid)) {
        /* Set Chassis Attributes */
        process_chassis_attributes(uuid, attributes, response);
    }
    else if (get_manager<ThermalZone>().entry_exists(uuid)) {
        /* Set Thermal Zone Attributes */
        process_thermal_zone_attributes(uuid, attributes, response);
    }
    else if (get_manager<Psu>().entry_exists(uuid)) {
        /* Set PSU Attributes */
        process_psu_attributes(uuid, attributes, response);
    }
    else if (is_managed_by_rmm(uuid)) {
        THROW(InvalidValue, "rmm-agent", "Operation not available for this component.");
    }
    else {
        THROW(InvalidUuid, "rmm-agent", "No component with UUID = '" + uuid + "'.");
    }

    log_info("rmm-agent", "setComponentAttributes finished successfully.");
}

}

REGISTER_COMMAND(SetComponentAttributes, ::set_component_attributes);
