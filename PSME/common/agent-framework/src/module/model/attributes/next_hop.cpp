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
 *
 *
 * @file next_hop.cpp
 * @brief Next hop neighbor data
 * */

#include "agent-framework/module/model/attributes/next_hop.hpp"
#include "agent-framework/module/constants/network.hpp"

using namespace agent_framework::model::attribute;

NextHop::NextHop() { }

NextHop::~NextHop() { }

json::Json NextHop::to_json() const {
    json::Json json = json::Json();
    json[literals::NextHop::METRIC] = get_metric();
    json[literals::NextHop::PORT_IDENTIFIER] = get_port_identifier();
    json[literals::NextHop::IPV4_ADDRESS] = get_ipv4_address();
    json[literals::NextHop::IPV6_ADDRESS] = get_ipv6_address();
    return json;
}

NextHop NextHop::from_json(const json::Json& json) {
    attribute::NextHop next_hop;
    next_hop.set_metric(json[literals::NextHop::METRIC]);
    next_hop.set_port_identifier(json[literals::NextHop::PORT_IDENTIFIER]);
    next_hop.set_ipv4_address(json[literals::NextHop::IPV4_ADDRESS]);
    next_hop.set_ipv6_address(json[literals::NextHop::IPV6_ADDRESS]);
    return next_hop;
}
