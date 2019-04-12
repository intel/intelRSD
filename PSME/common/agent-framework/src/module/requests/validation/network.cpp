/*!
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
 * */


#include "agent-framework/module/model/model_network.hpp"
#include "agent-framework/module/requests/validation/network.hpp"
#include "agent-framework/module/requests/validation/json_check_type.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/validators/procedure_validator.hpp"


using namespace agent_framework::exceptions;
using namespace agent_framework::module;
using namespace agent_framework::model::attribute;

namespace agent_framework {
namespace model {
namespace requests {
namespace validation {

using PrioritiesList = agent_framework::model::attribute::Array<std::uint32_t>;

void NetworkValidator::validate_request_switch_port_priorities(const PrioritiesList& priorities) {

    if (priorities.empty()) {
        return;
    }

    std::vector<std::uint32_t> priorities_to_check;
    std::copy(priorities.begin(), priorities.end(), std::back_inserter(priorities_to_check));

    std::sort(priorities_to_check.begin(), priorities_to_check.end());
    auto it = std::adjacent_find(priorities_to_check.begin(), priorities_to_check.end());
    if (it != priorities_to_check.end()) {
        THROW(exceptions::InvalidValue, "network-agent", "Duplicated parametres in QoS port priorities request");
    }
}

void NetworkValidator::validate_set_port_attributes(
        const EthernetSwitchPort& port, const Attributes& attributes) {
    jsonrpc::ProcedureValidator validator(
        jsonrpc::PARAMS_BY_NAME,
        literals::EthernetSwitchPort::LINK_SPEED_MBPS, VALID_OPTIONAL(VALID_NUMERIC_TYPED(UINT32)),
        literals::EthernetSwitchPort::FRAME_SIZE, VALID_OPTIONAL(VALID_NUMERIC_TYPED(UINT32)),
        literals::EthernetSwitchPort::AUTO_SENSE, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
        literals::EthernetSwitchPort::VLAN_ENABLE, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
        literals::EthernetSwitchPort::DEFAULT_VLAN, VALID_OPTIONAL(VALID_UUID),
        literals::EthernetSwitchPort::ADMINISTRATIVE_STATE, VALID_OPTIONAL(VALID_ENUM(enums::AdministrativeState)),
        literals::EthernetSwitchPort::MODE, VALID_OPTIONAL(VALID_ENUM(enums::PortMode)),
        literals::EthernetSwitchPort::OEM, VALID_OPTIONAL(VALID_JSON_OBJECT),
        literals::EthernetSwitchPort::LLDP_ENABLED, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
        literals::EthernetSwitchPort::PFC_ENABLED, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
        literals::EthernetSwitchPort::PFC_ENABLED_PRIORITIES, VALID_OPTIONAL(VALID_ARRAY_SIZE_OF(VALID_NUMERIC_EQLT(UINT32, 7), 0, 8)),
        literals::EthernetSwitchPort::DCBX_STATE, VALID_OPTIONAL(VALID_ENUM(enums::DcbxState)),
        nullptr
    );
    validator.validate(attributes.to_json());

    // additional check: does the default VLAN exist?
    if (attributes.to_json().count(literals::EthernetSwitchPort::DEFAULT_VLAN)) {
        const auto& vlan_uuid = attributes.get_value(literals::EthernetSwitchPort::DEFAULT_VLAN);

        if(!NetworkComponents::get_instance()->get_port_vlan_manager().entry_exists(vlan_uuid)) {
            THROW(exceptions::InvalidValue, "network-agent", "Provided default VLAN does not exist!");
        }
    }

    // additional check: there attributtes can be set on physical interfaces only ?
    if (attributes.to_json().count(literals::EthernetSwitchPort::LLDP_ENABLED) ||
        attributes.to_json().count(literals::EthernetSwitchPort::PFC_ENABLED) ||
        attributes.to_json().count(literals::EthernetSwitchPort::PFC_ENABLED_PRIORITIES) ||
        attributes.to_json().count(literals::EthernetSwitchPort::DCBX_STATE)) {

        // Check switch configuration for physical and working port only
        if (!port.is_physical_or_up()) {
            THROW(exceptions::InvalidValue, "network-agent",
                  std::string("QoS parameters can not be configured on ") +
                  std::string(port.get_port_identifier()) + std::string(" interface. ") +
                          std::string("This interface has been recognized as inactive - ") +
                          std::string("check if the splitter cable is plugged in."));
        }
    }

    // additional check: are any priorities duplicated?
    if (attributes.to_json().count(literals::EthernetSwitchPort::PFC_ENABLED_PRIORITIES)) {
        const auto& attribute_value = attributes.get_value(literals::EthernetSwitchPort::PFC_ENABLED_PRIORITIES);

        validate_request_switch_port_priorities(PrioritiesList::from_json(attribute_value));
    }

    log_debug("network-agent", "setPortAttributes - Request validation passed.");
}

namespace {

class VlanIdSchema {
public:
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static jsonrpc::ProcedureValidator procedure{
            jsonrpc::PARAMS_BY_NAME,
            literals::AclRule::ID, VALID_NUMERIC_TYPED(UINT32),
            literals::AclRule::MASK, VALID_OPTIONAL(VALID_NUMERIC_TYPED(UINT32)),
            nullptr
        };
        return procedure;
    }
};

class AddressSchema {
public:
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static jsonrpc::ProcedureValidator procedure{
            jsonrpc::PARAMS_BY_NAME,
            literals::AclRule::ADDRESS, VALID_JSON_STRING,
            literals::AclRule::MASK, VALID_OPTIONAL(VALID_JSON_STRING),
            nullptr
        };
        return procedure;
    }
};

class PortSchema {
public:
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static jsonrpc::ProcedureValidator procedure{
            jsonrpc::PARAMS_BY_NAME,
            literals::AclRule::PORT, VALID_NUMERIC_TYPED(UINT32),
            literals::AclRule::MASK, VALID_OPTIONAL(VALID_NUMERIC_TYPED(UINT32)),
            nullptr
        };
        return procedure;
    }
};

class QosSchema {
public:
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static constexpr const uint32_t MAX_PRIORITY{7};
        static constexpr const uint32_t MAX_PRIORITY_GROUP{15};
        static constexpr const uint32_t MAX_PORT{65535};
        static constexpr const uint32_t MAX_PERCENT{100};
        static jsonrpc::ProcedureValidator procedure{
            jsonrpc::PARAMS_BY_NAME,
            literals::NetworkQosAttribute::PRIORITY, VALID_OPTIONAL(VALID_NUMERIC_EQLT(UINT32, MAX_PRIORITY)),
            literals::NetworkQosAttribute::PRIORITY_GROUP, VALID_OPTIONAL(VALID_NUMERIC_EQLT(UINT32, MAX_PRIORITY_GROUP)),
            literals::NetworkQosAttribute::BANDWIDTH_PERCENT, VALID_OPTIONAL(VALID_NUMERIC_EQLT(UINT32, MAX_PERCENT)),
            literals::NetworkQosAttribute::PORT, VALID_OPTIONAL(VALID_NUMERIC_EQLT(UINT32, MAX_PORT)),
            literals::NetworkQosAttribute::PROTOCOL, VALID_OPTIONAL(VALID_ENUM(enums::TransportLayerProtocol)),
            nullptr
        };
        return procedure;
    }
};

}

