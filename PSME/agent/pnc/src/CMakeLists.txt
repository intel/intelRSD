# <license_header>
#
# Copyright (c) 2016-2017 Intel Corporation
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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(command)
add_subdirectory(status)
add_subdirectory(gas)
add_subdirectory(sysfs)
add_subdirectory(discovery)
add_subdirectory(loader)
add_subdirectory(nvme)
add_subdirectory(tree_stability)
add_subdirectory(state_machine)
add_subdirectory(i2c)
add_subdirectory(tools)

add_library(pnc-commands STATIC
            $<TARGET_OBJECTS:pnc-command>
            )

add_library(pnc-seeprom OBJECT seeprom.cpp)
add_library(pnc-cableid OBJECT cable_id.cpp)
add_library(pnc-fru_eeprom OBJECT fru_eeprom.cpp)
add_library(pnc-port-monitor OBJECT port_monitor_thread.cpp)

add_library(pnc-libs
    $<TARGET_OBJECTS:pnc-tools>
    $<TARGET_OBJECTS:pnc-port-monitor>
    $<TARGET_OBJECTS:pnc-seeprom>
    $<TARGET_OBJECTS:pnc-cableid>
    $<TARGET_OBJECTS:pnc-fru_eeprom>
    $<TARGET_OBJECTS:pnc-gas>
    $<TARGET_OBJECTS:pnc-discovery>
    $<TARGET_OBJECTS:pnc-tree-stability>
    $<TARGET_OBJECTS:pnc-status-impl>
    $<TARGET_OBJECTS:pnc-sysfs>
    $<TARGET_OBJECTS:pnc-loader>
    $<TARGET_OBJECTS:pnc-nvme>
    $<TARGET_OBJECTS:pnc-state-machine>
    $<TARGET_OBJECTS:pnc-i2c>
)

add_executable(psme-pnc
    main.cpp
)

target_link_libraries(psme-pnc
    ${AGENT_PNC_LIBRARIES}
    pnc-libs
    ipmi
    fru_eeprom
    ${IPMITOOL_LIBRARIES}
    ${AGENT_FRAMEWORK_LIBRARIES}
    ${SYSFS_LIBRARIES}
    ${UUID_LIBRARIES}
    ${LOGGER_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    ${CONFIGURATION_LIBRARIES}
    ${JSONCXX_LIBRARIES}
    pthread
    jsonrpccpp-server
    jsonrpccpp-common
    jsonrpccpp-client
    jsoncpp
    ${CURL_LIBRARIES}
    ${MICROHTTPD_LIBRARIES}
    md5
)
