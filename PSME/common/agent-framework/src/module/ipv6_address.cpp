/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
 *
 * */

#include "agent-framework/module/ipv6_address.hpp"

using namespace agent_framework::generic;

IPv6Address::~IPv6Address(){}

Json::Value IPv6Address::to_json() const {
    Json::Value result;

    result["address"] = m_address;
    result["prefixLength"] = m_prefix_length;
    result["addressOrigin"] = m_address_origin;
    result["addressState"] = m_address_state;

    return result;
}

