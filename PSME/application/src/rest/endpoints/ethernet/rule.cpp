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

#include "psme/rest/endpoints/ethernet/rule.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/rule.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "psme/core/agent/agent_manager.hpp"

#include "agent-framework/module/requests/network.hpp"
#include "agent-framework/module/responses/network.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/responses/common.hpp"



using namespace psme::rest;
using namespace psme::rest::validators;
using namespace psme::rest::error;
using namespace psme::rest::constants;
using namespace psme::rest::model::handler;
using namespace psme::core::agent;
using namespace agent_framework::module;
using namespace agent_framework::model;
using NetworkComponents = agent_framework::module::NetworkComponents;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitches/"
        "Members/__SWITCH_ID__/ACLs/Members/__ACL_ID__/Rules/Members/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EthernetSwitchACLRule.v1_0_0.EthernetSwitchACLRule";

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
        r[Rule::CONDITION][constants::Rule::MAC_SOURCE][constants::Common::MAC_ADDRESS] =
            rule.get_source_mac().get_address();
        r[Rule::CONDITION][constants::Rule::MAC_SOURCE][constants::Rule::MASK] =
            rule.get_source_mac().get_mask();
    }
    if (rule.get_destination_mac().get_address().has_value()) {
        r[Rule::CONDITION][constants::Rule::MAC_DESTINATION][constants::Common::MAC_ADDRESS] =
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
        port_link[Common::ODATA_ID] = endpoint::utils::get_component_url(enums::Component::EthernetSwitchPort, port);
        r[Rule::MIRROR_PORT_REGION].push_back(std::move(port_link));
    }
}


void fill_condition(const json::Value& json, attribute::Attributes& attributes) {

    std::vector<std::string> valid_conditions{
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
                attributes.set_value(literals::AclRule::SOURCE_IP, attribute::AclAddress(
                    json[field][Rule::IP_ADDRESS],
                    json[field][Rule::MASK]).to_json());
            }
            if (Rule::IP_DESTINATION == field) {
                attributes.set_value(literals::AclRule::DESTINATION_IP, attribute::AclAddress(
                    json[field][Rule::IP_ADDRESS],
                    json[field][Rule::MASK]).to_json());
            }
            else if (Rule::MAC_SOURCE == field) {
                attributes.set_value(literals::AclRule::SOURCE_MAC, attribute::AclAddress(
                    json[field][Common::MAC_ADDRESS],
                    json[field][Rule::MASK]).to_json());
            }
            else if (Rule::MAC_DESTINATION == field) {
                attributes.set_value(literals::AclRule::DESTINATION_MAC, attribute::AclAddress(
                    json[field][Common::MAC_ADDRESS],
                    json[field][Rule::MASK]).to_json());
            }
            else if (Rule::L4_SOURCE_PORT == field) {
                attributes.set_value(literals::AclRule::SOURCE_L4_PORT, attribute::AclPort(
                    json[field][Rule::PORT],
                    json[field][Rule::MASK]).to_json());
            }
            else if (Rule::L4_DESTINATION_PORT == field) {
                attributes.set_value(literals::AclRule::DESTINATION_L4_PORT, attribute::AclPort(
                    json[field][Rule::PORT],
                    json[field][Rule::MASK]).to_json());
            }
            else if (Rule::VLAN_ID == field) {
                attributes.set_value(literals::AclRule::VLAN_ID, attribute::AclVlanId(
                    json[field][Common::ID],
                    json[field][Rule::MASK]).to_json());
            }
            else if (Rule::L4_PROTOCOL == field) {
                attributes.set_value(literals::AclRule::PROTOCOL, OptionalField<std::uint32_t>(json[field]));
            }
        }
    }
}


