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

#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/endpoints/vlan_network_interface.hpp"
#include "psme/rest-ref/model/handlers/handler_manager.hpp"
#include "psme/rest-ref/model/handlers/generic_handler_deps.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"

#include "agent-framework/module-ref/requests/network.hpp"
#include "agent-framework/module-ref/responses/network.hpp"
#include "agent-framework/module-ref/network_manager.hpp"

#include <json/json.hpp>

using namespace psme::rest;
using namespace app::rest::constants;
using namespace agent_framework::model::attribute;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#VLanNetworkInterface.VLanNetworkInterface";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#VLanNetworkInterface.1.0.0.VLanNetworkInterface";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "VLAN Network Interface";
    r[Common::DESCRIPTION] = "VLAN Network Interface description";
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;
    r[Vlan::VLAN_ENABLE] = json::Value::Type::NIL;
    r[Vlan::VLAN_ID] = json::Value::Type::NIL;

    json::Value rs;
    rs[Common::ODATA_TYPE] =
        "#Intel.Oem.VLanNetworkInterface";
    rs[::Vlan::TAGGED] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE] = std::move(rs);

    return r;
}

}

endpoint::VlanNetworkInterface::VlanNetworkInterface(const std::string& path) : Endpoint(path) {}
endpoint::VlanNetworkInterface::~VlanNetworkInterface() {}

void endpoint::VlanNetworkInterface::get(const server::Request& req, server::Response& res) {
    auto r = make_prototype();

    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ID] = req.params[PathParam::VLAN_ID];
    r[Common::NAME] = Vlan::VLAN + req.params[PathParam::VLAN_ID];

    auto vlan = psme::rest::model::Find
        <agent_framework::model::PortVlan>(req.params[PathParam::VLAN_ID]).via
        <agent_framework::model::Switch>(req.params[PathParam::ETHERNET_SWITCH_ID]).via
        <agent_framework::model::SwitchPort>(req.params[PathParam::SWITCH_PORT_ID]).get();

    endpoint::utils::status_to_json(vlan, r);

    r[Vlan::VLAN_ENABLE] =  vlan.get_vlan_enable();
    r[Vlan::VLAN_ID] = vlan.get_vlan_id();
    r[Common::OEM][Common::RACKSCALE][::Vlan::TAGGED] =
        vlan.get_tagged();

    set_response(res,r);
}

void endpoint::VlanNetworkInterface::del(const server::Request& req, server::Response& res) {
    using AgentManager = psme::core::agent::AgentManager;
    using HandlerManager = psme::rest::model::handler::HandlerManager;

    auto vlan = psme::rest::model::Find
        <agent_framework::model::PortVlan>(req.params[PathParam::VLAN_ID]).via
        <agent_framework::model::Switch>(req.params[PathParam::ETHERNET_SWITCH_ID]).via
        <agent_framework::model::SwitchPort>(req.params[PathParam::SWITCH_PORT_ID]).get_one();

    auto agent_id = vlan->get_agent_id();
    auto port_uuid = vlan->get_parent_uuid();
    auto switch_uuid = psme::rest::model::Find
        <agent_framework::model::Switch>(req.params[PathParam::ETHERNET_SWITCH_ID]).get_one()->get_uuid();

    auto gami_req = agent_framework::model::requests::DeletePortVlan
        { vlan->get_uuid() };

    AgentManager::get_instance()->call_method
        <agent_framework::model::responses::DeletePortVlan>
        (vlan->get_agent_id(), gami_req);

    // remove the resource, DO NOT use vlan reference after this line!!
    HandlerManager::get_instance()->
        get_handler(agent_framework::model::enums::Component::PortVlan)->
        remove(vlan->get_uuid());

    psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::SwitchPort)->
        load(AgentManager::get_instance()->get_agent(agent_id), switch_uuid,
             port_uuid, false);

    res.set_status(server::status_2XX::NO_CONTENT);
}
