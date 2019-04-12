/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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

#include "agent-framework/module/model/attributes/acl_address.hpp"
#include "agent-framework/module/constants/network.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

AclAddress::AclAddress() { }

AclAddress::~AclAddress() { }

json::Json AclAddress::to_json() const {
    json::Json result = json::Json();
    result[literals::AclRule::ADDRESS] = get_address();
    result[literals::AclRule::MASK] = get_mask();
    return result;
}

AclAddress AclAddress::from_json(const json::Json& json) {
    AclAddress address;
    address.set_address(json[literals::AclRule::ADDRESS]);
    address.set_mask(json[literals::AclRule::MASK]);
    return address;
}
