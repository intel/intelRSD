/*!
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
 * @file pnc_loader.cpp
 * @brief PNC Loader
 * */

#include <agent-framework/service_uuid.hpp>
#include "loader/pnc_loader.hpp"
#include "configuration/configuration.hpp"
#include "logger/logger_factory.hpp"
#include "agent-framework/version.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "i2c/i2c_access_interface_factory.hpp"
#include "tree_stability/pnc_key_generator.hpp"



using namespace agent::pnc::loader;
using namespace agent::pnc;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent::pnc::i2c;

using agent_framework::module::PncComponents;
using agent_framework::module::CommonComponents;

namespace {

#define ENABLE_CONFIGURATION_ENCRYPTION

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

    if (!config["service-uuid-file"].is_string()) {
        throw std::runtime_error("'service-uuid-file' field is required.");
    }

    if (!config["managers"].is_array()) {
        throw std::runtime_error("'PCIePorts' array is required.");
    }

    auto& managers_array = config["managers"].as_array();

    for (auto& manager_config : managers_array) {
        auto& pcie_ports_array = manager_config["PCIePorts"].as_array();

        if (pcie_ports_array.empty()) {
            throw std::runtime_error("'PCIePorts' array should have at least one entry.");
        }

        for (const auto& port: pcie_ports_array) {
            if (!port["PortId"].is_string()) {
                throw std::runtime_error("Each PCIePort must contain PortId field.");
            }
            if (!port["TwiPort"].is_uint()) {
                throw std::runtime_error("Each PCIePort must contain TwiPort field.");
            }
            if (!port["TwiChannel"].is_uint()) {
                throw std::runtime_error("Each PCIePort must contain TwiChannel field.");
            }
        }
    }
}


void configure_i2c_interface(const json::Value& config) {
    if (!config["i2c"].is_object()) {
        throw std::runtime_error("'i2c' field for manager is required.");
    }
    if (!config["i2c"]["interface"].is_string()) {
        throw std::runtime_error("'interface' field for i2c in manager is required.");
    }
    std::string interface = config["i2c"]["interface"].as_string();
    if ("GAS" == interface) {
        log_debug("agent", "GAS has been selected as the I2c interface");
        I2cAccessInterfaceFactory::get_instance().switch_to_gas();
    }
    else if ("IPMI" == interface) {
        log_debug("agent", "IPMI has been selected as the I2c interface");
        I2cAccessInterfaceFactory::get_instance().switch_to_ipmi();
        if (!config["i2c"]["ipv4"].is_string()) {
            throw std::runtime_error("Missing 'ipv4' for IPMI configuration.");
        }
        if (!config["i2c"]["port"].is_uint()) {
            throw std::runtime_error("Missing 'port' for IPMI configuration.");
        }
        if (!config["i2c"]["username"].is_string()) {
            throw std::runtime_error("Missing 'username' for IPMI configuration.");
        }
        if (!config["i2c"]["password"].is_string()) {
            throw std::runtime_error("Missing 'password' for IPMI configuration.");
        }
        I2cAccessInterfaceFactory::get_instance().init_ipmi_interface(
            config["i2c"]["ipv4"].as_string(),
            config["i2c"]["port"].as_uint(),
            decrypt_value(config["i2c"]["username"].as_string()),
            decrypt_value(config["i2c"]["password"].as_string()));
    }
    else {
        throw std::runtime_error("Unknown I2c interface type");
    }
}


Chassis make_chassis(const std::string& parent, const json::Value& json) {
    Chassis chassis{parent};
    try {
        chassis.set_location_offset(json["locationOffset"].as_uint());
        const auto& parent_id_json = json["parentId"];
        if (parent_id_json.is_uint()) { // for backward compatibility
            chassis.set_parent_id(std::to_string(parent_id_json.as_uint()));
        }
        else {
            chassis.set_parent_id(parent_id_json.as_string());
        }
        chassis.set_platform(enums::PlatformType::from_string(json["platform"].as_string()));
    }
    catch (const std::runtime_error& e) {
        log_error(GET_LOGGER("agent"), "Invalid chassis configuration.");
        log_debug(GET_LOGGER("agent"), e.what());
    }

    if (json["networkInterface"].is_string()) {
        chassis.set_network_interface(json["networkInterface"].as_string());
    }

    chassis.set_status({
                           agent_framework::model::enums::State::Enabled,
                           agent_framework::model::enums::Health::OK
                       });
    chassis.add_collection(attribute::Collection(
        enums::CollectionName::Drives,
        enums::CollectionType::Drives,
        ""
    ));

    log_info(GET_LOGGER("agent"), "Chassis found");
    log_debug(GET_LOGGER("agent"), "\tChassis uuid: " + chassis.get_uuid());
    CommonComponents::get_instance()->
        get_chassis_manager().add_entry(chassis);

    return chassis;
}


Manager make_manager() {
    Manager manager{};
    manager.set_firmware_version(agent_framework::generic::Version::VERSION_STRING);

    attribute::Status status;
    status.set_health(enums::Health::OK);
    status.set_state(enums::State::Enabled);
    manager.set_status(status);

    manager.set_manager_type(enums::ManagerInfoType::EnclosureManager);
    manager.add_collection(attribute::Collection(
        enums::CollectionName::Fabrics,
        enums::CollectionType::Fabrics,
        ""
    ));
    manager.add_collection(attribute::Collection(
        enums::CollectionName::Systems,
        enums::CollectionType::Systems,
        ""
    ));
    manager.add_collection(attribute::Collection(
        enums::CollectionName::PcieDevices,
        enums::CollectionType::PCIeDevices,
        ""
    ));
    manager.add_collection(attribute::Collection(
        enums::CollectionName::Chassis,
        enums::CollectionType::Chassis,
        ""
    ));

    log_info(GET_LOGGER("agent"), "Manager found");
    log_debug(GET_LOGGER("agent"), "\tManager uuid: " + manager.get_uuid());
    CommonComponents::get_instance()->get_module_manager().add_entry(manager);

    return manager;
}


void build_pnc_agent(const json::Value& config) {

    auto& managers_array = config["managers"].as_array();

    if (managers_array.size() > 0) {

        auto manager_config = managers_array.front();

        auto manager = make_manager();
        if (manager_config["chassis"].is_object()) {
            auto chassis = make_chassis(manager.get_uuid(), manager_config["chassis"]);
        }

        configure_i2c_interface(manager_config);

        // Create temporary PCIePorts list. The list will be used during discovery, and deleted after it finished.
        auto& pcie_port_array = manager_config["PCIePorts"].as_array();
        auto pc = PncComponents::get_instance();
        for (const auto& element: pcie_port_array) {
            Port pcie_port{};
            std::string port_id = element["PortId"].as_string();
            pcie_port.set_port_id(port_id);
            pcie_port.set_phys_port_id(std::stoi(port_id));
            pcie_port.set_twi_port(element["TwiPort"].as_uint());
            pcie_port.set_twi_channel(element["TwiChannel"].as_uint());

            pc->get_port_manager().add_entry(pcie_port);
        }
    }
    agent::pnc::PncKeyGenerator::set_agent_id(
        agent_framework::generic::ServiceUuid::get_instance()->get_service_uuid());
}
}


bool PncLoader::load(const json::Value& config) {
    try {
        check_required_fields(config);
        build_pnc_agent(config);
    }
    catch (const std::runtime_error& error) {
        log_error(GET_LOGGER("pnc-agent"), "Loading modules configuration failed: " << error.what());
        return false;
    }

    return true;
}
