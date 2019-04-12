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

#include "agent-framework/module/constants/pnc.hpp"
#include "agent-framework/module/requests/common/set_component_attributes.hpp"
#include "agent-framework/module/responses/common/set_component_attributes.hpp"

#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/switch.hpp"

#include "psme/rest/endpoints/fabric/switch.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "psme/rest/server/error/error_factory.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

namespace {

json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Switch.Switch";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Switch.v1_0_0.Switch";
    r[Common::NAME] = "PCIe Switch";
    r[Common::DESCRIPTION] = "PCIe Switch description";
    r[Common::ID] = json::Json::value_t::null;

    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;

    r[Common::MANUFACTURER] = json::Json::value_t::null;
    r[Common::MODEL] = json::Json::value_t::null;
    r[Common::SKU] = json::Json::value_t::null;
    r[Common::SERIAL_NUMBER] = json::Json::value_t::null;
    r[Common::PART_NUMBER] = json::Json::value_t::null;
    r[Common::ASSET_TAG] = json::Json::value_t::null;
    r[constants::Switch::SWITCH_TYPE] = json::Json::value_t::null;
    r[constants::Switch::DOMAIN_ID] = json::Json::value_t::null;
    r[constants::Switch::IS_MANAGED] = json::Json::value_t::null;
    r[constants::Switch::TOTAL_SWITCH_WIDTH] = json::Json::value_t::null;
    r[constants::Switch::INDICATOR_LED] = json::Json::value_t::null;
    r[constants::Switch::POWER_STATE] = json::Json::value_t::null;
    r[constants::Switch::PORTS] = json::Json::value_t::null;
    r[constants::Common::REDUNDANCY] = json::Json::value_t::array;

    r[Common::LINKS][Common::CHASSIS] = json::Json::value_t::array;
    r[Common::LINKS][Common::MANAGED_BY] = json::Json::value_t::array;
    r[Common::LINKS][Common::OEM] = json::Json::value_t::object;

    json::Json actions = json::Json();
    actions[constants::Switch::SWITCH_RESET][Common::TARGET] = json::Json::value_t::null;
    actions[constants::Switch::SWITCH_RESET][Common::ALLOWABLE_RESET_TYPES] = json::Json::value_t::array;
    actions[Common::OEM] = json::Json::value_t::object;
    r[Common::ACTIONS] = std::move(actions);

    r[Common::OEM] = json::Json::value_t::object;

    return r;
}


void fill_links(const agent_framework::model::Switch& pcie_switch, const server::Request& req, json::Json& json) {
    // fill ManagedBy link
    // the Manager is the parent of the Fabric containing this Switch
    auto pnc_manager_uuid = psme::rest::model::find<agent_framework::model::Fabric>(
        req.params).get_one()->get_parent_uuid();

    auto manager_id = agent_framework::module::get_manager<agent_framework::model::Manager>()
        .get_entry(pnc_manager_uuid).get_id();
    json::Json manager_link = json::Json();
    manager_link[Common::ODATA_ID] = endpoint::PathBuilder(PathParam::BASE_URL)
        .append(constants::Root::MANAGERS)
        .append(manager_id).build();
    json[Common::LINKS][Common::MANAGED_BY].push_back(std::move(manager_link));

    // fill Chassis link
    if (pcie_switch.get_chassis().has_value()) {
        try {
            json::Json chassis_link = json::Json();
            chassis_link[Common::ODATA_ID] = endpoint::utils::get_component_url(
                agent_framework::model::enums::Component::Chassis, pcie_switch.get_chassis().value());
            json[Common::LINKS][Common::CHASSIS].push_back(std::move(chassis_link));
        }
        catch (const agent_framework::exceptions::InvalidUuid&) {
            log_error("rest", "Fabric Switch " + pcie_switch.get_uuid() + " has chassis "
                              + pcie_switch.get_chassis().value() + " which does not exist as a resource");
        }
    }

    // fill Ports field
    json::Json ports_link = json::Json();
    ports_link[Common::ODATA_ID] = endpoint::PathBuilder(req)
        .append(constants::Switch::PORTS).build();
    json[constants::Switch::PORTS] = std::move(ports_link);

}

}


