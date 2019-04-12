/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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

#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/manager/manager_vlan_network_interface.hpp"
#include "psme/rest/utils/status_helpers.hpp"



using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::constants;

namespace {

json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#VLanNetworkInterface.VLanNetworkInterface";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#VLanNetworkInterface.v1_0_0.VLanNetworkInterface";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "VLAN Network Interface";
    r[Common::DESCRIPTION] = "Ethernet Interface VLAN";
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

}


ManagerVlanNetworkInterface::ManagerVlanNetworkInterface(const std::string& path) :
    EndpointBase(path) {}


ManagerVlanNetworkInterface::~ManagerVlanNetworkInterface() {}


void ManagerVlanNetworkInterface::get(const server::Request& request, server::Response& response) {
    auto r = ::make_prototype();

    r[Common::ODATA_ID] = PathBuilder(request).build();
    r[Common::ID] = request.params[PathParam::VLAN_ID];
    r[Common::NAME] = Vlan::VLAN + request.params[PathParam::VLAN_ID];

    auto manager_uuid = psme::rest::model::find<agent_framework::model::Manager>(request.params).get_uuid();
    auto nic_ids = agent_framework::module::get_manager<agent_framework::model::NetworkInterface>()
        .get_ids(manager_uuid);

    if (nic_ids.empty()) {
        throw agent_framework::exceptions::NotFound(
            "There is no VLANs under NetworkInterface with ID: " + request.params[PathParam::NIC_ID] + ".");
    }

    auto vlan = psme::rest::model::find<agent_framework::model::Manager,
                                        agent_framework::model::NetworkInterface,
                                        agent_framework::model::EthernetSwitchPortVlan>(request.params).get();

    endpoint::status_to_json(vlan, r[Common::OEM][Common::RACKSCALE]);

    r[Vlan::VLAN_ENABLE] = vlan.get_vlan_enable();
    r[Vlan::VLAN_ID] = vlan.get_vlan_id();
    r[Common::OEM][Common::RACKSCALE][Vlan::TAGGED] = vlan.get_tagged();

    set_response(response, r);
}