attribute::Attributes fill_attributes(const json::Value& parsed_json, const AclRule& rule) {
    using AclAction = agent_framework::model::enums::AclAction;

    attribute::Attributes attributes;
    auto action = rule.get_action();

    if (parsed_json.is_member(Rule::ACTION)) {
        action = OptionalField<AclAction>(parsed_json[Rule::ACTION]);
        attributes.set_value(literals::AclRule::ACTION, parsed_json[Rule::ACTION].as_string());
    }
    if (parsed_json.is_member(Rule::RULE_ID)) {
        // Workaround for GAMI - REST spec mismatch:
        THROW(agent_framework::exceptions::NotImplemented, "rest",
              "Setting 'RuleId' is not implemented in the network agent.");
    }
    if (parsed_json.is_member(Rule::MIRROR_TYPE)) {
        attributes.set_value(literals::AclRule::MIRROR_TYPE, parsed_json[Rule::MIRROR_TYPE].as_string());
    }
    if (parsed_json.is_member(Rule::FORWARD_MIRROR_INTERFACE)) {
        const auto port_url = parsed_json[Rule::FORWARD_MIRROR_INTERFACE][Common::ODATA_ID].as_string();
        attributes.set_value(literals::AclRule::FORWARD_MIRROR_PORT, endpoint::utils::get_port_uuid_from_url(port_url));
    }
    if (parsed_json.is_member(Rule::MIRROR_PORT_REGION)) {
        attribute::Array<std::string> port_array{};
        for (const auto& port : parsed_json[Rule::MIRROR_PORT_REGION].as_array()) {
            const auto port_url = port[Common::ODATA_ID].as_string();
            port_array.add_entry(endpoint::utils::get_port_uuid_from_url(port_url));
        }
        attributes.set_value(literals::AclRule::MIRRORED_PORTS, port_array.to_json());
    }
    if (parsed_json.is_member(Rule::CONDITION)) {
        fill_condition(parsed_json[Rule::CONDITION], attributes);
    }

    if (AclAction::Mirror == action || AclAction::Forward == action) {
        // If ForwardMirrorInterface is (not present in json or model) or is (present in json and set to null)
        if ((!parsed_json.is_member(Rule::FORWARD_MIRROR_INTERFACE) &&
             !rule.get_forward_mirror_port().has_value()) ||
            (parsed_json.is_member(Rule::FORWARD_MIRROR_INTERFACE) &&
             parsed_json[Rule::FORWARD_MIRROR_INTERFACE].is_null())) {
            throw ServerException(ErrorFactory::create_property_missing_error(
                constants::Rule::FORWARD_MIRROR_INTERFACE,
                "Mandatory field is missing for " + std::string(action.value().to_string()) + " action."
            ));
        }
    }
    if (agent_framework::model::enums::AclAction::Mirror == action) {
        // If MirrorType is (not present in json or model) or is (present in json and set to null)
        if ((!parsed_json.is_member(Rule::MIRROR_TYPE) &&
             !rule.get_forward_mirror_port().has_value()) ||
            (parsed_json.is_member(Rule::MIRROR_TYPE) &&
             parsed_json[Rule::MIRROR_TYPE].is_null())) {
            throw ServerException(ErrorFactory::create_property_missing_error(
                constants::Rule::MIRROR_TYPE,
                "Mandatory field is missing for " + std::string(action.value().to_string()) + " action."
            ));
        }
    }

    return attributes;
}


static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::AclRule::ACTION, constants::Rule::ACTION},
    {agent_framework::model::literals::AclRule::FORWARD_MIRROR_PORT, constants::Rule::FORWARD_MIRROR_INTERFACE},
    {agent_framework::model::literals::AclRule::MIRRORED_PORTS, constants::Rule::MIRROR_PORT_REGION},
    {agent_framework::model::literals::AclRule::MIRROR_TYPE, constants::Rule::MIRROR_TYPE},
    {agent_framework::model::literals::AclRule::SOURCE_IP, endpoint::PathBuilder(constants::Rule::CONDITION)
                                                               .append(constants::Rule::IP_SOURCE).build()},
    {agent_framework::model::literals::AclRule::DESTINATION_IP, endpoint::PathBuilder(constants::Rule::CONDITION)
                                                               .append(constants::Rule::IP_DESTINATION).build()},
    {agent_framework::model::literals::AclRule::SOURCE_MAC, endpoint::PathBuilder(constants::Rule::CONDITION)
                                                               .append(constants::Rule::MAC_SOURCE).build()},
    {agent_framework::model::literals::AclRule::DESTINATION_MAC, endpoint::PathBuilder(constants::Rule::CONDITION)
                                                               .append(constants::Rule::MAC_DESTINATION).build()},
    {agent_framework::model::literals::AclRule::VLAN_ID, endpoint::PathBuilder(constants::Rule::CONDITION)
                                                               .append(constants::Rule::VLAN_ID).build()},
    {agent_framework::model::literals::AclRule::SOURCE_L4_PORT, endpoint::PathBuilder(constants::Rule::CONDITION)
                                                               .append(constants::Rule::L4_SOURCE_PORT).build()},
    {agent_framework::model::literals::AclRule::DESTINATION_L4_PORT, endpoint::PathBuilder(constants::Rule::CONDITION)
                                                               .append(constants::Rule::L4_DESTINATION_PORT).build()},
    {agent_framework::model::literals::AclRule::PROTOCOL, endpoint::PathBuilder(constants::Rule::CONDITION)
                                                               .append(constants::Rule::L4_PROTOCOL).build()},
};
}