endpoint::Switch::Switch(const std::string& path) : EndpointBase(path) {}


endpoint::Switch::~Switch() {}


void endpoint::Switch::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    const auto pcie_switch =
        psme::rest::model::find<agent_framework::model::Fabric, agent_framework::model::Switch>(req.params).get();

    json[Common::ACTIONS][constants::Switch::SWITCH_RESET][Common::TARGET] = endpoint::PathBuilder(
        req).append(Common::ACTIONS).append(constants::Switch::SWITCH_RESET_ENDPOINT).build();
    for (const auto& allowed_reset_type : pcie_switch.get_allowed_actions()) {
        json[Common::ACTIONS][constants::Switch::SWITCH_RESET][Common::ALLOWABLE_RESET_TYPES].push_back(
            allowed_reset_type.to_string());
    }

    json[Common::ID] = req.params[PathParam::SWITCH_ID];

    fill_links(pcie_switch, req, json);

    const auto& fru = pcie_switch.get_fru_info();
    json[Common::MANUFACTURER] = fru.get_manufacturer();
    json[Common::MODEL] = fru.get_model_number();
    json[Common::PART_NUMBER] = fru.get_part_number();
    json[Common::SERIAL_NUMBER] = fru.get_serial_number();

    json[Common::ASSET_TAG] = pcie_switch.get_asset_tag();
    json[constants::Switch::SWITCH_TYPE] = pcie_switch.get_protocol();
    json[constants::Switch::POWER_STATE] = pcie_switch.get_power_state();
    json[constants::Switch::INDICATOR_LED] = pcie_switch.get_indicator_led();
    json[Common::SKU] = pcie_switch.get_sku();

    endpoint::status_to_json(pcie_switch, json);

    json[Common::STATUS][Common::HEALTH_ROLLUP] =
        endpoint::HealthRollup<agent_framework::model::Switch>().get(pcie_switch.get_uuid());

    set_response(res, json);
}


namespace {

static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::Switch::ASSET_TAG, constants::Common::ASSET_TAG}
};

}


void endpoint::Switch::patch(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PostSwitch";

    auto pnc_switch = model::find<agent_framework::model::Switch>(request.params).get();
    agent_framework::model::attribute::Attributes attributes{};

    const auto& json = JsonValidator::validate_request_body<schema::SwitchPatchSchema>(request);

    if (json.value(Common::ASSET_TAG, json::Json()).is_string()) {
        attributes.set_value(agent_framework::model::literals::Switch::ASSET_TAG,
                             json[Common::ASSET_TAG].get<std::string>());
    }

    if (!attributes.empty()) {
        agent_framework::model::requests::SetComponentAttributes set_component_attributes_request
            {pnc_switch.get_uuid(), attributes};

        const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(pnc_switch.get_agent_id());

        auto set_switch_attributes = [&, gami_agent] {
            const auto& set_component_attributes_response = gami_agent
                ->execute<agent_framework::model::responses::SetComponentAttributes>(set_component_attributes_request);

            const auto& result_statuses = set_component_attributes_response.get_statuses();
            if (!result_statuses.empty()) {
                const auto& error = error::ErrorFactory::create_error_from_set_component_attributes_results(
                    result_statuses, gami_to_rest_attributes);
                throw error::ServerException(error);
            }

            psme::rest::model::handler::HandlerManager::get_instance()->
                get_handler(agent_framework::model::enums::Component::Switch)->
                load(gami_agent, pnc_switch.get_parent_uuid(),
                     agent_framework::model::enums::Component::Chassis, pnc_switch.get_uuid(), false);
        };

        gami_agent->execute_in_transaction(TRANSACTION_NAME, set_switch_attributes);
    }
    get(request, response);
}
