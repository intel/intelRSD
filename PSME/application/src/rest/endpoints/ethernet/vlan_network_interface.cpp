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

#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/ethernet/vlan_network_interface.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/vlan_network_interface.hpp"

#include "agent-framework/module/requests/network.hpp"
#include "agent-framework/module/responses/network.hpp"
#include "agent-framework/module/responses/common.hpp"
#include "agent-framework/module/network_components.hpp"

#include "json-wrapper/json-wrapper.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::error;
using namespace psme::rest::validators;
using namespace agent_framework::model::attribute;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#VLanNetworkInterface.VLanNetworkInterface";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#VLanNetworkInterface.v1_0_1.VLanNetworkInterface";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "VLAN Network Interface";
    r[Common::DESCRIPTION] = "VLAN Network Interface description";
    r[Vlan::VLAN_ENABLE] = json::Json::value_t::null;
    r[Vlan::VLAN_ID] = json::Json::value_t::null;

    json::Json rs = json::Json();
    rs[Common::ODATA_TYPE] = "#Intel.Oem.VLanNetworkInterface";
    rs[::Vlan::TAGGED] = json::Json::value_t::null;
    rs[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    rs[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    rs[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;
    r[Common::OEM][Common::RACKSCALE] = std::move(rs);

    return r;
}


Attributes fill_attributes(json::Json& json) {
    Attributes attributes{};

    if (json.count(constants::Vlan::VLAN_ID)) {
        attributes.set_value(agent_framework::model::literals::Vlan::VLAN_ID,
                             json[constants::Vlan::VLAN_ID].get<std::int64_t>());
    }
    return attributes;
}


static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::Vlan::VLAN_ID,
        constants::Vlan::VLAN_ID}
};
}


endpoint::VlanNetworkInterface::VlanNetworkInterface(const std::string& path) : EndpointBase(path) {}


endpoint::VlanNetworkInterface::~VlanNetworkInterface() {}


void endpoint::VlanNetworkInterface::get(const server::Request& request, server::Response& response) {
    auto r = make_prototype();

    r[Common::ODATA_ID] = PathBuilder(request).build();
    r[Common::ID] = request.params[PathParam::VLAN_ID];
    r[Common::NAME] = Vlan::VLAN + request.params[PathParam::VLAN_ID];

    auto vlan =
        psme::rest::model::find<agent_framework::model::EthernetSwitch, agent_framework::model::EthernetSwitchPort, agent_framework::model::EthernetSwitchPortVlan>(
            request.params).get();

    endpoint::status_to_json(vlan, r[Common::OEM][Common::RACKSCALE]);

    r[Vlan::VLAN_ENABLE] = vlan.get_vlan_enable();
    r[Vlan::VLAN_ID] = vlan.get_vlan_id();
    r[Common::OEM][Common::RACKSCALE][Vlan::TAGGED] = vlan.get_tagged();

    set_response(response, r);
}


void endpoint::VlanNetworkInterface::del(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "DeleteVlanNetworkInterface";

    using AgentManager = psme::core::agent::AgentManager;
    using HandlerManager = psme::rest::model::handler::HandlerManager;

    auto vlan_interface =
        psme::rest::model::find<agent_framework::model::EthernetSwitch, agent_framework::model::EthernetSwitchPort, agent_framework::model::EthernetSwitchPortVlan>(
            request.params).get();
    auto gami_agent = AgentManager::get_instance()->get_agent(vlan_interface.get_agent_id());

    auto delete_vlan = [&, gami_agent] {
        // Those resources are needed to properly lock the resource tree and avoid deadlocks
        auto parent_switch = psme::rest::model::find<agent_framework::model::EthernetSwitch>(request.params).get_one();
        auto parent_switch_port = psme::rest::model::find<agent_framework::model::EthernetSwitch, agent_framework::model::EthernetSwitchPort>(
            request.params).get_one();
        auto vlan =
            psme::rest::model::find<agent_framework::model::EthernetSwitch, agent_framework::model::EthernetSwitchPort, agent_framework::model::EthernetSwitchPortVlan>(
                request.params).get_one();

        auto port_uuid = vlan->get_parent_uuid();
        auto switch_uuid = parent_switch->get_uuid();

        auto gami_request = agent_framework::model::requests::DeletePortVlan{vlan->get_uuid()};
        gami_agent->execute<agent_framework::model::responses::DeletePortVlan>(gami_request);

        // remove the resource
        HandlerManager::get_instance()->
            get_handler(agent_framework::model::enums::Component::EthernetSwitchPortVlan)->remove(vlan->get_uuid());

        psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
            agent_framework::model::enums::Component::EthernetSwitchPort)->
            load(gami_agent, switch_uuid, agent_framework::model::enums::Component::EthernetSwitch, port_uuid, false);

        response.set_status(server::status_2XX::NO_CONTENT);
    };

    gami_agent->execute_in_transaction(TRANSACTION_NAME, delete_vlan);
}


void endpoint::VlanNetworkInterface::patch(const server::Request& request, server::Response& response) {
    using HandlerManager = psme::rest::model::handler::HandlerManager;
    static const constexpr char TRANSACTION_NAME[] = "PatchVlanNetworkInterface";

    auto json = JsonValidator::validate_request_body<schema::VlanNetworkInterfacePatchSchema>(request);
    auto attributes = fill_attributes(json);

    // Holding reference to parent object ensures that locks are acquired in the same order in each thread
    auto parent_switch = psme::rest::model::find<agent_framework::model::EthernetSwitch>(request.params).get();
    auto port = psme::rest::model::find<agent_framework::model::EthernetSwitch, agent_framework::model::EthernetSwitchPort>(
        request.params).get();
    auto vlan =
        psme::rest::model::find<agent_framework::model::EthernetSwitch, agent_framework::model::EthernetSwitchPort, agent_framework::model::EthernetSwitchPortVlan>(
            request.params).get();
    auto gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(port.get_agent_id());

    auto patch_vlan = [&, gami_agent] {
        bool updated{false};
        if (!attributes.empty()) {
            const auto& set_component_attributes_request =
                agent_framework::model::requests::SetComponentAttributes{vlan.get_uuid(), attributes};
            const auto& set_component_attributes_response =
                gami_agent->execute<agent_framework::model::responses::SetComponentAttributes>
                    (set_component_attributes_request);

            const auto& result_statuses = set_component_attributes_response.get_statuses();
            if (!result_statuses.empty()) {
                const auto& error = ErrorFactory::create_error_from_set_component_attributes_results(
                    result_statuses, gami_to_rest_attributes);
                throw ServerException(error);
            }
            updated = true;
        }
        if (updated) {
            HandlerManager::get_instance()->
                get_handler(agent_framework::model::enums::Component::EthernetSwitchPortVlan)->
                load(gami_agent, port.get_uuid(),
                     agent_framework::model::enums::Component::EthernetSwitchPort,
                     vlan.get_uuid(), true);
        }
    };

    gami_agent->execute_in_transaction(TRANSACTION_NAME, patch_vlan);

    get(request, response);
}
