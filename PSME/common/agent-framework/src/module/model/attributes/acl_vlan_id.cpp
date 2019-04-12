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

#include "agent-framework/module/model/attributes/acl_vlan_id.hpp"
#include "agent-framework/module/constants/network.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

AclVlanId::AclVlanId() { }

AclVlanId::~AclVlanId() { }

json::Json AclVlanId::to_json() const {
    json::Json result = json::Json();
    result[literals::AclRule::ID] = get_id();
    result[literals::AclRule::MASK] = get_mask();
    return result;
}

AclVlanId AclVlanId::from_json(const json::Json& json) {
    AclVlanId vlan_id;
    vlan_id.set_id(json[literals::AclRule::ID]);
    vlan_id.set_mask(json[literals::AclRule::MASK]);
    return vlan_id;
}
