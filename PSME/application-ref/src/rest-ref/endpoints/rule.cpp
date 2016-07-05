/*!
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
 * */

#include "psme/rest-ref/endpoints/rule.hpp"
#include "psme/rest-ref/utils/json_validator.hpp"
#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/rest-ref/model/handlers/handler_manager.hpp"
#include "psme/rest-ref/model/handlers/generic_handler_deps.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"
#include "psme/core/agent/agent_manager.hpp"

#include "agent-framework/module-ref/requests/network.hpp"
#include "agent-framework/module-ref/responses/network.hpp"
#include "agent-framework/module-ref/requests/common.hpp"
#include "agent-framework/module-ref/responses/common.hpp"

using namespace psme::rest;
using namespace psme::rest::utils;
using namespace psme::rest::error;
using namespace psme::rest::model::handler;
using namespace psme::core::agent;
using namespace app::rest;
using namespace app::rest::constants;
using namespace agent_framework::module;
using namespace agent_framework::model;
using NetworkManager = agent_framework::module::NetworkManager;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitches/"
        "Members/__SWITCH_ID__/ACLs/Members/__ACL_ID__/Rules/Members/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#ACLRule.1.0.0.ACLRule";

    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "ACL Rule";
    r[Common::DESCRIPTION] = "Access Control List Rule";

    r[Rule::RULE_ID] = json::Value::Type::NIL;
    r[Rule::ACTION] = json::Value::Type::NIL;
    r[Rule::FORWARD_MIRROR_INTERFACE] = json::Value::Type::NIL;
    r[Rule::MIRROR_PORT_REGION] = json::Value::Type::ARRAY;
    r[Rule::MIRROR_TYPE] = json::Value::Type::NIL;

    json::Value condition;
    condition[Rule::IP_SOURCE] = json::Value::Type::NIL;
    condition[Rule::IP_DESTINATION] = json::Value::Type::NIL;
    condition[Rule::MAC_SOURCE] = json::Value::Type::NIL;
    condition[Rule::MAC_DESTINATION] = json::Value::Type::NIL;
    condition[Rule::VLAN_ID] = json::Value::Type::NIL;
    condition[Rule::L4_SOURCE_PORT] = json::Value::Type::NIL;
    condition[Rule::L4_DESTINATION_PORT] = json::Value::Type::NIL;
    condition[Rule::L4_PROTOCOL] = json::Value::Type::NIL;
    r[Rule::CONDITION] = std::move(condition);

    r[Common::OEM] = json::Value::Type::OBJECT;
    r[Common::LINKS] = json::Value::Type::OBJECT;

    return r;
}

void set_condition(const AclRule& rule, json::Value& r) {
    if (rule.get_source_ip().get_address().has_value()) {
        r[Rule::CONDITION][constants::Rule::IP_SOURCE][constants::Rule::IP_ADDRESS] =
            rule.get_source_ip().get_address();
        r[Rule::CONDITION][constants::Rule::IP_SOURCE][constants::Rule::MASK] =
            rule.get_source_ip().get_mask();
    }
    if (rule.get_destination_ip().get_address().has_value()) {
        r[Rule::CONDITION][constants::Rule::IP_DESTINATION][constants::Rule::IP_ADDRESS] =
            rule.get_destination_ip().get_address();
        r[Rule::CONDITION][constants::Rule::IP_DESTINATION][constants::Rule::MASK] =
            rule.get_destination_ip().get_mask();
    }
    if (rule.get_source_mac().get_address().has_value()) {
        r[Rule::CONDITION][constants::Rule::MAC_SOURCE][constants::Rule::MAC_ADDRESS] =
            rule.get_source_mac().get_address();
        r[Rule::CONDITION][constants::Rule::MAC_SOURCE][constants::Rule::MASK] =
            rule.get_source_mac().get_mask();
    }
    if (rule.get_destination_mac().get_address().has_value()) {
        r[Rule::CONDITION][constants::Rule::MAC_DESTINATION][constants::Rule::MAC_ADDRESS] =
            rule.get_destination_mac().get_address();
        r[Rule::CONDITION][constants::Rule::MAC_DESTINATION][constants::Rule::MASK] =
            rule.get_destination_mac().get_mask();
    }
    if (rule.get_vlan_id().get_id().has_value()) {
        r[Rule::CONDITION][constants::Rule::VLAN_ID][constants::Common::ID] =
            rule.get_vlan_id().get_id();
        r[Rule::CONDITION][constants::Rule::VLAN_ID][constants::Rule::MASK] =
            rule.get_vlan_id().get_mask();
    }
    if (rule.get_source_port().get_port().has_value()) {
        r[Rule::CONDITION][constants::Rule::L4_SOURCE_PORT][constants::Rule::PORT] =
            rule.get_source_port().get_port();
        r[Rule::CONDITION][constants::Rule::L4_SOURCE_PORT][constants::Rule::MASK] =
            rule.get_source_port().get_mask();
    }
    if (rule.get_destination_port().get_port().has_value()) {
        r[Rule::CONDITION][constants::Rule::L4_DESTINATION_PORT][constants::Rule::PORT] =
            rule.get_destination_port().get_port();
        r[Rule::CONDITION][constants::Rule::L4_DESTINATION_PORT][constants::Rule::MASK] =
            rule.get_destination_port().get_mask();
    }

    r[Rule::CONDITION][constants::Rule::L4_PROTOCOL] = rule.get_protocol();
}

