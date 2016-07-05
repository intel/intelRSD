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
 *
 * */


#include "loader/chassis_loader.hpp"
#include "configuration/configuration.hpp"
#include "logger/logger_factory.hpp"
#include "agent-framework/module-ref/chassis_manager.hpp"
#include "agent-framework/version.hpp"

using namespace agent::chassis::loader;
using namespace agent_framework::generic;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::model::attribute;

#define ENABLE_CONFIGURATION_ENCRYPTION

namespace {
#ifdef ENABLE_CONFIGURATION_ENCRYPTION
    inline std::string decrypt_value(const std::string& value) {
        return configuration::Configuration::get_instance().decrypt_value(value);
    }
#else
    inline std::string decrypt_value(const std::string& value) {
    return value;
}
#endif
}

using ChassisComponents = agent_framework::module::ChassisManager;

ChassisLoader::~ChassisLoader() {}

class LoadManagers {
public:

    void read_managers(const json::Value& json) {
        for (const auto& elem : json["managers"].as_array()) {
            auto manager = make_manager(elem);
            read_manager(manager, elem);
        }
    }

    void read_managers(const std::string& parent, const json::Value& json) {
        for (const auto& elem : json["managers"].as_array()) {
            auto manager = make_manager(parent, elem);
            read_manager(manager, elem);
        }
    }

    void generate_blade_chassis() {
        auto drawer_manager_keys = ChassisComponents::get_instance()->
                get_module_manager().get_keys("");
        auto blade_manager_keys = ChassisComponents::get_instance()->
                get_module_manager().get_keys(drawer_manager_keys.front());

        for (const auto& key: blade_manager_keys) {
            Chassis chassis{key};
            chassis.set_type(enums::ChassisType::Module);
            ChassisComponents ::get_instance()->
                    get_chassis_manager().add_entry(chassis);

            Psu psu{chassis.get_uuid()};
            ChassisComponents ::get_instance()->
                    get_psu_manager().add_entry(psu);

            PowerZone power_zone{chassis.get_uuid()};
            ChassisComponents ::get_instance()->
                    get_power_zone_manager().add_entry(power_zone);

            ThermalZone thermal_zone{chassis.get_uuid()};
            ChassisComponents ::get_instance()->
                    get_thermal_zone_manager().add_entry(thermal_zone);

            Fan fan{chassis.get_uuid()};
            ChassisComponents ::get_instance()->
                    get_fan_manager().add_entry(fan);
        }
    }

private:
    void read_manager(Manager& manager, const json::Value& json) {

        log_debug(GET_LOGGER("agent"), "Adding manager:" << manager.get_uuid());
        ChassisComponents::get_instance()->
                get_module_manager().add_entry(manager);

        if (json["chassis"].is_object()) {
            auto chassis = make_chassis(manager.get_uuid(), json["chassis"]);
            log_debug(GET_LOGGER("agent"), "Adding chassis:" << chassis.get_uuid()
                                           << " to manager " << manager.get_uuid());
            ChassisComponents ::get_instance()->
                    get_chassis_manager().add_entry(chassis);
        }
        if (json["managers"].is_array()) {
            log_debug(GET_LOGGER("agent"), "Adding children managers to manager:" << manager.get_uuid());
            read_managers(manager.get_uuid(), json);
        }
    }

    Manager make_manager(const json::Value& json) {
        Manager manager{};

        //Chassis collection is added only to top level manager.
        manager.add_collection(attribute::Collection(
                enums::CollectionName::Chassis,
                enums::CollectionType::Chassis,
                "slotMask"
        ));

        make_manager_internals(manager, json);
        return manager;
    }

    Manager make_manager(const std::string& parent, const json::Value& json) {
        Manager manager{parent};
        make_manager_internals(manager, json);
        return manager;
    }

    void make_manager_internals(Manager& manager, const json::Value& json) {
        manager.set_firmware_version(Version::to_string());
        manager.set_slot(uint8_t(json["slot"].as_uint()));

        try {
            // TODO: add literals for e.g. Chassis types,
            // right now the enums cannot be compared
            // statically, that is without creating both
            // instances of them.
            if(json["chassis"].is_object() &&
               "Drawer" == json["chassis"]["type"].as_string()) {
                manager.set_manager_type(
                    enums::ManagerInfoType::EnclosureManager);
            }

            manager.set_ipv4_address(json["ipv4"].as_string());

            ConnectionData connection_data{};
            connection_data.set_ip_address(json["ipv4"].as_string());
            connection_data.set_port(json["port"].as_uint());
            connection_data.set_username(decrypt_value(json["username"].as_string()));
            connection_data.set_password(decrypt_value(json["password"].as_string()));
            log_debug(GET_LOGGER("agent"), "Manager connection data loaded. Ip="
                                             << connection_data.get_ip_address()
                                             << ", port=" << connection_data.get_port());
            manager.set_connection_data(connection_data);
        }
        catch (std::runtime_error& e) {
            log_warning(GET_LOGGER("agent"), "Cannot read connection data.");
            log_debug(GET_LOGGER("agent"), e.what());
        }

        manager.set_status({
                                   agent_framework::model::enums::State::Enabled,
                                   agent_framework::model::enums::Health::OK
                           });

        attribute::SerialConsole serial{};
        serial.set_bitrate(115200);
        serial.set_data_bits(8);
        serial.set_enabled(json["serialConsoleEnabled"].as_bool());
        serial.set_flow_control(enums::SerialConsoleFlowControl::None);
        serial.set_parity(enums::SerialConsoleParity::None);
        serial.set_pin_out(enums::SerialConsolePinOut::Cisco);
        serial.set_stop_bits(1);
        serial.set_signal_type(enums::SerialConsoleSignalType::Rs232);
        manager.set_serial_console(std::move(serial));

    }

    Chassis make_chassis(const std::string& parent, const json::Value& json) {
        Chassis chassis{parent};
        try {
            chassis.set_type(enums::ChassisType::from_string(json["type"].as_string()));
            chassis.set_size(json["size"].as_uint());
            chassis.set_location_offset(json["locationOffset"].as_uint());
            chassis.set_parent_id_as_uint(json["parentId"].as_uint());
        }
        catch (const std::runtime_error& e) {
            log_error(GET_LOGGER("agent"), "Invalid chassis configuration.");
            log_debug(GET_LOGGER("agent"), e.what());
        }

        if (json["platform"].is_string()) {
            chassis.set_platform(enums::PlatformType::from_string(json["platform"].as_string()));
        }
        if (json["networkInterface"].is_string()) {
            chassis.set_network_interface(json["networkInterface"].as_string());
        }

        chassis.set_status({
                                   agent_framework::model::enums::State::Enabled,
                                   agent_framework::model::enums::Health::OK
                           });

        return chassis;
    }

};

bool ChassisLoader::load(const json::Value& json) {
    try {
        LoadManagers lm{};
        lm.read_managers(json);
        lm.generate_blade_chassis();
    }
    catch (const json::Value::Exception& e) {
        log_error(GET_LOGGER("discovery"),
                  "Load agent configuration failed: " << e.what());
        return false;
    }
    return true;
}
