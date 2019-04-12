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

#include "agent-framework/module/requests/network/add_port_vlan.hpp"
#include "agent-framework/module/responses/network/add_port_vlan.hpp"

#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/ethernet/vlan_network_interface_collection.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/vlan_network_interface_collection.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;
using namespace psme::rest::endpoint;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#VLanNetworkInterfaceCollection.VLanNetworkInterfaceCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#VLanNetworkInterfaceCollection.VLanNetworkInterfaceCollection";
    r[Common::NAME] = "VLAN Network Interface Collection";
    r[Common::DESCRIPTION] = "Collection of VLAN Network Interfaces";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}


const json::Json validate_post_request(const server::Request& request) {
    return JsonValidator::validate_request_body(request,
                                                schema::VlanNetworkInterfaceCollectionPostSchema::get_procedure());
}

}


VlanNetworkInterfaceCollection::VlanNetworkInterfaceCollection(const std::string& path) : EndpointBase(path) {}


VlanNetworkInterfaceCollection::~VlanNetworkInterfaceCollection() {}


void VlanNetworkInterfaceCollection::get(const server::Request& request, server::Response& response) {
    auto r = ::make_prototype();

    r[Common::ODATA_ID] = PathBuilder(request).build();

    auto port_uuid =
        psme::rest::model::find<agent_framework::model::EthernetSwitch, agent_framework::model::EthernetSwitchPort>(
            request.params).get_uuid();

    auto keys = agent_framework::module::NetworkComponents::get_instance()->get_port_vlan_manager().get_ids(port_uuid);
    r[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(keys.size());

    for (const auto& key : keys) {
        json::Json link_elem(json::Json::value_t::object);
        link_elem[Common::ODATA_ID] = PathBuilder(request).append(key).build();
        r[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(response, r);
}


void VlanNetworkInterfaceCollection::post(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PostVlanNetworkInterfaceCollection";

    auto port =
        psme::rest::model::find<agent_framework::model::EthernetSwitch, agent_framework::model::EthernetSwitchPort>(
            request.params).get();

    std::string agent_id = port.get_agent_id();
    std::string port_id = port.get_uuid();

    const auto json = validate_post_request(request);

    auto vlan_id = json[Vlan::VLAN_ID].get<std::uint32_t>();
    bool tagged = json[Common::OEM][Common::RACKSCALE][Vlan::TAGGED].get<bool>();
    auto oem = agent_framework::model::attribute::Oem{};

    agent_framework::model::requests::AddPortVlan add_port_vlan_req{
        port.get_uuid(),
        json.value(Common::NAME, OptionalField<std::string>()),
        vlan_id,
        tagged,
        oem
    };

    const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(agent_id);

    auto add_vlan = [&, gami_agent] {
        auto add_response = gami_agent->execute<agent_framework::model::responses::AddPortVlan>(add_port_vlan_req);

        psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
            agent_framework::model::enums::Component::EthernetSwitchPortVlan)->load(
            gami_agent, port_id,
            agent_framework::model::enums::Component::EthernetSwitchPort, add_response.get_port_vlan());

        auto created_vlan = agent_framework::module::NetworkComponents::get_instance()->
            get_port_vlan_manager().get_entry(add_response.get_port_vlan());

        psme::rest::endpoint::utils::set_location_header(
            request, response, PathBuilder(request).append(created_vlan.get_id()).build());

        response.set_status(server::status_2XX::CREATED);
    };
    gami_agent->execute_in_transaction(TRANSACTION_NAME, add_vlan);
}