endpoint::Rule::Rule(const std::string& path) : EndpointBase(path) {}


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

    const auto rule =
        psme::rest::model::Find<agent_framework::model::AclRule>(req.params[PathParam::RULE_ID])
            .via<agent_framework::model::EthernetSwitch>(req.params[PathParam::ETHERNET_SWITCH_ID])
            .via<agent_framework::model::Acl>(req.params[PathParam::ACL_ID])
            .get();

    r[constants::Rule::RULE_ID] = rule.get_rule_id();
    r[constants::Rule::ACTION] = rule.get_action();
    if (rule.get_forward_mirror_port().has_value()) {
        r[constants::Rule::FORWARD_MIRROR_INTERFACE][Common::ODATA_ID] =
            endpoint::utils::get_component_url(enums::Component::EthernetSwitchPort, rule.get_forward_mirror_port());
    }
    r[constants::Rule::MIRROR_TYPE] = rule.get_mirror_type();

    ::set_region(rule, r);
    ::set_condition(rule, r);

    set_response(res, r);
}


void endpoint::Rule::del(const server::Request& req, server::Response& res) {
    auto rule = psme::rest::model::Find<agent_framework::model::AclRule>(req.params[PathParam::RULE_ID])
        .via<agent_framework::model::EthernetSwitch>(req.params[PathParam::ETHERNET_SWITCH_ID])
        .via<agent_framework::model::Acl>(req.params[PathParam::ACL_ID])
        .get();

    auto delete_acl_rule_request = requests::DeleteAclRule(rule.get_uuid());

    const auto& agent = psme::core::agent::AgentManager::get_instance()->get_agent(rule.get_agent_id());

    auto delete_rule = [&, agent] {
        // try removing ACL from agent's model
        agent->execute<responses::DeleteAclRule>(delete_acl_rule_request);

        // remove the resource from application's model
        HandlerManager::get_instance()->get_handler(enums::Component::AclRule)->remove(rule.get_uuid());

        res.set_status(server::status_2XX::NO_CONTENT);
    };
    agent->execute_in_transaction(delete_rule);
}


void endpoint::Rule::patch(const server::Request& request, server::Response& response) {
    const auto rule =
        psme::rest::model::Find<agent_framework::model::AclRule>(request.params[PathParam::RULE_ID])
            .via<agent_framework::model::EthernetSwitch>(request.params[PathParam::ETHERNET_SWITCH_ID])
            .via<agent_framework::model::Acl>(request.params[PathParam::ACL_ID])
            .get();

    const auto json = JsonValidator::validate_request_body<schema::RulePatchSchema>(request);
    const auto attributes = fill_attributes(json, rule);

    if (!attributes.empty()) {
        requests::SetComponentAttributes set_component_attributes_request{rule.get_uuid(), attributes};

        const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(rule.get_agent_id());

        auto set_rule_attributes = [&, gami_agent] {
            auto set_component_attributes_response = gami_agent
                ->execute<responses::SetComponentAttributes>(set_component_attributes_request);

            const auto& result_statuses = set_component_attributes_response.get_statuses();
            if (!result_statuses.empty()) {
                const auto& error = ErrorFactory::create_error_from_set_component_attributes_results(
                    result_statuses, gami_to_rest_attributes);
                throw ServerException(error);
            }

            HandlerManager::get_instance()->get_handler(enums::Component::AclRule)->
                load(gami_agent, rule.get_parent_uuid(), enums::Component::Acl, rule.get_uuid(), true);
        };
        gami_agent->execute_in_transaction(set_rule_attributes);
    }

    get(request, response);
}
