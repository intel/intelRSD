/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * */

#include "agent-framework/module/model/attributes/ipv4_address.hpp"
#include "agent-framework/module/constants/common.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

Ipv4Address::Ipv4Address() { }

Ipv4Address::~Ipv4Address() { }

json::Json Ipv4Address::to_json() const {
    json::Json result = json::Json();
    if (get_address()) {
        result[literals::Ipv4Address::ADDRESS] = get_address();
        result[literals::Ipv4Address::SUBNET_MASK] = get_subnet_mask();
        result[literals::Ipv4Address::ADDRESS_ORIGIN] = get_address_origin();
        result[literals::Ipv4Address::GATEWAY] = get_gateway();
    }
    return result;
}

Ipv4Address Ipv4Address::from_json(const json::Json& json) {
    Ipv4Address ipv4_address{};
    if (!json.is_null()) {
        ipv4_address.set_address(json[literals::Ipv4Address::ADDRESS]);
        ipv4_address.set_subnet_mask(json[literals::Ipv4Address::SUBNET_MASK]);
        ipv4_address.set_address_origin(json[literals::Ipv4Address::ADDRESS_ORIGIN]);
        ipv4_address.set_gateway(json[literals::Ipv4Address::GATEWAY]);
    }
    return ipv4_address;
}
