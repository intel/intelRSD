# <license_header>
#
# Copyright (c) 2016-2019 Intel Corporation
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
add_subdirectory(gas)
add_subdirectory(sysfs)
add_subdirectory(discovery)
add_subdirectory(loader)
add_subdirectory(nvme)
add_subdirectory(protocol/pldm)
add_subdirectory(protocol/smbus)
add_subdirectory(tree_stability)
add_subdirectory(state_machine)
add_subdirectory(i2c)
add_subdirectory(tools)
add_subdirectory(aardvark)

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
    $<TARGET_OBJECTS:pnc-sysfs>
    $<TARGET_OBJECTS:pnc-loader>
    $<TARGET_OBJECTS:pnc-nvme>
    $<TARGET_OBJECTS:pnc-state-machine>
    $<TARGET_OBJECTS:pnc-i2c>
    $<TARGET_OBJECTS:pnc-aardvark>
    )

target_link_libraries(pnc-libs PUBLIC
    pnc-discovery
    pnc-pldm
    pnc-smbus
    pnc-tree-stability)

add_executable(psme-pnc
    main.cpp
)

generate_default_config_file(
    main.cpp
    agent
    generic
    "AGENT_PNC_CONFIGURATION_FILE"
    ../include/default_configuration.hpp
    ..
)

target_link_libraries(psme-pnc
    ${AGENT_PNC_LIBRARIES}
    pnc-libs
    agent-framework
    agent-framework-discovery
    agent-framework-database
    ipmi
    fru_eeprom
    ${IPMITOOL_LIBRARIES}
    sysfsref
    ${UUID_LIBRARIES}
    ${LOGGER_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    configuration
    pthread
    ${CURL_LIBRARIES}
    ${MICROHTTPD_LIBRARIES}
    opae11pp
    md5
    ${AARDVARK_LIBRARIES})
