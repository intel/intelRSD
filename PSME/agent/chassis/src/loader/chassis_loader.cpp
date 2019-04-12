/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
 *
 * */

#include "loader/chassis_loader.hpp"
#include "configuration/configuration.hpp"
#include "logger/logger_factory.hpp"

#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/version.hpp"

#include <chrono>
#include <thread>



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


ChassisLoader::~ChassisLoader() {}


class LoadManagers {
public:

    void read_managers(const json::Json& json) {
        if (json.count("managers")) {
            for (const auto& elem : json["managers"]) {
                auto manager = make_manager(elem);
                read_manager(manager, elem);
            }
        }
    }


    void read_managers(const std::string& parent, const json::Json& json) {
        if (json.count("managers")) {
            for (const auto& elem : json["managers"]) {
                auto manager = make_manager(parent, elem);
                read_manager(manager, elem);
            }
        }
    }


    void generate_blade_chassis() {
        auto drawer_manager_keys = get_manager<Manager>().get_keys("");
        if (drawer_manager_keys.empty()) {
            log_warning("discovery", "Drawer managers were not loaded!");
            return;
        }
        auto blade_manager_keys = get_manager<Manager>().get_keys(drawer_manager_keys.front());

        for (const auto& key: blade_manager_keys) {
            Chassis chassis{key};
            chassis.set_type(enums::ChassisType::Module);
            get_manager<Chassis>().add_entry(chassis);

            Psu psu{chassis.get_uuid()};
            get_manager<Psu>().add_entry(psu);

            PowerZone power_zone{chassis.get_uuid()};
            get_manager<PowerZone>().add_entry(power_zone);

            ThermalZone thermal_zone{chassis.get_uuid()};
            get_manager<ThermalZone>().add_entry(thermal_zone);

            Fan fan{chassis.get_uuid()};
            get_manager<Fan>().add_entry(fan);

            ChassisSensor temperature_sensor = make_chassis_sensor(chassis.get_uuid(), enums::ReadingUnits::Celsius);
            get_manager<ChassisSensor>().add_entry(temperature_sensor);
        }
    }


private:
    void read_manager(Manager& manager, const json::Json& json) {

        if (json.value("chassis", json::Json()).is_object()) {
            auto chassis = make_chassis(manager, json["chassis"]);
            log_debug("discovery",
                      "Adding chassis:" << chassis.get_uuid() << " to manager " << manager.get_uuid());

            get_manager<Chassis>().add_entry(chassis);
        }

        log_debug("discovery", "Adding manager:" << manager.get_uuid());
        get_manager<Manager>().add_entry(manager);

        if (json.value("managers", json::Json()).is_array()) {
            log_debug("discovery", "Adding children managers to manager:" << manager.get_uuid());
            read_managers(manager.get_uuid(), json);
        }
    }


    Manager make_manager(const json::Json& json) {
        Manager manager{};

        //Chassis collection is added only to top level manager.
        manager.add_collection(attribute::Collection(
            enums::CollectionName::Chassis,
            enums::CollectionType::Chassis
        ));

        make_manager_internals(manager, json);
        return manager;
    }


    Manager make_manager(const std::string& parent, const json::Json& json) {
        Manager manager{parent};
        make_manager_internals(manager, json);
        return manager;
    }


    void make_manager_internals(Manager& manager, const json::Json& json) {
        manager.set_firmware_version(Version::VERSION_STRING);
        manager.set_slot(std::uint8_t(json.value("slot", std::uint16_t{})));

        try {
            if (json.value("chassis", json::Json()).is_object() &&
                enums::ChassisType(enums::ChassisType::Drawer).to_string() == json.value("chassis", json::Json::object()).value("type", std::string{})) {
                manager.set_manager_type(enums::ManagerInfoType::EnclosureManager);
            }

            manager.set_ipv4_address(json.value("ipv4", std::string{}));

            ConnectionData connection_data{};
            connection_data.set_ip_address(json.value("ipv4", std::string{}));
            connection_data.set_port(json.value("port", std::uint16_t{}));
            connection_data.set_username(decrypt_value(json.value("username", std::string{})));
            connection_data.set_password(decrypt_value(json.value("password", std::string{})));
            log_debug("agent", "Manager connection data loaded. IP="
                << connection_data.get_ip_address()
                << ", port=" << connection_data.get_port());
            manager.set_connection_data(connection_data);
        }
        catch (std::runtime_error& e) {
            log_warning("agent", "Cannot read connection data.");
            log_debug("agent", "Cannot read connection data: " << e.what());
        }

        manager.set_status({
                               agent_framework::model::enums::State::Enabled,
                               agent_framework::model::enums::Health::OK
                           });

        attribute::SerialConsole serial{};
        serial.set_bitrate(115200);
        serial.set_data_bits(8);
        serial.set_enabled(json.value("serialConsoleEnabled", bool{}));
        serial.set_flow_control(enums::SerialConsoleFlowControl::None);
        serial.set_parity(enums::SerialConsoleParity::None);
        serial.set_pin_out(enums::SerialConsolePinOut::Cisco);
        serial.set_stop_bits(1);
        serial.set_signal_type(enums::SerialConsoleSignalType::Rs232);
        manager.set_serial_console(std::move(serial));

    }


    Chassis make_chassis(Manager& parent, const json::Json& json) {
        Chassis chassis{parent.get_uuid()};
        try {
            chassis.set_location_offset(json.value("locationOffset", std::uint16_t{}));

            chassis.set_type(enums::ChassisType::from_string(json.value("type", std::string{})));
            if (chassis.get_type() == enums::ChassisType::Drawer) {
                /* Drawers are managed by RackManager from RMM... */
                chassis.set_is_managed(false);
                /* ...but they contain DrawerManager */
                parent.set_location(chassis.get_uuid());
            }

            const auto& parent_id_json = json.value("parentId", json::Json());
            if (parent_id_json.is_number_unsigned()) { // for backward compatibility
                chassis.set_parent_id(std::to_string(parent_id_json.get<unsigned int>()));
            }
            else {
                chassis.set_parent_id(parent_id_json.get<std::string>());
            }
        }
        catch (const std::runtime_error& e) {
            log_error("discovery", "Invalid chassis configuration.");
            log_debug("discovery", "Invalid chassis configuration: " << e.what());
        }

        if (json.value("platform", json::Json()).is_string()) {
            chassis.set_platform(enums::PlatformType::from_string(json.value("platform", std::string{})));
        }
        else {
            log_warning("discovery", "Field 'platform' missing in configuration file. Set default: BDCR");
            chassis.set_platform(enums::PlatformType::BDCR);
        }

        if (json.value("networkInterface", json::Json()).is_string()) {
            chassis.set_network_interface(json.value("networkInterface", std::string{}));
        }

        chassis.set_status({
                               agent_framework::model::enums::State::Enabled,
                               agent_framework::model::enums::Health::OK
                           });

        return chassis;
    }


    ChassisSensor make_chassis_sensor(const std::string& parent, enums::ReadingUnits unit) {
        ChassisSensor sensor{parent};
        sensor.set_reading_units(unit);
        return sensor;
    }

};


bool ChassisLoader::load(const json::Json& json) {
    try {
        LoadManagers lm{};
        lm.read_managers(json);
        lm.generate_blade_chassis();
        return true;
    }
    catch (const std::exception e) {
        log_error("discovery", "Load agent configuration failed: " << e.what());
    }
    return false;
}