using QosApplicationProtocolList = agent_framework::model::attribute::Array<attribute::QosApplicationProtocol>;

void NetworkValidator::validate_request_switch_qos_application_protocol(
        const QosApplicationProtocolList& application_mappings) {

    if (application_mappings.empty()) {
        return;
    }

    for (auto it = application_mappings.begin(); it != std::prev(application_mappings.end()); ++it) {
        auto mappings = *it;
        auto duplicated = std::any_of(std::next(it), application_mappings.end(),
                                      [mappings](const attribute::QosApplicationProtocol& m) {
                                          return mappings.get_protocol() == m.get_protocol() &&
                                                 mappings.get_port() == m.get_port();
                                      });
        if (duplicated) {
            THROW(InvalidValue, "network-agent", "Duplicated parametres in QoS application protocol mapping request");
        }
    }
}

using QosPriorityGroupMappingList = agent_framework::model::attribute::Array<attribute::QosPriorityGroupMapping>;

void NetworkValidator::validate_request_switch_qos_priority_group_mapping(
        const QosPriorityGroupMappingList& priority_mappings) {

    if (priority_mappings.empty()) {
        return;
    }

    for (auto it = priority_mappings.begin(); it != std::prev(priority_mappings.end()); ++it) {
        auto mappings = *it;
        bool duplicated = std::any_of(std::next(it), priority_mappings.end(),
                                      [mappings](const attribute::QosPriorityGroupMapping& m) {
                                          return mappings.get_priority() == m.get_priority();
                                      });
        if (duplicated) {
            THROW(InvalidValue, "network-agent", "Duplicated parametres in QoS priority group mapping request");
        }
    }
}

