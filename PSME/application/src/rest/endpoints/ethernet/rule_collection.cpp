
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

#include <psme/rest/validators/schemas/acl.hpp>
#include "psme/rest/endpoints/ethernet/rule_collection.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/rule_collection.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"

#include "agent-framework/module/requests/network.hpp"
#include "agent-framework/module/responses/network.hpp"



using namespace psme::rest;
using namespace psme::rest::validators;
using namespace psme::rest::error;
using namespace psme::core::agent;
using namespace psme::rest::constants;
using namespace agent_framework::model;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitchACLRuleCollection.EthernetSwitchACLRuleCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#EthernetSwitchACLRuleCollection.EthernetSwitchACLRuleCollection";
    r[Common::NAME] = "Ethernet Switch Access Control List Rules Collection";
    r[Common::DESCRIPTION] = "Rules for switch Access Control List."
                             " Each rule defines single action and at least one condition.";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}


void get_rule_condition(const json::Json& json, AclRule& rule) {
    bool any_condition_found = false;
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
        if (json.count(field)) {
            // Value null is acceptable, but at least one of the fields must not be null
            if (json[field].is_null()) {
                continue;
            }
            any_condition_found = true;

            if (Rule::IP_SOURCE == field) {
                rule.set_source_ip(attribute::AclAddress(
                    json[field][Rule::IP_ADDRESS], json[field][Rule::MASK]));
            }
            else if (Rule::IP_DESTINATION == field) {
                rule.set_destination_ip(attribute::AclAddress(
                    json[field][Rule::IP_ADDRESS], json[field][Rule::MASK]));
            }
            else if (Rule::MAC_SOURCE == field) {
                rule.set_source_mac(attribute::AclAddress(
                    json[field][Common::MAC_ADDRESS], json[field][Rule::MASK]));
            }
            else if (Rule::MAC_DESTINATION == field) {
                rule.set_destination_mac(attribute::AclAddress(
                    json[field][Common::MAC_ADDRESS], json[field][Rule::MASK]));
            }
            else if (Rule::L4_SOURCE_PORT == field) {
                rule.set_source_port(attribute::AclPort(
                    json[field][Rule::PORT], json[field][Rule::MASK]));
            }
            else if (Rule::L4_DESTINATION_PORT == field) {
                rule.set_destination_port(attribute::AclPort(
                    json[field][Rule::PORT], json[field][Rule::MASK]));
            }
            else if (Rule::VLAN_ID == field) {
                rule.set_vlan_id(attribute::AclVlanId(
                    json[field][Common::ID], json[field][Rule::MASK]));
            }
            else if (Rule::L4_PROTOCOL == field) {
                rule.set_protocol(json[field]);
            }
        }
    }

    if (!any_condition_found) {
        ServerError error = ErrorFactory::create_property_missing_error(Rule::CONDITION);
        std::string valid_conditions_message{};
        for (const auto& field : valid_conditions) {
            valid_conditions_message += field;
            // Do not append a comma for the last element
            if (field != valid_conditions[valid_conditions.size() - 1]) {
                valid_conditions_message += ", ";
            }
        }
        error.add_extended_message("Condition must have at least one of these fields present and not nulled: "
                                   + valid_conditions_message);
        throw ServerException(error);
    }
}


AclRule get_rule_model_from_post(const json::Json& posted_json) {
    AclRule rule{};

    rule.set_action(posted_json[Rule::ACTION]);
    rule.set_rule_id(posted_json[Rule::RULE_ID]);
    rule.set_mirror_type(posted_json[Rule::MIRROR_TYPE]);

    if (posted_json.count(Rule::FORWARD_MIRROR_INTERFACE)
        && !posted_json[Rule::FORWARD_MIRROR_INTERFACE].is_null()) {

        const auto& port_url = posted_json[Rule::FORWARD_MIRROR_INTERFACE][Common::ODATA_ID].get<std::string>();
        rule.set_forward_mirror_port(endpoint::utils::get_port_uuid_from_url(port_url));
    }
    else if (enums::AclAction::Mirror == rule.get_action() || enums::AclAction::Forward == rule.get_action()) {
        throw ServerException(ErrorFactory::create_property_missing_error(
            Rule::FORWARD_MIRROR_INTERFACE,
            "Property is required for " + std::string(rule.get_action().value().to_string()) + " action.",
            {Rule::FORWARD_MIRROR_INTERFACE}
        ));
    }

    if (posted_json.count(Rule::MIRROR_PORT_REGION) &&
        !posted_json.value(Rule::MIRROR_PORT_REGION, json::Json()).is_null()) {
        for (const auto& port : posted_json[Rule::MIRROR_PORT_REGION]) {
            const auto& port_url = port[Common::ODATA_ID].get<std::string>();
            rule.add_mirrored_port(endpoint::utils::get_port_uuid_from_url(port_url));
        }
    }
    else if (enums::AclAction::Mirror == rule.get_action()) {
        throw ServerException(ErrorFactory::create_property_missing_error(
            Rule::MIRROR_PORT_REGION,
            "Property is required for " + std::string(rule.get_action().value().to_string()) + " action.",
            {Rule::MIRROR_PORT_REGION}
        ));
    }

    get_rule_condition(posted_json[Rule::CONDITION], rule);
    return rule;
}

}


endpoint::RuleCollection::RuleCollection(const std::string& path) : EndpointBase(path) {}


endpoint::RuleCollection::~RuleCollection() {}


void endpoint::RuleCollection::get(const server::Request& request, server::Response& response) {
    auto r = ::make_prototype();

    r[Common::ODATA_ID] = PathBuilder(request).build();

    auto acl_uuid =
        psme::rest::model::find<agent_framework::model::EthernetSwitch, agent_framework::model::Acl>(
            request.params).get_uuid();

    const auto ids = agent_framework::module::get_manager<agent_framework::model::AclRule>().get_ids(acl_uuid);

    r[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(ids.size());

    for (const auto& id : ids) {
        json::Json link = json::Json();
        link[Common::ODATA_ID] = PathBuilder(request).append(id).build();
        r[Collection::MEMBERS].push_back(std::move(link));
    }

    set_response(response, r);
}


void endpoint::RuleCollection::post(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PostRuleCollection";

    auto parent_acl = model::find<agent_framework::model::EthernetSwitch, agent_framework::model::Acl>(
        request.params).get();

    const auto& json = JsonValidator::validate_request_body<schema::RulePostSchema>(request);
    const auto rule = get_rule_model_from_post(json);

    const requests::AddAclRule add_acl_rule_request{
        parent_acl.get_uuid(),
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

    auto gami_agent = AgentManager::get_instance()->get_agent(parent_acl.get_agent_id());

    auto add_rule = [&, gami_agent] {
        const auto add_acl_rule_response = gami_agent->
            execute<responses::AddAclRule>(add_acl_rule_request);

        const auto rule_id = model::handler::HandlerManager::get_instance()->
            get_handler(enums::Component::AclRule)->
            load(gami_agent, parent_acl.get_uuid(), enums::Component::Acl, add_acl_rule_response.get_rule(), true);

        endpoint::utils::set_location_header(request, response, PathBuilder(request).append(rule_id).build());
        response.set_status(server::status_2XX::CREATED);
    };
    gami_agent->execute_in_transaction(TRANSACTION_NAME, add_rule);
}
