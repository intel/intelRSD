# <license_header>
#
# Copyright (c) 2017-2019 Intel Corporation
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
    eapi_client.cpp
    eapi_command.cpp
    port_running_configuration_parser.cpp
    switch_running_configuration_parser.cpp
    get_switch_running_configuration.cpp
    get_port_running_configuration.cpp
    set_port_pfc_enabled.cpp
    set_port_pfc_priorities.cpp
    set_port_lldp_enabled.cpp
    set_port_dcbx_state.cpp
    set_switch_pfc_enabled.cpp
    set_switch_lldp_enabled.cpp
    add_switch_vlan.cpp
    delete_switch_vlan.cpp
    add_switch_qos_application_protocol.cpp
    delete_switch_qos_application_protocol.cpp
    add_switch_qos_priority_group_mapping.cpp
    delete_switch_qos_priority_group_mapping.cpp
    add_switch_qos_bandwidth_allocation.cpp
    delete_switch_qos_bandwidth_allocation.cpp
    show_priority_flow_control.cpp
    show_priority_flow_control_parser.cpp
    switch_info_impl.cpp
    switch_port_info_impl.cpp
)

add_library(eos-eapi-hal-impl OBJECT ${SOURCES})
