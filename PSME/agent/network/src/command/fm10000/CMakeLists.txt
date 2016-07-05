# <license_header>
#
# Copyright (c) 2015-2016 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# </license_header>

set(SOURCES
    initialization.cpp
    get_manager_collection.cpp
    get_collection.cpp
    set_component_attributes.cpp
    get_manager_info.cpp
    get_chassis_info.cpp
    get_ethernet_switch_info.cpp
    get_ethernet_switch_port_info.cpp
    get_port_vlan_info.cpp
    add_port_vlan.cpp
    add_ethernet_switch_port_members.cpp
    delete_port_vlan.cpp
    add_ethernet_switch_port.cpp
    delete_ethernet_switch_port.cpp
    delete_ethernet_switch_port_members.cpp
    get_remote_ethernet_switch_info.cpp
    add_acl.cpp
    delete_acl.cpp
    get_acl_info.cpp
    add_acl_rule.cpp
    add_acl_port.cpp
    delete_acl_rule.cpp
    delete_acl_port.cpp
    get_acl_rule_info.cpp
    get_port_static_mac_info.cpp
    add_port_static_mac.cpp
    delete_port_static_mac.cpp
)

add_library(network-command-fm10000 OBJECT ${SOURCES})
set_psme_command_target_properties(network-command-fm10000)
