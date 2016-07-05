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

#include "agent-framework/module/ipv4_address.hpp"

using namespace agent_framework::generic;

IPv4Address::~IPv4Address(){}

Json::Value IPv4Address::to_json() const {
    Json::Value result;

    result["address"] = m_address;
    result["subnetMask"] = m_subnet_mask;
    result["addressOrigin"] = m_address_origin;
    result["gateway"] = m_gateway;

    return result;
}

