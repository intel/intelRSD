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
 * @file pnc_loader.cpp
 * @brief PNC Loader
 * */

#include "aardvark/aardvark_oob_tool.hpp"
#include "agent-framework/version.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/service_uuid.hpp"

#include "configuration/configuration.hpp"
#include "logger/logger_factory.hpp"
#include "loader/pnc_configuration.hpp"
#include "loader/pnc_loader.hpp"
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


void check_required_fields(const json::Json& config) {
    if (!config.value("service", json::Json()).is_string()) {
        throw std::runtime_error("'service' field is required.");
    }

    if (!config.value("agent", json::Json()).is_object()) {
        throw std::runtime_error("'agent' field is required.");
    }

    if (!config["agent"].value("vendor", json::Json()).is_string()) {
        throw std::runtime_error("'agent:vendor' field is required.");
    }

    if (!config["agent"].value("capabilities", json::Json()).is_array()) {
        throw std::runtime_error("'agent:capabilities' field is required.");
    }

    if (!config.value("registration", json::Json()).is_object()) {
        throw std::runtime_error("'registration' field is required.");
    }

    if (!config["registration"].value("ipv4", json::Json()).is_string()) {
        throw std::runtime_error("'registration:ipv4' field is required.");
    }

    if (!config["registration"].value("port", json::Json()).is_number()) {
        throw std::runtime_error("'registration:port' field is required.");
    }

    if (!config["registration"].value("interval", json::Json()).is_number()) {
        throw std::runtime_error("'registration:interval' field is required.");
    }

    if (!config.value("server", json::Json()).is_object()) {
        throw std::runtime_error("'server' field is required.");
    }

    if (!config["server"].value("port", json::Json()).is_number()) {
        throw std::runtime_error("'server:port' field is required.");
    }

    if (!config.value("managers", json::Json()).is_array()) {
        throw std::runtime_error("'PCIePorts' array is required.");
    }

    if (config.count("fabric-discovery-mode")) {
        if (!config["fabric-discovery-mode"].is_string()) {
            throw std::runtime_error("fabric-discovery-mode' should be a string value");
        }

        if (config["fabric-discovery-mode"].get<std::string>() != std::string("AUTOMATIC") &&
            config["fabric-discovery-mode"].get<std::string>() != std::string("DATABASE")) {
            throw std::runtime_error("Acceptable values for 'fabric-discovery-mode' are 'AUTOMATIC' or 'DATABASE'.");
        }
    }
    else {
        throw std::runtime_error("'fabric-discovery-mode' field is required.");
    }

    auto& managers_array = config["managers"];

    for (auto& manager_config : managers_array) {
        auto& pcie_ports_array = manager_config.count("PCIePorts") ? manager_config["PCIePorts"] : json::Json{};

        if (pcie_ports_array.empty()) {
            throw std::runtime_error("'PCIePorts' array should have at least one entry.");
        }

        for (const auto& port: pcie_ports_array) {
            if (!port.value("PortId", json::Json()).is_string()) {
                throw std::runtime_error("Each PCIePort must contain PortId field.");
            }
            if (!port.value("TwiPort", json::Json()).is_number_unsigned()) {
                throw std::runtime_error("Each PCIePort must contain TwiPort field.");
            }
            if (!port.value("TwiChannel", json::Json()).is_number_unsigned()) {
                throw std::runtime_error("Each PCIePort must contain TwiChannel field.");
            }
        }
    }

    if (!config.value("opae", json::Json()).is_object()) {
        throw std::runtime_error("'opae' configuration is required.");
    }
    if (!config["opae"].value("secureEraseGBS", json::Json()).is_string()) {
        throw std::runtime_error("'secureEraseGBS' path is required.");
    }
}


void configure_i2c_interface(const json::Json& config) {
    if (!config.value("i2c", json::Json()).is_object()) {
        throw std::runtime_error("'i2c' field for manager is required.");
    }
    const json::Json& i2c_config = config["i2c"];
    if (!i2c_config.value("interface", json::Json()).is_string()) {
        throw std::runtime_error("'interface' field for i2c in manager is required.");
    }
    std::string interface = i2c_config["interface"];
    if ("GAS" == interface) {
        log_debug("agent", "GAS has been selected as the I2c interface");
        I2cAccessInterfaceFactory::get_instance().switch_to_gas();
    }
    else if ("IPMI" == interface) {
        log_debug("agent", "IPMI has been selected as the I2c interface");
        I2cAccessInterfaceFactory::get_instance().switch_to_ipmi();
        if (!i2c_config.value("ipv4", json::Json()).is_string()) {
            throw std::runtime_error("Missing 'ipv4' for IPMI configuration.");
        }
        if (!i2c_config.value("port", json::Json()).is_number_unsigned()) {
            throw std::runtime_error("Missing 'port' for IPMI configuration.");
        }
        if (!i2c_config.value("username", json::Json()).is_string()) {
            throw std::runtime_error("Missing 'username' for IPMI configuration.");
        }
        if (!i2c_config.value("password", json::Json()).is_string()) {
            throw std::runtime_error("Missing 'password' for IPMI configuration.");
        }
        I2cAccessInterfaceFactory::get_instance().init_ipmi_interface(
            i2c_config["ipv4"].get<std::string>(),
            i2c_config["port"].get<std::uint16_t>(),
            decrypt_value(i2c_config["username"].get<std::string>()),
            decrypt_value(i2c_config["password"].get<std::string>()));
    }
    else {
        throw std::runtime_error("Unknown I2c interface type");
    }
}


