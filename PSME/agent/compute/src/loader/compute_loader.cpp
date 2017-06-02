/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
 *
 * */

#include "loader/compute_loader.hpp"
#include "configuration/configuration.hpp"
#include "logger/logger_factory.hpp"
#include "agent-framework/module/common_components.hpp"

using namespace agent::compute::loader;
using namespace agent::compute;
using namespace std;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

using agent_framework::module::CommonComponents;

namespace {

#define ENABLE_CONFIGURATION_ENCRYPTION // Enable by default.

#ifdef ENABLE_CONFIGURATION_ENCRYPTION
    inline std::string decrypt_value(const std::string& value) {
        return configuration::Configuration::get_instance().decrypt_value(value);
    }
#else
    inline std::string decrypt_value(const std::string& value) {
        return value;
    }
#endif

    void check_required_fields(const json::Value& config) {
        if (!config["agent"].is_object()) {
            throw std::runtime_error("'agent' field is required.");
        }

        if (!config["agent"]["vendor"].is_string()) {
            throw std::runtime_error("'agent:vendor' field is required.");
        }

        if (!config["agent"]["capabilities"].is_array()) {
            throw std::runtime_error("'agent:capabilities' field is required.");
        }

        if (!config["registration"].is_object()) {
            throw std::runtime_error("'registration' field is required.");
        }

        if (!config["registration"]["ipv4"].is_string()) {
            throw std::runtime_error("'registration:ipv4' field is required.");
        }

        if (!config["registration"]["port"].is_number()) {
            throw std::runtime_error("'registration:port' field is required.");
        }

        if (!config["registration"]["interval"].is_number()) {
            throw std::runtime_error("'registration:interval' field is required.");
        }

        if (!config["server"].is_object()) {
            throw std::runtime_error("'server' field is required.");
        }

        if (!config["server"]["port"].is_number()) {
            throw std::runtime_error("'server:port' field is required.");
        }

        if (!config["managers"].is_array()) {
            throw std::runtime_error("'managers' field is required.");
        }

        auto& managers_array = config["managers"].as_array();

        if (managers_array.empty()) {
            throw std::runtime_error("'managers' array should have at least one entry.");
        }

        for(const auto& manager: managers_array) {
            if (!manager["ipv4"].is_string()) {
                throw std::runtime_error("Each entry in manager must have ipv4 field.");
            }
            if (!manager["username"].is_string()) {
                throw std::runtime_error("Each entry in manager must have username field.");
            }
            if (!manager["password"].is_string()) {
                throw std::runtime_error("Each entry in manager must have password field.");
            }
            if (!manager["port"].is_number()) {
                throw std::runtime_error("Each entry in manager must have port field.");
            }
            if (!manager["slot"].is_number()) {
                throw std::runtime_error("Each entry in manager must have slot field.");
            }
            if (!manager["switchPortIdentifier"].is_string()) {
                throw std::runtime_error("Each entry in manager must have switchPortIdentifier field.");
            }
            if (!manager["serialConsoleEnabled"].is_boolean()) {
                throw std::runtime_error("Each entry in manager must have serialConsoleEnabled field.");
            }
        }
    }

    Manager build_manager(const json::Value& element) {
        Manager manager{};
        ConnectionData connection{};

        SerialConsole console{};
        console.set_enabled(element["serialConsoleEnabled"].as_bool());
        manager.set_serial_console(std::move(console));

        manager.set_ipv4_address(element["ipv4"].as_string());
        connection.set_ip_address(element["ipv4"].as_string());
        connection.set_username(decrypt_value(element["username"].as_string()));
        connection.set_password(decrypt_value(element["password"].as_string()));
        connection.set_port(element["port"].as_uint());
        manager.set_connection_data(connection);
        manager.set_slot(element["slot"].as_uint());
        manager.set_switch_port_identifier(element["switchPortIdentifier"].
                                           as_string());

        manager.add_collection(attribute::Collection(
            enums::CollectionName::Systems,
            enums::CollectionType::Systems,
            ""
        ));
        manager.add_collection(attribute::Collection(
            enums::CollectionName::Chassis,
            enums::CollectionType::Chassis,
            ""));

        attribute::Status status;
        status.set_health(enums::Health::OK);
        status.set_state(enums::State::Enabled);
        manager.set_status(status);

        return manager;
    }

    Chassis build_chassis(const string& parent_uuid,
                          const json::Value& config) {
        Chassis chassis{parent_uuid};

        attribute::Status status;
        status.set_state(enums::State::Enabled);
        chassis.set_status(status);
        chassis.set_type(enums::ChassisType::Module);
        chassis.set_location_offset(config["slot"].as_uint());
        chassis.add_collection(attribute::Collection(
            enums::CollectionName::Drives,
            enums::CollectionType::Drives,
            ""
        ));
        return chassis;
    }

    void build_compute_agent(const json::Value& config) {
        auto& managers_array = config["managers"];

        auto cc = CommonComponents::get_instance();
        for (const auto& element: managers_array) {

            auto manager = build_manager(element);
            cc->get_module_manager().add_entry(manager);

            auto chassis = build_chassis(manager.get_uuid(), element);
            cc->get_chassis_manager().add_entry(chassis);
        }
    }
}

bool ComputeLoader::load(const json::Value& config) {
    try {
        check_required_fields(config);
        build_compute_agent(config);
    }
    catch (const std::runtime_error& error) {
        std::cerr << "Loading modules configuration failed: " << error.what() << std::endl;
        return false;
    }

    return true;
}