void set_region(const AclRule& rule, json::Value& r) {
    const auto& ports = rule.get_mirrored_ports();
    for (const auto& port : ports) {
        json::Value port_link;
        port_link[Common::ODATA_ID] =
            endpoint::utils::get_component_url(
                enums::Component::SwitchPort, port);
        r[Rule::MIRROR_PORT_REGION].push_back(std::move(port_link));
    }
}

void validate_mirror_interface(const json::Value& json, attribute::Attributes& attributes) {
    if (json.is_null()) {
        attributes.set_value(literals::AclRule::FORWARD_MIRROR_PORT, Json::Value::null);
        return;
    }
    json::Value schema({
        json::Pair(Common::ODATA_ID,
            JsonValidator::type_of(JsonValidator::STRING_TYPE, true))
    });

    const auto port_url = JsonValidator::validate_request_body(json, schema)[Common::ODATA_ID].as_string();
    attributes.set_value(literals::AclRule::FORWARD_MIRROR_PORT, endpoint::utils::get_port_uuid_from_url(port_url));
}

void validate_mirror_region(const json::Value& json, attribute::Attributes& attributes) {
    json::Value schema({
        json::Pair(Common::ODATA_ID,
            JsonValidator::type_of(JsonValidator::STRING_TYPE, true))
    });

    attribute::Array<std::string> port_array{};
    for (const auto& key : json) {
        const auto port_url = JsonValidator::validate_request_body(key, schema)[Common::ODATA_ID].as_string();
        port_array.add_entry(endpoint::utils::get_port_uuid_from_url(port_url));
    }
    attributes.set_value(literals::AclRule::MIRRORED_PORTS, port_array.to_json());
}

void validate_ip(const json::Value& json) {
    if(json.is_null()) {
        return;
    }
    json::Value schema;
    if (!json[Rule::MASK].is_null()) {
        schema = json::Value({
            json::Pair(Rule::IP_ADDRESS,
                JsonValidator::type_of(JsonValidator::STRING_TYPE, true)),
            json::Pair(Rule::MASK,
                JsonValidator::type_of(JsonValidator::STRING_TYPE, false))
        });
    }
    else {
        schema = json::Value({
            json::Pair(Rule::IP_ADDRESS,
                JsonValidator::type_of(JsonValidator::STRING_TYPE, true)),
        });
    }
    JsonValidator::validate_request_body(json, schema);
}

