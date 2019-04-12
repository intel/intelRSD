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
 * @file requests/network.hpp
 * @brief Common header for network requests
 * */

#pragma once
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/requests/network/add_vlan.hpp"
#include "agent-framework/module/requests/network/add_port_vlan.hpp"
#include "agent-framework/module/requests/network/add_ethernet_switch_port.hpp"
#include "agent-framework/module/requests/network/add_ethernet_switch_port_members.hpp"
#include "agent-framework/module/requests/network/delete_vlan.hpp"
#include "agent-framework/module/requests/network/delete_port_vlan.hpp"
#include "agent-framework/module/requests/network/delete_ethernet_switch_port.hpp"
#include "agent-framework/module/requests/network/delete_ethernet_switch_port_members.hpp"
#include "agent-framework/module/requests/network/get_vlan_info.hpp"
#include "agent-framework/module/requests/network/get_port_vlan_info.hpp"
#include "agent-framework/module/requests/network/get_remote_ethernet_switch_info.hpp"
#include "agent-framework/module/requests/network/get_ethernet_switch_info.hpp"
#include "agent-framework/module/requests/network/get_ethernet_switch_port_info.hpp"
#include "agent-framework/module/requests/network/delete_acl.hpp"
#include "agent-framework/module/requests/network/add_acl.hpp"
#include "agent-framework/module/requests/network/get_acl_info.hpp"
#include "agent-framework/module/requests/network/add_acl_rule.hpp"
#include "agent-framework/module/requests/network/get_acl_rule_info.hpp"
#include "agent-framework/module/requests/network/add_acl_port.hpp"
#include "agent-framework/module/requests/network/delete_acl_rule.hpp"
#include "agent-framework/module/requests/network/delete_acl_port.hpp"
#include "agent-framework/module/requests/network/get_port_static_mac_info.hpp"
#include "agent-framework/module/requests/network/add_port_static_mac.hpp"
#include "agent-framework/module/requests/network/delete_port_static_mac.hpp"

