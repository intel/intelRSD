
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

#include "psme/rest-ref/endpoints/rule_collection.hpp"
#include "psme/rest-ref/utils/json_validator.hpp"
#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/rest-ref/model/handlers/handler_manager.hpp"
#include "psme/rest-ref/model/handlers/generic_handler_deps.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"
#include "psme/core/agent/agent_manager.hpp"

#include "agent-framework/module-ref/requests/network.hpp"
#include "agent-framework/module-ref/responses/network.hpp"

using namespace psme::rest;
using namespace psme::rest::utils;
using namespace psme::rest::error;
using namespace psme::core::agent;
using namespace app::rest::constants;
using namespace agent_framework::model;
using NetworkManager = agent_framework::module::NetworkManager;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitches/"
        "Members/__SWITCH_ID__/ACLs/Members/__ACL_ID__/Rules";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] =
        "#EthernetSwitchACLRuleCollection.EthernetSwitchACLRuleCollection";
    r[Common::NAME] = "Ethernet Switch Access Control List Rules Collection";
    r[Common::DESCRIPTION] = "Rules for switch Access Control List. Each rule "
        "defines single action and at least one condition";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}

void validate_mirror_interface(const json::Value& json, AclRule& rule) {
    json::Value schema({
        json::Pair(Common::ODATA_ID,
            JsonValidator::type_of(JsonValidator::STRING_TYPE, true))
    });

    const auto port_url = JsonValidator::validate_request_body(json, schema)[Common::ODATA_ID].as_string();
    rule.set_forward_mirror_port(endpoint::utils::get_port_uuid_from_url(port_url));
}

void validate_mirror_region(const json::Value& json, AclRule& rule) {
    json::Value schema({
        json::Pair(Common::ODATA_ID,
            JsonValidator::type_of(JsonValidator::STRING_TYPE, true))
    });

    for (const auto& key : json) {
        const auto port_url = JsonValidator::validate_request_body(key, schema)[Common::ODATA_ID].as_string();
        rule.add_mirrored_port(endpoint::utils::get_port_uuid_from_url(port_url));
    }
}

void validate_ip(const json::Value& json) {
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

void validate_condition(const json::Value& json, AclRule& rule) {
    bool any_condition_found = false;
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
    //
    for (const auto& field : valid_conditions) {
        if (json.is_member(field)) {
            // null is acceptable, but at least one of the fields must not be null
            if (json[field].is_null()) {
                continue;
            }
            any_condition_found = true;

            if (Rule::IP_SOURCE == field) {
                validate_ip(json[field]);
                rule.set_source_ip(attribute::AclAddress(
                                    json[field][Rule::IP_ADDRESS],
                                    json[field][Rule::MASK]));
            }
            else if (Rule::IP_DESTINATION == field) {
                validate_ip(json[field]);
                rule.set_destination_ip(attribute::AclAddress(
                                    json[field][Rule::IP_ADDRESS],
                                    json[field][Rule::MASK]));
            }
            else if (Rule::MAC_SOURCE == field) {
                validate_mac(json[field]);
                rule.set_source_mac(attribute::AclAddress(
                                    json[field][Rule::MAC_ADDRESS],
                                    json[field][Rule::MASK]));
            }
            else if (Rule::MAC_DESTINATION == field) {
                validate_mac(json[field]);
                rule.set_destination_mac(attribute::AclAddress(
                                    json[field][Rule::MAC_ADDRESS],
                                    json[field][Rule::MASK]));
            }
            else if (Rule::L4_SOURCE_PORT == field) {
                validate_port(json[field]);
                rule.set_source_port(attribute::AclPort(
                                    json[field][Rule::PORT],
                                    json[field][Rule::MASK]));
            }
            else if (Rule::L4_DESTINATION_PORT == field) {
                validate_port(json[field]);
                rule.set_destination_port(attribute::AclPort(
                                    json[field][Rule::PORT],
                                    json[field][Rule::MASK]));
            }
            else if (Rule::VLAN_ID == field) {
                validate_vlan_id(json[field]);
                rule.set_vlan_id(attribute::AclVlanId(
                                    json[field][Common::ID],
                                    json[field][Rule::MASK]));
            }
            else if (Rule::L4_PROTOCOL == field) {
                validate_protocol(json);
                rule.set_protocol(json[field]);
            }
        }
    }

    if (!any_condition_found) {
        ServerError error = ErrorFactory::create_invalid_payload_error();
        std::string valid_conditions_message{};
        for (const auto& field : valid_conditions) {
            valid_conditions_message += field;
            // do not append a comma for the last element
            if (field != valid_conditions[valid_conditions.size() - 1]) {
                valid_conditions_message += ", ";
            }
        }
        error.add_extended_message("Condition must have at least one of these fields present and not nulled: "
            + valid_conditions_message);
        throw ServerException(error);
    }
}

json::Value validate_post_request(const server::Request& request, AclRule& rule) {

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
            JsonValidator::any_of(JsonValidator::STRING_TYPE, acl_actions, true)),
        json::Pair(Rule::FORWARD_MIRROR_INTERFACE,
            JsonValidator::type_of(JsonValidator::OBJECT_TYPE, false)),
        json::Pair(Rule::MIRROR_PORT_REGION,
                JsonValidator::type_of(JsonValidator::ARRAY_TYPE, false)),
        json::Pair(Rule::MIRROR_TYPE,
            JsonValidator::any_of(JsonValidator::STRING_TYPE, mirror_types, false)),
        json::Pair(Rule::CONDITION,
            JsonValidator::type_of(JsonValidator::OBJECT_TYPE, true))
    });

    const auto parsed_json = JsonValidator::validate_request_body(request, schema);

    rule.set_rule_id(parsed_json[Rule::RULE_ID]);
    rule.set_action(parsed_json[Rule::ACTION]);
    rule.set_mirror_type(parsed_json[Rule::MIRROR_TYPE]);

    if (parsed_json.is_member(Rule::FORWARD_MIRROR_INTERFACE) && !parsed_json[Rule::FORWARD_MIRROR_INTERFACE].is_null()) {
        validate_mirror_interface(parsed_json[Rule::FORWARD_MIRROR_INTERFACE], rule);
    }

    if (parsed_json.is_member(Rule::MIRROR_PORT_REGION) && !parsed_json[Rule::MIRROR_PORT_REGION].is_null()) {
        validate_mirror_region(parsed_json[Rule::MIRROR_PORT_REGION], rule);
    }

    validate_condition(parsed_json[Rule::CONDITION], rule);

    return parsed_json;
}

}

