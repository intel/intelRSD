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
    # Interface
    switch_vlan.cpp
    switch_port_info.cpp
    switch_info.cpp
    remote_switch_info.cpp
    acl.cpp
    static_mac.cpp
)

if (NL3_FOUND)
    set_source_files_properties(
        netlink/vlan_message.cpp
        netlink/port_message.cpp
        netlink/port_attribute_message.cpp
        netlink/lag_message.cpp
        netlink/lag_port_message.cpp
        netlink/acl_message.cpp
        static_mac.cpp
        COMPILE_FLAGS "-Wno-missing-field-initializers -Wno-old-style-cast -Wno-cast-align")

    set(BDC_R_SOURCES ${SOURCES}
        # Netlink Interface
        netlink/vlan_message.cpp
        netlink/port_message.cpp
        netlink/port_attribute_message.cpp
        netlink/lag_message.cpp
        netlink/add_lag_message.cpp
        netlink/del_lag_message.cpp
        netlink/lag_port_message.cpp
        netlink/del_lag_port_message.cpp
        netlink/add_lag_port_message.cpp
        netlink/ethtool.cpp
        netlink/switch_port_info.cpp
        netlink/switch_vlan.cpp
        netlink/switch_info.cpp
        netlink/sysfs.cpp
        netlink/acl_message.cpp
        netlink/acl_socket.cpp
        netlink/static_mac_message.cpp
        # DCRP Interface
        dcrp/remote_switch_info.cpp
    )

    if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
        set_source_files_properties(
            netlink/vlan_message.cpp
            netlink/port_message.cpp
            netlink/port_attribute_message.cpp
            netlink/lag_message.cpp
            netlink/lag_port_message.cpp
            netlink/acl_message.cpp
            dcrp/remote_switch_info.cpp
            static_mac.cpp
            PROPERTIES
            COMPILE_FLAGS "-Wno-old-style-cast -Wno-missing-field-initializers -Wno-sign-conversion"
        )
    endif()

    add_subdirectory(netlink)
    add_subdirectory(dcrp)
    add_library(api-fm10000 OBJECT ${BDC_R_SOURCES})
endif()

add_subdirectory(lldp)