constexpr int MAX_TOTAL_BANDWIDTH_ALLOCATION = 100;

using QosBandwidthAllocationList = agent_framework::model::attribute::Array<attribute::QosBandwidthAllocation>;

void NetworkValidator::validate_request_switch_qos_bandwidth_allocation(
        const QosBandwidthAllocationList& bandwidth_allocations) {

    if (bandwidth_allocations.empty()) {
        return;
    }

    int total_bandwidth(0);
    for (auto it = bandwidth_allocations.begin(); it != bandwidth_allocations.end(); ++it) {
        auto allocations = *it;
        bool duplicated = std::any_of(std::next(it), bandwidth_allocations.end(),
                                      [allocations](const attribute::QosBandwidthAllocation& a) {
                                          return allocations.get_priority_group() == a.get_priority_group();
                                      });
        if (duplicated) {
            THROW(InvalidValue, "network-agent", "Duplicated parametres in QoS bandwidth allocation request");
        }
        total_bandwidth += allocations.get_bandwidth_percent();
    }

    if (total_bandwidth > MAX_TOTAL_BANDWIDTH_ALLOCATION) {
        THROW(InvalidValue, "network-agent", "Total bandwidth exeeded " +
                            std::to_string(MAX_TOTAL_BANDWIDTH_ALLOCATION) + "% in QoS bandwidth allocation request");
    }
}

void NetworkValidator::validate_set_switch_attributes(const Attributes& attributes) {
    jsonrpc::ProcedureValidator validator(
        jsonrpc::PARAMS_BY_NAME,
        literals::EthernetSwitch::LLDP_ENABLED, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
        literals::EthernetSwitch::PFC_ENABLED, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
        literals::EthernetSwitch::ETS_ENABLED, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
        literals::EthernetSwitch::DCBX_ENABLED, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
        literals::EthernetSwitch::QOS_APPLICATION_PROTOCOL, VALID_OPTIONAL(VALID_ARRAY_OF(VALID_ATTRIBUTE(QosSchema))),
        literals::EthernetSwitch::QOS_PRIORITY_TO_PRIORITY_GROUP_MAPPING, VALID_OPTIONAL(VALID_ARRAY_OF(VALID_ATTRIBUTE(QosSchema))),
        literals::EthernetSwitch::QOS_BANDWIDTH_ALLOCATION, VALID_OPTIONAL(VALID_ARRAY_OF(VALID_ATTRIBUTE(QosSchema))),
        nullptr
    );
    validator.validate(attributes.to_json());

    // additional check: are any application protocol duplicated?
    if (attributes.to_json().count(literals::EthernetSwitch::QOS_APPLICATION_PROTOCOL)) {
        const auto& attribute_value = attributes.get_value(literals::EthernetSwitch::QOS_APPLICATION_PROTOCOL);

        validate_request_switch_qos_application_protocol(QosApplicationProtocolList::from_json(attribute_value));
    }

    // additional check: are any priority group mapping duplicated?
    if (attributes.to_json().count(literals::EthernetSwitch::QOS_PRIORITY_TO_PRIORITY_GROUP_MAPPING)) {
        const auto& attribute_value = attributes.get_value(literals::EthernetSwitch::QOS_PRIORITY_TO_PRIORITY_GROUP_MAPPING);

        validate_request_switch_qos_priority_group_mapping(QosPriorityGroupMappingList::from_json(attribute_value));
    }

    // additional check: are any bandwidth allocation duplicated?
    if (attributes.to_json().count(literals::EthernetSwitch::QOS_BANDWIDTH_ALLOCATION)) {
        const auto& attribute_value = attributes.get_value(literals::EthernetSwitch::QOS_BANDWIDTH_ALLOCATION);

        validate_request_switch_qos_bandwidth_allocation(QosBandwidthAllocationList::from_json(attribute_value));
    }

    log_debug("network-agent", "setSwitchAttributes - Request validation passed.");
}

