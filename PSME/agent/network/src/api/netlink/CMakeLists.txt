# <license_header>
#
# Copyright (c) 2015-2017 Intel Corporation
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


set_source_files_properties(
                            lag_message.cpp
                            lag_port_message.cpp
                            COMPILE_FLAGS "-Wno-missing-field-initializers -Wno-old-style-cast -Wno-cast-align")

set(NETLINK_SOURCES
    link_message.cpp
    info_vlan_port_message.cpp
    add_vlan_port_message.cpp
    del_vlan_port_message.cpp
    port_message.cpp
    port_attribute_message.cpp
    lag_message.cpp
    add_lag_message.cpp
    del_lag_message.cpp
    lag_port_message.cpp
    del_lag_port_message.cpp
    add_lag_port_message.cpp
    ethtool.cpp
    switch_port_info.cpp
    switch_vlan.cpp
    switch_info.cpp
    sysfs.cpp
    acl_message.cpp
    static_mac_message.cpp
    get_port_state_message.cpp
    set_port_state_message.cpp
   )

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(
                                lag_message.cpp
                                lag_port_message.cpp
                                PROPERTIES
                                COMPILE_FLAGS "-Wno-old-style-cast -Wno-missing-field-initializers -Wno-sign-conversion"
                               )
endif()

add_library(api-fm10000-netlink OBJECT ${NETLINK_SOURCES})
