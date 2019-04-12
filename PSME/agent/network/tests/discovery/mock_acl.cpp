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
 * */

#include "hal/acl.hpp"

using namespace agent::network::hal;
using namespace std;

static const string ACL_NAME = "Acl1";

Acl::Acl() {
    m_acls = {ACL_NAME};
    m_bound_ports[ACL_NAME] = {"sw0p1"};
    m_rules =
    {
        {1, Acl::ActionType::ACTION_DENY, "", {},
        Acl::MirrorType::MIRROR_NA,
        {{Acl::ConditionType::CONDITION_IP_SRC, 3234240300, 0}}},
        {2, Acl::ActionType::ACTION_PERMIT, "", {},
        Acl::MirrorType::MIRROR_NA,
        {{Acl::ConditionType::CONDITION_IP_DEST, 3234240300, 0}}},
        {3, Acl::ActionType::ACTION_FORWARD, "sw0p1", {},
        Acl::MirrorType::MIRROR_NA,
        {{Acl::ConditionType::CONDITION_MAC_SRC, 3234240300, 0}}},
        {4, Acl::ActionType::ACTION_MIRROR, "sw0p2", {"sw0p1"},
        Acl::MirrorType::MIRROR_INGRESS,
        {{Acl::ConditionType::CONDITION_MAC_DEST, 3234240300, 0}}},
        {5, Acl::ActionType::ACTION_DENY, "", {},
        Acl::MirrorType::MIRROR_NA,
        {{Acl::ConditionType::CONDITION_VLAN, 300, 0}}},
        {6, Acl::ActionType::ACTION_DENY, "", {},
        Acl::MirrorType::MIRROR_NA,
        {{Acl::ConditionType::CONDITION_PROTOCOL, 30, 0}}},
        {7, Acl::ActionType::ACTION_DENY, "", {},
        Acl::MirrorType::MIRROR_NA,
        {{Acl::ConditionType::CONDITION_L4_PORT_SRC, 3, 0}}},
        {8, Acl::ActionType::ACTION_DENY, "", {},
        Acl::MirrorType::MIRROR_NA,
        {{Acl::ConditionType::CONDITION_L4_PORT_DEST, 3, 0}}}
    };
}

Acl::~Acl() {}

void Acl::read_acl_list() {}

void Acl::read_acl_rule_list(const string&) {}

const std::string& Acl::get_acl_name(const string&) {
    return ACL_NAME;
}

void Acl::add_acl_name(const string&, const string&) {}
