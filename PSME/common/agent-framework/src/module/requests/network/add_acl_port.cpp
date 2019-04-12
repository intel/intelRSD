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
 *
 * @file requests/network/add_acl_port.cpp
 *
 * @brief Network request AddAclPort implementation
 * */

#include "agent-framework/module/requests/network/add_acl_port.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

AddAclPort::AddAclPort(const std::string& acl, const Ports& ports,
                       const Oem& oem)
    : m_acl(acl), m_ports(ports), m_oem(oem) {}

json::Json AddAclPort::to_json() const {
    json::Json value = json::Json();
    value[Acl::ACL] = m_acl;
    value[Acl::PORTS] = m_ports.to_json();
    value[Acl::OEM] = m_oem.to_json();
    return value;
}

AddAclPort AddAclPort::from_json(const json::Json& json) {
    return AddAclPort{json[Acl::ACL],
        Ports::from_json(json[Acl::PORTS]),
        Oem::from_json(json[Acl::OEM])
    };
}
