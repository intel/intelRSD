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
#include "psme/rest/endpoints/ethernet/ethernet_switch_port_metrics.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace agent_framework::module;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitchPortMetrics.EthernetSwitchPortMetrics";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#EthernetSwitchPortMetrics.v1_0_0.EthernetSwitchPortMetrics";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Ethernet Switch Port Metrics";
    r[Common::DESCRIPTION] = "Ethernet Switch Port Metrics";

    json::Json data = json::Json();
    data[constants::EthernetSwitchPortMetrics::PACKETS] = json::Json::value_t::null;
    data[constants::EthernetSwitchPortMetrics::DROPPED_PACKETS] = json::Json::value_t::null;
    data[constants::EthernetSwitchPortMetrics::ERROR_PACKETS] = json::Json::value_t::null;
    data[constants::EthernetSwitchPortMetrics::BROADCAST_PACKETS] = json::Json::value_t::null;
    data[constants::EthernetSwitchPortMetrics::MULTICAST_PACKETS] = json::Json::value_t::null;
    data[constants::EthernetSwitchPortMetrics::ERRORS] = json::Json::value_t::null;
    data[constants::EthernetSwitchPortMetrics::BYTES] = json::Json::value_t::null;

    json::Json data_received = data;
    r[constants::EthernetSwitchPortMetrics::RECEIVED] = std::move(data_received);
    r[constants::EthernetSwitchPortMetrics::TRANSMITTED] = std::move(data);

    r[constants::EthernetSwitchPortMetrics::COLLISIONS] = json::Json::value_t::null;

    return r;
}

}


endpoint::EthernetSwitchPortMetrics::EthernetSwitchPortMetrics(const std::string& path) : EndpointBase(path) {}


endpoint::EthernetSwitchPortMetrics::~EthernetSwitchPortMetrics() {}


void endpoint::EthernetSwitchPortMetrics::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();
    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ID] = constants::Common::METRICS;
    r[Common::NAME] = "Port " + req.params[PathParam::SWITCH_PORT_ID] + " Metrics";

    // check if port exists
    psme::rest::model::find<agent_framework::model::EthernetSwitch, agent_framework::model::EthernetSwitchPort>(
        req.params).get_uuid();

    set_response(res, r);
}