void configure_aardvark_oob_tool() {
    try {
        agent::pnc::aardvark::AardvarkOobTool::get_default_instance()->init_single_device();
    }
    catch (std::runtime_error& e) {
        log_error("agent", std::string("Could not initialize Aardvark device: ") + e.what());
    }
}


Chassis make_chassis(const std::string& parent, const json::Json& json) {
    Chassis chassis{parent};
    try {
        chassis.set_location_offset(json.value("locationOffset", std::uint16_t{}));
        const auto& parent_id_json = json.value("parentId", json::Json());
        if (parent_id_json.is_number_unsigned()) { // for backward compatibility
            chassis.set_parent_id(std::to_string(parent_id_json.get<unsigned int>()));
        }
        else {
            chassis.set_parent_id(parent_id_json.get<std::string>());
        }
        chassis.set_platform(enums::PlatformType::from_string(json.value("platform", std::string{})));
    }
    catch (const std::runtime_error& e) {
        log_error("agent", "Invalid chassis configuration.");
        log_debug("agent", e.what());
    }

    if (json.value("networkInterface", json::Json()).is_string()) {
        chassis.set_network_interface(json["networkInterface"]);
    }

    chassis.set_status({
                           agent_framework::model::enums::State::Enabled,
                           agent_framework::model::enums::Health::OK
                       });
    chassis.add_collection(attribute::Collection(
        enums::CollectionName::Drives,
        enums::CollectionType::Drives
    ));

    log_info("agent", "Chassis found");
    log_debug("agent", "\tChassis uuid: " + chassis.get_uuid());
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
        enums::CollectionType::Fabrics
    ));
    manager.add_collection(attribute::Collection(
        enums::CollectionName::Systems,
        enums::CollectionType::Systems
    ));
    manager.add_collection(attribute::Collection(
        enums::CollectionName::PcieDevices,
        enums::CollectionType::PCIeDevices
    ));
    manager.add_collection(attribute::Collection(
        enums::CollectionName::Chassis,
        enums::CollectionType::Chassis
    ));

    log_info("agent", "Manager found");
    log_debug("agent", "\tManager uuid: " + manager.get_uuid());
    CommonComponents::get_instance()->get_module_manager().add_entry(manager);

    return manager;
}


void build_pnc_agent(const json::Json& config) {

    auto& managers_array = config.count("managers") ? config["managers"] : json::Json::array();

    if (managers_array.size() > 0) {

        auto manager_config = managers_array.front();

        auto manager = make_manager();
        if (manager_config.value("chassis", json::Json()).is_object()) {
            auto chassis = make_chassis(manager.get_uuid(), manager_config["chassis"]);
        }

        configure_i2c_interface(manager_config);
        configure_aardvark_oob_tool();

        if (manager_config.count("PCIePorts")) {
            // Create temporary PCIePorts list. The list will be used during discovery, and deleted after it finished.
            auto& pcie_port_array = manager_config["PCIePorts"];
            auto pc = PncComponents::get_instance();
            for (const auto& element: pcie_port_array) {
                Port pcie_port{};
                std::string port_id = element["PortId"].get<std::string>();
                pcie_port.set_port_id(port_id);
                pcie_port.set_phys_port_id(std::stoi(port_id));
                pcie_port.set_twi_port(element["TwiPort"].get<std::uint16_t>());
                pcie_port.set_twi_channel(element["TwiChannel"].get<std::uint16_t>());

                pc->get_port_manager().add_entry(pcie_port);
            }
        }
    }
    agent::pnc::PncKeyGenerator::set_agent_id(agent_framework::module::ServiceUuid::get_instance()->get_service_uuid());

    PncConfiguration::get_instance()->set_fabric_discovery_mode(config["fabric-discovery-mode"].get<std::string>());
    PncConfiguration::get_instance()->set_secure_erase_gbs(config["opae"]["secureEraseGBS"].get<std::string>());
}
}


bool PncLoader::load(const json::Json& config) {
    try {
        check_required_fields(config);
        build_pnc_agent(config);
    }
    catch (const std::runtime_error& error) {
        log_error("pnc-agent", "Loading modules configuration failed: " << error.what());
        return false;
    }

    return true;
}