void validate_mac(const json::Value& json) {
    if(json.is_null()) {
        return;
    }
    json::Value schema;
    if (!json[Rule::MASK].is_null()) {
        schema = json::Value({
            json::Pair(Rule::MAC_ADDRESS,
                JsonValidator::type_of(JsonValidator::STRING_TYPE, true)),
            json::Pair(Rule::MASK,
                JsonValidator::type_of(JsonValidator::STRING_TYPE, false))
        });
    }
    else {
        schema = json::Value({
            json::Pair(Rule::MAC_ADDRESS,
                JsonValidator::type_of(JsonValidator::STRING_TYPE, true)),
        });
    }

    JsonValidator::validate_request_body(json, schema);
}

void validate_vlan_id(const json::Value& json) {
    if(json.is_null()) {
        return;
    }
    json::Value schema;
    if (!json[Rule::MASK].is_null()) {
        schema = json::Value({
            json::Pair(Common::ID,
                JsonValidator::type_of(JsonValidator::UINT_TYPE, true)),
            json::Pair(Rule::MASK,
                JsonValidator::type_of(JsonValidator::UINT_TYPE, false))
        });
    }
    else {
        schema = json::Value({
            json::Pair(Common::ID,
                JsonValidator::type_of(JsonValidator::UINT_TYPE, true)),
        });
    }

    JsonValidator::validate_request_body(json, schema);
}

void validate_port(const json::Value& json) {
    if(json.is_null()) {
        return;
    }
    json::Value schema;
    if (!json[Rule::MASK].is_null()) {
        schema = json::Value({
            json::Pair(Rule::PORT,
                JsonValidator::type_of(JsonValidator::UINT_TYPE, true)),
            json::Pair(Rule::MASK,
                JsonValidator::type_of(JsonValidator::UINT_TYPE, false))
        });
    }
    else {
        schema = json::Value({
            json::Pair(Rule::PORT,
                JsonValidator::type_of(JsonValidator::UINT_TYPE, true)),
        });
    }

    JsonValidator::validate_request_body(json, schema);
}

void validate_protocol(const json::Value& json) {
    json::Value schema({
        json::Pair(Rule::L4_PROTOCOL,
            JsonValidator::type_of(JsonValidator::UINT_TYPE, true))
    });

    JsonValidator::validate_request_body(json, schema);
}

void validate_condition(const json::Value& json, attribute::Attributes& attributes) {

    std::vector<std::string> valid_conditions {
        Rule::IP_SOURCE,
        Rule::IP_DESTINATION,
        Rule::MAC_SOURCE,
        Rule::MAC_DESTINATION,
        Rule::VLAN_ID,
        Rule::L4_SOURCE_PORT,
        Rule::L4_DESTINATION_PORT,
        Rule::L4_PROTOCOL
    };

    for (const auto& field : valid_conditions) {
        if (json.is_member(field)) {
            if (Rule::IP_SOURCE == field) {
                validate_ip(json[field]);
                attributes.set_value(literals::AclRule::SOURCE_IP,
                                     attribute::AclAddress(
                                     json[field][Rule::IP_ADDRESS],
                                     json[field][Rule::MASK]).to_json());
            }
            if (Rule::IP_DESTINATION == field) {
                validate_ip(json[field]);
                attributes.set_value(literals::AclRule::DESTINATION_IP,
                                     attribute::AclAddress(
                                     json[field][Rule::IP_ADDRESS],
                                     json[field][Rule::MASK]).to_json());
            }
            else if (Rule::MAC_SOURCE == field) {
                validate_mac(json[field]);
                attributes.set_value(literals::AclRule::SOURCE_MAC,
                                     attribute::AclAddress(
                                     json[field][Rule::MAC_ADDRESS],
                                     json[field][Rule::MASK]).to_json());
            }
            else if (Rule::MAC_DESTINATION == field) {
                validate_mac(json[field]);
                attributes.set_value(literals::AclRule::DESTINATION_MAC,
                                     attribute::AclAddress(
                                     json[field][Rule::MAC_ADDRESS],
                                     json[field][Rule::MASK]).to_json());
            }
            else if (Rule::L4_SOURCE_PORT == field) {
                validate_port(json[field]);
                attributes.set_value(literals::AclRule::SOURCE_L4_PORT,
                                     attribute::AclPort(
                                     json[field][Rule::PORT],
                                     json[field][Rule::MASK]).to_json());
            }
            else if (Rule::L4_DESTINATION_PORT == field) {
                validate_port(json[field]);
                attributes.set_value(literals::AclRule::DESTINATION_L4_PORT,
                                     attribute::AclPort(
                                     json[field][Rule::PORT],
                                     json[field][Rule::MASK]).to_json());
            }
            else if (Rule::VLAN_ID == field) {
                validate_vlan_id(json[field]);
                attributes.set_value(literals::AclRule::VLAN_ID,
                                     attribute::AclVlanId(
                                     json[field][Common::ID],
                                     json[field][Rule::MASK]).to_json());
            }
            else if (Rule::L4_PROTOCOL == field) {
                if(json[field].is_null()) {
                    attributes.set_value(literals::AclRule::PROTOCOL, Json::Value::null);
                }
                else {
                    validate_protocol(json);
                    attributes.set_value(literals::AclRule::PROTOCOL, json[field].as_uint());
                }
            }
        }
    }
}