void NetworkValidator::validate_set_acl_rule_attributes(const Attributes& attributes) {
    static jsonrpc::ProcedureValidator validator(
        jsonrpc::PARAMS_BY_NAME,
        literals::AclRule::ACTION, VALID_OPTIONAL(VALID_ENUM(enums::AclAction)),
        literals::AclRule::FORWARD_MIRROR_PORT, VALID_OPTIONAL(VALID_JSON_STRING),
        literals::AclRule::MIRRORED_PORTS, VALID_OPTIONAL(VALID_ARRAY_OF(VALID_JSON_STRING)),
        literals::AclRule::MIRROR_TYPE, VALID_OPTIONAL(VALID_ENUM(enums::AclMirrorType)),
        literals::AclRule::VLAN_ID, VALID_OPTIONAL(VALID_ATTRIBUTE(VlanIdSchema)),
        literals::AclRule::SOURCE_IP, VALID_OPTIONAL(VALID_ATTRIBUTE(AddressSchema)),
        literals::AclRule::DESTINATION_IP, VALID_OPTIONAL(VALID_ATTRIBUTE(AddressSchema)),
        literals::AclRule::SOURCE_MAC, VALID_OPTIONAL(VALID_ATTRIBUTE(AddressSchema)),
        literals::AclRule::DESTINATION_MAC, VALID_OPTIONAL(VALID_ATTRIBUTE(AddressSchema)),
        literals::AclRule::SOURCE_L4_PORT, VALID_OPTIONAL(VALID_ATTRIBUTE(PortSchema)),
        literals::AclRule::DESTINATION_L4_PORT, VALID_OPTIONAL(VALID_ATTRIBUTE(PortSchema)),
        nullptr
    );
    validator.validate(attributes.to_json());

    log_debug("network-agent", "setAclRuleAttributes - Request validation passed.");
}

void NetworkValidator::validate_set_vlan_attributes(const Attributes& attributes) {

    const auto& attributes_names = attributes.get_names();
    for (const auto& name : attributes_names) {
        const auto& value = attributes.get_value(name);

        if (literals::Vlan::VLAN_NAME == name) {
            check_string(value, name, "network-agent");
        }
        else if (literals::Vlan::VLAN_ENABLE == name) {
            check_boolean(value, name, "network-agent");
        }
        else if (literals::Vlan::OEM == name) {
            Oem::from_json(value);
        }
        else {
            THROW(InvalidField, "network-agent", "Unrecognized attribute.", name, value);
        }
    }
    log_debug("network-agent", "setVlanAttributes - Request validation passed.");
}

void NetworkValidator::validate_set_port_vlan_attributes(const Attributes& attributes) {

    const auto& attributes_names = attributes.get_names();
    for (const auto& name : attributes_names) {
        const auto& value = attributes.get_value(name);

        if (literals::Vlan::VLAN_NAME == name) {
            check_string(value, name, "network-agent");
        }
        else if (literals::Vlan::VLAN_ENABLE == name) {
            check_boolean(value, name, "network-agent");
        }
        else if (literals::Vlan::VLAN_ID == name) {
            check_number(value, name, "network-agent");
        }
        else if (literals::Vlan::OEM == name) {
            Oem::from_json(value);
        }
        else {
            THROW(InvalidField, "network-agent", "Unrecognized attribute.", name, value);
        }
    }
    log_debug("network-agent", "setPortVlanAttributes - Request validation passed.");
}

void NetworkValidator::validate_set_static_mac_attributes(const Attributes& attributes) {

    const auto& attributes_names = attributes.get_names();
    for (const auto& name : attributes_names) {
        const auto& value = attributes.get_value(name);

        if (literals::StaticMac::VLAN_ID == name) {
            check_number(value, name, "network-agent");
        }
        else if (literals::Vlan::OEM == name) {
            Oem::from_json(value);
        }
        else {
            THROW(InvalidField, "network-agent", "Unrecognized attribute.", name, value);
        }
    }
    log_debug("network-agent", "setStaticMacAttributes - Request validation passed.");
}

}
}
}
}