endpoint::RuleCollection::RuleCollection(const std::string& path) : Endpoint(path) {}
endpoint::RuleCollection::~RuleCollection() {}


void endpoint::RuleCollection::get(const server::Request& req, server::Response& res) {
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

    const auto acl_uuid = NetworkManager::get_instance()->
        get_acl_manager().rest_id_to_uuid(
        endpoint::utils::id_to_uint64(req.params[PathParam::ACL_ID]),
        NetworkManager::get_instance()->
        get_switch_manager().rest_id_to_uuid(
        endpoint::utils::id_to_uint64(req.params[PathParam::ETHERNET_SWITCH_ID])));

    const auto keys = NetworkManager::get_instance()->
        get_acl_rule_manager().get_ids(acl_uuid);

    r[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(keys.size());

    for (const auto& key : keys) {
        json::Value link_elem(json::Value::Type::OBJECT);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(key).build();
        r[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(res, r);
}


void endpoint::RuleCollection::post(const server::Request& req,
    server::Response& res) {

    auto parent_acl = model::Find<agent_framework::model::Acl>
         (req.params[PathParam::ACL_ID])
         .via<agent_framework::model::Switch>
         (req.params[PathParam::ETHERNET_SWITCH_ID]).get_one();

    AclRule rule{parent_acl->get_uuid()};

    // validate and fill in AclRule at the same time
    const auto json = validate_post_request(req, rule);

    const requests::AddAclRule add_acl_rule_request{
        parent_acl->get_uuid(),
        rule.get_rule_id(),
        rule.get_action(),
        rule.get_forward_mirror_port(),
        rule.get_mirrored_ports(),
        rule.get_mirror_type(),
        rule.get_vlan_id(),
        rule.get_source_ip(),
        rule.get_destination_ip(),
        rule.get_source_mac(),
        rule.get_destination_mac(),
        rule.get_source_port(),
        rule.get_destination_port(),
        rule.get_protocol(),
        attribute::Oem()
    };

    const auto add_acl_rule_response = AgentManager::get_instance()
        ->call_method<responses::AddAclRule>(parent_acl->get_agent_id(), add_acl_rule_request);

    const auto rule_id = model::handler::HandlerManager::get_instance()->
            get_handler(enums::Component::AclRule)->
            load(AgentManager::get_instance()->get_agent(parent_acl->get_agent_id()),
            parent_acl->get_uuid(), add_acl_rule_response.get_rule(), true);

    endpoint::utils::set_location_header(res, req, PathBuilder(req).append(rule_id).build());

    res.set_status(server::status_2XX::CREATED);
}