attribute::Attributes validate_patch_request(const server::Request& request) {

    json::Value acl_actions{json::Value::Type::ARRAY};
    for (const auto& value : agent_framework::model::enums::AclAction::get_values()) {
        acl_actions.push_back(value);
    }

    json::Value mirror_types{json::Value::Type::ARRAY};
    for (const auto& value : agent_framework::model::enums::AclMirrorType::get_values()) {
        mirror_types.push_back(value);
    }

    json::Value schema({
        json::Pair(Rule::RULE_ID,
            JsonValidator::type_of(JsonValidator::UINT_TYPE, false)),
        json::Pair(Rule::ACTION,
            JsonValidator::any_of(JsonValidator::STRING_TYPE, acl_actions, false)),
        json::Pair(Rule::FORWARD_MIRROR_INTERFACE,
            JsonValidator::type_of(JsonValidator::OBJECT_TYPE, false)),
        json::Pair(Rule::MIRROR_PORT_REGION,
            JsonValidator::type_of(JsonValidator::ARRAY_TYPE, false)),
        json::Pair(Rule::MIRROR_TYPE,
            JsonValidator::any_of(JsonValidator::STRING_TYPE, mirror_types, false)),
        json::Pair(Rule::CONDITION,
            JsonValidator::type_of(JsonValidator::OBJECT_TYPE, false))
    });

    const auto parsed_json = JsonValidator::validate_request_body(request, schema);
    attribute::Attributes attributes;

    if (parsed_json.is_member(Rule::RULE_ID)) {
        if (parsed_json[Rule::RULE_ID].is_null()) {
            attributes.set_value(literals::AclRule::RULE_ID, Json::Value::null);
        }
        else {
            attributes.set_value(literals::AclRule::RULE_ID, parsed_json[Rule::RULE_ID].as_uint());
        }
    }

    if (parsed_json.is_member(Rule::ACTION)) {
        if (parsed_json[Rule::ACTION].is_null()) {
            ServerError error = ErrorFactory::create_invalid_payload_error();
            error.add_extended_message("Action cannot be set to null");
            throw ServerException(error);
        }
        attributes.set_value(literals::AclRule::ACTION, parsed_json[Rule::ACTION].as_string());
    }

    if (parsed_json.is_member(Rule::MIRROR_TYPE)) {
        if (parsed_json[Rule::MIRROR_TYPE].is_null()) {
            attributes.set_value(literals::AclRule::MIRROR_TYPE, Json::Value::null);
        }
        else {
            attributes.set_value(literals::AclRule::MIRROR_TYPE, parsed_json[Rule::MIRROR_TYPE].as_string());
        }
    }

    if (parsed_json.is_member(Rule::FORWARD_MIRROR_INTERFACE)) {
         validate_mirror_interface(parsed_json[Rule::FORWARD_MIRROR_INTERFACE], attributes);
    }

    if (parsed_json.is_member(Rule::MIRROR_PORT_REGION)) {
         validate_mirror_region(parsed_json[Rule::MIRROR_PORT_REGION], attributes);
    }

    if (parsed_json.is_member(Rule::CONDITION)) {
        if (parsed_json[Rule::CONDITION].is_null()) {
            ServerError error = ErrorFactory::create_invalid_payload_error();
            error.add_extended_message("Condition cannot be set to null");
            throw ServerException(error);
        }
        validate_condition(parsed_json[Rule::CONDITION], attributes);
    }

    return attributes;
}

}

