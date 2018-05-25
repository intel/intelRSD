/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * */

#include "agent-framework/module/enum/network.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/utils/json_transformations.hpp"
#include "agent-framework/module/requests/validation/network.hpp"
#include "agent-framework/module/requests/validation/common.hpp"
#include "agent-framework/version.hpp"

#include "loader/network_loader.hpp"
#include "loader/network_config.hpp"
#include "logger/logger_factory.hpp"

#include <json/json.hpp>

using namespace agent::network::loader;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::requests::validation;

namespace {
void check_required_fields(const json::Value& config) {
    if (!config.is_member("agent")) {
        throw std::runtime_error("'agent' field is required.");
    }
    const auto& agent_prop = config["agent"];
    if (!agent_prop.is_object()) {
        throw std::runtime_error("'agent' field should be an object");
    }

    if (!agent_prop.is_member("vendor") ||
            !agent_prop["vendor"].is_string()) {
        throw std::runtime_error("'agent:vendor' field is required and should be a string.");
    }

    if (!agent_prop.is_member("capabilities") ||
           !agent_prop["capabilities"].is_array()) {
        throw std::runtime_error("'agent:capabilities' field is required and should be an array.");
    }

    if (!config.is_member("registration")) {
        throw std::runtime_error("'registration' field is required.");
    }
    const auto& registration_prop = config["registration"];
    if (!registration_prop.is_object()) {
        throw std::runtime_error("'registration' field should be an object");
    }

    if (!registration_prop.is_member("ipv4") ||
            !registration_prop["ipv4"].is_string()) {
        throw std::runtime_error("'registration:ipv4' field is required and should be a string.");
    }

    if (!registration_prop.is_member("port") ||
            !registration_prop["port"].is_number()) {
        throw std::runtime_error("'registration:port' field is required and should be a number.");
    }

    if (!registration_prop.is_member("interval") ||
            !registration_prop["interval"].is_number()) {
        throw std::runtime_error("'registration:interval' field is required and should be a number.");
    }

    if (!config.is_member("server")) {
        throw std::runtime_error("'server' field is required.");
    }
    const auto& server_prop = config["server"];
    if (!server_prop.is_object()) {
        throw std::runtime_error("'server' field should be an object");
    }

    if (!server_prop.is_member("port") ||
            !server_prop["port"].is_number()) {
        throw std::runtime_error("'server:port' field is required and should be a number.");
    }

    if (!config.is_member("managers") || !config["managers"].is_array()) {
        throw std::runtime_error("'managers' field is required and should be an array.");
    }
    auto& managers_array = config["managers"].as_array();

    if (managers_array.empty()) {
        throw std::runtime_error("'managers' array should have at least one entry.");
    }

    for(const auto& manager: managers_array) {
        if (!manager.is_member("ipv4") || !manager["ipv4"].is_string()) {
            throw std::runtime_error("Each entry in manager must have 'ipv4' field.");
        }
        if (!manager.is_member("serialConsoleEnabled") || !manager["serialConsoleEnabled"].is_boolean()) {
            throw std::runtime_error("Each entry in manager must have 'serialConsoleEnabled' field.");
        }
        auto& switches = manager["switches"].as_array();

        if (switches.empty()) {
            throw std::runtime_error("'switches' array should have at least one entry.");
        }
        for (const auto& sw: switches) {
            if (!sw.is_member("mgmt_port") || !sw["mgmt_port"].is_string()) {
                throw std::runtime_error("Each entry in switches must have "
                                         "'mgmt_port' field.");
            }
            if (!sw.is_member("locationOffset") || !sw["locationOffset"].is_number()) {
                throw std::runtime_error("Each entry in switches must have "
                                         "'locationOffset' field.");
            }
            if (!sw.is_member("parentId") || !sw["parentId"].is_string()) {
                throw std::runtime_error("Each entry in switches must have "
                                         "'parentId' field.");
            }
        }
    }
}

using PrioritiesList = agent_framework::model::attribute::Array<std::uint32_t>;

EthernetSwitchPort make_port_module(const json::Value& json, const std::string& switch_uuid) {
    if (json.is_member("id") && json["id"].is_string()) {
        EthernetSwitchPort port_model{switch_uuid};
        port_model.set_port_identifier(json["id"].as_string());

        /* read port type */
        if (json.is_member("portType") && json["portType"].is_string()) {
            port_model.set_port_type(PortType::from_string(json["portType"].as_string()));
        }

        /* read DCBX state */
        if (json.is_member(literals::EthernetSwitchPort::DCBX_STATE) && json[literals::EthernetSwitchPort::DCBX_STATE].is_string()) {
            port_model.set_dcbx_state(json[literals::EthernetSwitchPort::DCBX_STATE]);
        }

        /* read LLDP enabled */
        if (json.is_member(literals::EthernetSwitchPort::LLDP_ENABLED) && json[literals::EthernetSwitchPort::LLDP_ENABLED].is_boolean()) {
            port_model.set_lldp_enabled(json[literals::EthernetSwitchPort::LLDP_ENABLED]);
        }

        /* read PFC enabled */
        if (json.is_member(literals::EthernetSwitchPort::PFC_ENABLED) && json[literals::EthernetSwitchPort::PFC_ENABLED].is_boolean()) {
            port_model.set_pfc_enabled(json[literals::EthernetSwitchPort::PFC_ENABLED]);
        }

        /* read PFC enabled priorities */
        if (json.is_member(literals::EthernetSwitchPort::PFC_ENABLED_PRIORITIES) &&
            json[literals::EthernetSwitchPort::PFC_ENABLED_PRIORITIES].is_array()) {

            const auto json_pfc_priorities = agent_framework::model::utils::to_json_rpc(json[literals::EthernetSwitchPort::PFC_ENABLED_PRIORITIES]);
            port_model.set_pfc_enabled_priorities(PrioritiesList::from_json(json_pfc_priorities));
        }

        log_debug("network-agent", "Created Port module [port=" << port_model.get_port_identifier()
                  << " DCBX State=" << port_model.get_dcbx_state()
                  << " LLDP Enabled=" << port_model.get_lldp_enabled()
                  << " PFC Enabled=" << port_model.get_pfc_enabled() << "]");

        return port_model;
    }
    throw std::runtime_error("'id' field for port is required.");
}

EthernetSwitch make_switch_module(const json::Value& json,
                          const std::string& manager_uuid) {
    EthernetSwitch switch_model{manager_uuid};

    /* set switch identifier */
    auto switch_count = NetworkComponents::get_instance()->
        get_switch_manager().get_entry_count();
    switch_model.set_switch_identifier(std::to_string(switch_count));

    /* read switch management port name */
    std::string mgmt_port{};
    if (json.is_member("mgmt_port") && json["mgmt_port"].is_string()) {
        mgmt_port = json["mgmt_port"].as_string();
    }

    NetworkConfig().get_instance()->
        add_switch_mgmt_port(switch_model.get_switch_identifier(), mgmt_port);

    /* read port configurations */
    if (json.is_member("ports") && json["ports"].is_array()) {
        for (const auto& port_json : json["ports"].as_array()) {
            auto port = make_port_module(port_json, switch_model.get_uuid());
            NetworkComponents::get_instance()->
                get_port_manager().add_entry(port);
        }
    }

    /* add chassis */
    Chassis chassis_model{manager_uuid};
    chassis_model.set_type(enums::ChassisType::Enclosure);
    chassis_model.set_status(attribute::Status(true));

    /* read chassis configuration */
    if (json.is_member("locationOffset") && json["locationOffset"].is_number()) {
        chassis_model.set_location_offset(json["locationOffset"].as_uint());
    }
    if (json.is_member("parentId") && json["parentId"].is_string()) {
        chassis_model.set_parent_id(json["parentId"].as_string());
    }

    switch_model.set_chassis(chassis_model.get_uuid());

    log_debug("network-agent", "Created Chassis module");
    CommonComponents::get_instance()->
        get_chassis_manager().add_entry(chassis_model);

    /* add QoS */
    if (json.is_member("qosConfiguration")) {
        const auto& qos_config = json["qosConfiguration"];

        if (qos_config.is_member(literals::EthernetSwitch::QOS_APPLICATION_PROTOCOL) &&
            qos_config[literals::EthernetSwitch::QOS_APPLICATION_PROTOCOL].is_array()) {

            agent_framework::model::attribute::Array<agent_framework::model::attribute::QosApplicationProtocol> qap_array{};

            for (const auto& array_elem : qos_config[literals::EthernetSwitch::QOS_APPLICATION_PROTOCOL]) {
                qap_array.add_entry({agent_framework::model::enums::TransportLayerProtocol::
                                     from_string(array_elem[literals::NetworkQosAttribute::PROTOCOL].as_string()),
                                     array_elem[literals::NetworkQosAttribute::PORT].as_uint(),
                                     array_elem[literals::NetworkQosAttribute::PRIORITY].as_uint()});
            }
            switch_model.set_qos_application_protocol(qap_array);
        }

        if (qos_config.is_member(literals::EthernetSwitch::QOS_PRIORITY_TO_PRIORITY_GROUP_MAPPING) &&
            qos_config[literals::EthernetSwitch::QOS_PRIORITY_TO_PRIORITY_GROUP_MAPPING].is_array()) {

            agent_framework::model::attribute::Array<agent_framework::model::attribute::QosPriorityGroupMapping> qpm_array{};

            for (const auto& array_elem : qos_config[literals::EthernetSwitch::QOS_PRIORITY_TO_PRIORITY_GROUP_MAPPING]) {
                qpm_array.add_entry({array_elem[literals::NetworkQosAttribute::PRIORITY_GROUP].as_uint(),
                                     array_elem[literals::NetworkQosAttribute::PRIORITY].as_uint()});
            }
            switch_model.set_qos_priority_group_mapping(qpm_array);
        }

        if (qos_config.is_member(literals::EthernetSwitch::QOS_BANDWIDTH_ALLOCATION) &&
            qos_config[literals::EthernetSwitch::QOS_BANDWIDTH_ALLOCATION].is_array()) {

            agent_framework::model::attribute::Array<agent_framework::model::attribute::QosBandwidthAllocation> qba_array{};

            for (const auto& array_elem : qos_config[literals::EthernetSwitch::QOS_BANDWIDTH_ALLOCATION]) {
                qba_array.add_entry({array_elem[literals::NetworkQosAttribute::PRIORITY_GROUP].as_uint(),
                                     array_elem[literals::NetworkQosAttribute::BANDWIDTH_PERCENT].as_uint()});
            }
            switch_model.set_qos_bandwidth_allocation(qba_array);
        }

        if (qos_config.is_member(literals::EthernetSwitch::LLDP_ENABLED) && qos_config[literals::EthernetSwitch::LLDP_ENABLED].is_boolean()) {
            switch_model.set_lldp_enabled(qos_config[literals::EthernetSwitch::LLDP_ENABLED]);
        }

        if (qos_config.is_member(literals::EthernetSwitch::PFC_ENABLED) && qos_config[literals::EthernetSwitch::PFC_ENABLED].is_boolean()) {
            switch_model.set_pfc_enabled(qos_config[literals::EthernetSwitch::PFC_ENABLED]);
        }
    }

    /* return new switch model object */
    log_debug("network-agent", "Created Switch module [switch=" << switch_model.get_switch_identifier()
              << " mgmt=" << NetworkConfig().get_instance()->get_switch_mgmt_port(switch_model.get_switch_identifier())
              << " LLDP Enabled=" << switch_model.get_lldp_enabled()
              << " PFC Enabled=" << switch_model.get_pfc_enabled() << "]");

    return switch_model;
}

Manager make_manager(const json::Value& element) {
    Manager manager{};
    manager.set_status({enums::State::Enabled, enums::Health::OK});
    manager.set_ipv4_address(element["ipv4"].as_string());
    manager.set_firmware_version(agent_framework::generic::Version::VERSION_STRING);
    manager.set_manager_type(enums::ManagerInfoType::EnclosureManager);

    attribute::SerialConsole console{};
    console.set_enabled(element["serialConsoleEnabled"].as_bool());
    manager.set_serial_console(std::move(console));

    auto nm = NetworkComponents::get_instance();
    if (!element["switches"].as_array().empty()) {
        for (const auto& switch_json : element["switches"].as_array()) {
            auto switch_module = make_switch_module(switch_json,
                                                    manager.get_uuid());
            nm->get_switch_manager().add_entry(switch_module);
        }
        // the chassis is the location of the manager:
        auto chassis_uuid = get_manager<Chassis>().get_keys().front();
        manager.set_location(chassis_uuid);
    }

    log_debug("network-agent", "Created Manager module [ipv4="
            << manager.get_ipv4_address() << "]");
    return manager;
}

void validate_port_modules(const json::Value& config) {

    if (config.is_member(literals::EthernetSwitchPort::PFC_ENABLED_PRIORITIES) &&
        config[literals::EthernetSwitchPort::PFC_ENABLED_PRIORITIES].is_array()) {

        const auto json_pfc_priorities = agent_framework::model::utils::to_json_rpc(
                config[literals::EthernetSwitchPort::PFC_ENABLED_PRIORITIES]);

        if (!json_pfc_priorities.empty()) {
            const auto pfc_priorities = PrioritiesList::from_json(json_pfc_priorities);
            NetworkValidator::validate_request_switch_port_priorities(pfc_priorities);
        }
    }
}

using QosApplicationProtocolList = agent_framework::model::attribute::Array<attribute::QosApplicationProtocol>;
using QosPriorityGroupMappingList = agent_framework::model::attribute::Array<attribute::QosPriorityGroupMapping>;
using QosBandwidthAllocationList = agent_framework::model::attribute::Array<attribute::QosBandwidthAllocation>;

void validate_switch_modules(const json::Value& config) {

    if (config.is_member("qosConfiguration")) {
        const auto& qos_config = config["qosConfiguration"];

        if (qos_config.is_member(literals::EthernetSwitch::QOS_APPLICATION_PROTOCOL) &&
            qos_config[literals::EthernetSwitch::QOS_APPLICATION_PROTOCOL].is_array()) {

            QosApplicationProtocolList qos_application_protocol_array{};

            for (const auto& array_elem : qos_config[literals::EthernetSwitch::QOS_APPLICATION_PROTOCOL]) {
                qos_application_protocol_array.add_entry({
                        agent_framework::model::enums::TransportLayerProtocol::
                        from_string(array_elem[literals::NetworkQosAttribute::PROTOCOL].as_string()),
                        array_elem[literals::NetworkQosAttribute::PORT].as_uint(),
                        array_elem[literals::NetworkQosAttribute::PRIORITY].as_uint()});
            }

            NetworkValidator::validate_request_switch_qos_application_protocol(qos_application_protocol_array);
        }

        if (qos_config.is_member(literals::EthernetSwitch::QOS_PRIORITY_TO_PRIORITY_GROUP_MAPPING) &&
            qos_config[literals::EthernetSwitch::QOS_PRIORITY_TO_PRIORITY_GROUP_MAPPING].is_array()) {

            QosPriorityGroupMappingList qos_priority_group_mapping_array{};

            for (const auto& array_elem : qos_config[literals::EthernetSwitch::QOS_PRIORITY_TO_PRIORITY_GROUP_MAPPING]) {
                qos_priority_group_mapping_array.add_entry({
                        array_elem[literals::NetworkQosAttribute::PRIORITY_GROUP].as_uint(),
                        array_elem[literals::NetworkQosAttribute::PRIORITY].as_uint()});
            }

            NetworkValidator::validate_request_switch_qos_priority_group_mapping(qos_priority_group_mapping_array);
        }

        if (qos_config.is_member(literals::EthernetSwitch::QOS_BANDWIDTH_ALLOCATION) &&
            qos_config[literals::EthernetSwitch::QOS_BANDWIDTH_ALLOCATION].is_array()) {

            QosBandwidthAllocationList qos_bandwidth_allocation_array{};

            for (const auto& array_elem : qos_config[literals::EthernetSwitch::QOS_BANDWIDTH_ALLOCATION]) {
                qos_bandwidth_allocation_array.add_entry({
                        array_elem[literals::NetworkQosAttribute::PRIORITY_GROUP].as_uint(),
                        array_elem[literals::NetworkQosAttribute::BANDWIDTH_PERCENT].as_uint()});
            }

            NetworkValidator::validate_request_switch_qos_bandwidth_allocation(qos_bandwidth_allocation_array);
        }
    }

    auto& ports_array = config["ports"].as_array();
    if (!ports_array.empty()) {
        for (const auto& ports_json : ports_array) {
            validate_port_modules(ports_json);
        }
    }
}

void validate_manager_modules(const json::Value& config) {
    auto& switches_array = config["switches"].as_array();
    if (!switches_array.empty()) {
        for (const auto& switches_json : switches_array) {
            validate_switch_modules(switches_json);
        }
    }
}

void validate_config_fields(const json::Value& config) {
    auto& managers_array = config["managers"].as_array();
    if (!managers_array.empty()) {
        for (const auto& managers_json : managers_array) {
            validate_manager_modules(managers_json);
        }
    }
}

}

void NetworkLoader::read_fabric_modules(const json::Value& config) {
    auto& managers_array = config["managers"].as_array();

    auto nm = CommonComponents::get_instance();
    for (const auto& element : managers_array) {
        auto manager = make_manager(element);
        nm->get_module_manager().add_entry(manager);
    }
}

bool NetworkLoader::load(const json::Value& json) {
    try {
        check_required_fields(json);
        validate_config_fields(json);
        read_fabric_modules(json);
    }
    catch (const std::exception& e) {
        log_error("network-agent",
                "Load module configuration failed: " << e.what());
        return false;
    }
    return true;
}
