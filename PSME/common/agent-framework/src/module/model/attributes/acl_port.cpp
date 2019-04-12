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

#include "agent-framework/module/model/attributes/acl_port.hpp"
#include "agent-framework/module/constants/network.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

AclPort::AclPort() { }

AclPort::~AclPort() { }

json::Json AclPort::to_json() const {
    json::Json result = json::Json();
    result[literals::AclRule::PORT] = get_port();
    result[literals::AclRule::MASK] = get_mask();
    return result;
}

AclPort AclPort::from_json(const json::Json& json) {
    AclPort acl_port;
    acl_port.set_port(json[literals::AclRule::PORT]);
    acl_port.set_mask(json[literals::AclRule::MASK]);
    return acl_port;
}