endpoint::Rule::Rule(const std::string& path) : Endpoint(path) {}
endpoint::Rule::~Rule() {}

void endpoint::Rule::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();

    r[Common::ODATA_CONTEXT] = std::regex_replace(
        r[Common::ODATA_CONTEXT].as_string(),
        std::regex("__SWITCH_ID__"),
        req.params[PathParam::ETHERNET_SWITCH_ID]);

    r[Common::ODATA_CONTEXT] = std::regex_replace(
        r[Common::ODATA_CONTEXT].as_string(),
        std::regex("__ACL_ID__"),
        req.params[PathParam::ACL_ID]);

    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ID] = req.params[PathParam::RULE_ID];
    r[Common::NAME] = constants::Rule::RULE + req.params[PathParam::RULE_ID];

    const auto rule = psme::rest::model::Find<agent_framework::model::AclRule>(
            req.params[PathParam::RULE_ID])
            .via<agent_framework::model::Switch>(
            req.params[PathParam::ETHERNET_SWITCH_ID])
            .via<agent_framework::model::Acl>(
            req.params[PathParam::ACL_ID])
            .get();

    r[constants::Rule::RULE_ID] = rule.get_rule_id();
    r[constants::Rule::ACTION] = rule.get_action();
    if (rule.get_forward_mirror_port().has_value()) {
        r[constants::Rule::FORWARD_MIRROR_INTERFACE][Common::ODATA_ID] =
            endpoint::utils::get_component_url(
                enums::Component::SwitchPort, rule.get_forward_mirror_port());
    }
    r[constants::Rule::MIRROR_TYPE] = rule.get_mirror_type();

    ::set_region(rule, r);
    ::set_condition(rule, r);

    set_response(res, r);
}

void endpoint::Rule::del(const server::Request& req, server::Response& res) {
    auto rule = psme::rest::model::Find<agent_framework::model::AclRule>(
            req.params[PathParam::RULE_ID])
            .via<agent_framework::model::Switch>(
            req.params[PathParam::ETHERNET_SWITCH_ID])
            .via<agent_framework::model::Acl>(
            req.params[PathParam::ACL_ID])
            .get_one();

    auto delete_acl_rule_request = requests::DeleteAclRule(rule->get_uuid());

    // try removing ACL from agent's model
    AgentManager::get_instance()->call_method<responses::DeleteAclRule>(rule->get_agent_id(), delete_acl_rule_request);

    // remove the resource from application's model
    HandlerManager::get_instance()->get_handler(enums::Component::AclRule)->remove(rule->get_uuid());

    res.set_status(server::status_2XX::NO_CONTENT);
}

void endpoint::Rule::patch(const server::Request& req, server::Response& res) {
    const auto rule = psme::rest::model::Find<agent_framework::model::AclRule>(
            req.params[PathParam::RULE_ID])
            .via<agent_framework::model::Switch>(
            req.params[PathParam::ETHERNET_SWITCH_ID])
            .via<agent_framework::model::Acl>(
            req.params[PathParam::ACL_ID])
            .get();

    // validate and fill in Attributes at the same time
    const auto attributes = validate_patch_request(req);

    if (!attributes.to_json().isNull()) {
        requests::SetComponentAttributes set_component_attributes_request{rule.get_uuid(), attributes};
        AgentManager::get_instance()->call_method<responses::SetComponentAttributes>(rule.get_agent_id(),
            set_component_attributes_request);
        HandlerManager::get_instance()->get_handler(enums::Component::AclRule)->
            load(AgentManager::get_instance()->get_agent(
            rule.get_agent_id()), rule.get_parent_uuid(), rule.get_uuid(), true);
    }

    res.set_status(server::status_2XX::NO_CONTENT);
}
