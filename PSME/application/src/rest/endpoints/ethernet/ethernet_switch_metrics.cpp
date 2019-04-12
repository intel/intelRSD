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

#include "psme/rest/endpoints/ethernet/ethernet_switch_metrics.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;



namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitchMetric.EthernetSwitchMetric";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#EthernetSwitchMetrics.v1_0_0.EthernetSwitchMetrics";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Ethernet Switch Metrics";
    r[Common::DESCRIPTION] = "Ethernet Switch Metrics";

    r[Common::HEALTH] = json::Json::value_t::null;

    return r;
}

}

endpoint::EthernetSwitchMetrics::EthernetSwitchMetrics(const std::string& path) : EndpointBase(path) {}
endpoint::EthernetSwitchMetrics::~EthernetSwitchMetrics() {}

void endpoint::EthernetSwitchMetrics::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();

    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ID] = constants::Common::METRICS;
    r[Common::NAME] = "Switch " + req.params[PathParam::ETHERNET_SWITCH_ID] + " Metrics";

    // check if switch exists
    model::find<agent_framework::model::EthernetSwitch>(req.params).get_uuid();

    set_response(res, r);
}
