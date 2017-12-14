/*!
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
 * */

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

void NetworkValidator::validate_set_port_attributes(const Attributes& attributes) {
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

    log_debug(GET_LOGGER("network-agent"), "Request validation passed.");
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

    log_debug(GET_LOGGER("network-agent"), "Request validation passed.");
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
    log_debug(GET_LOGGER("network-agent"), "Request validation passed.");
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
    log_debug(GET_LOGGER("network-agent"), "Request validation passed.");
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
    log_debug(GET_LOGGER("network-agent"), "Request validation passed.");
}

}
}
}
}
