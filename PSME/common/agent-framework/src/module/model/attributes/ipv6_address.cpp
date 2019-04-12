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

#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/model/attributes/ipv6_address.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

Ipv6Address::Ipv6Address() { }

Ipv6Address::~Ipv6Address() { }

json::Json Ipv6Address::to_json() const {
    json::Json result = json::Json();
    if (get_address()) {
        result[literals::Ipv6Address::ADDRESS] = get_address();
        result[literals::Ipv6Address::PREFIX_LENGTH] = get_prefix_length();
        result[literals::Ipv6Address::ADDRESS_ORIGIN] = get_address_origin();
        result[literals::Ipv6Address::ADDRESS_STATE] = get_address_state();
    }
    return result;
}

Ipv6Address Ipv6Address::from_json(const json::Json& json) {
    attribute::Ipv6Address ipv6_address{};
    if (!json.is_null()) {
        ipv6_address.set_address(json[literals::Ipv6Address::ADDRESS]);
        ipv6_address.set_prefix_length(json[literals::Ipv6Address::PREFIX_LENGTH]);
        ipv6_address.set_address_origin(OptionalField<enums::Ipv6AddressOrigin>(
            json[literals::Ipv6Address::ADDRESS_ORIGIN]));
        ipv6_address.set_address_state(OptionalField<enums::Ipv6AddressState>(
            json[literals::Ipv6Address::ADDRESS_STATE]));
    }
    return ipv6_address;
}
