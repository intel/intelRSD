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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(certificate_management)
add_subdirectory(command)
add_subdirectory(discovery)
add_subdirectory(net_reader)
add_subdirectory(tree_stability)
add_subdirectory(utils)
add_subdirectory(ptas_reader)
add_subdirectory(loader)
add_subdirectory(event_collector)
add_subdirectory(telemetry)

add_library(rmm-libs STATIC
    $<TARGET_OBJECTS:rmm-certificate-management>
    $<TARGET_OBJECTS:rmm-discovery>
    $<TARGET_OBJECTS:rmm-command>
    $<TARGET_OBJECTS:rmm-net-reader>
    $<TARGET_OBJECTS:rmm-ptas-reader>
    $<TARGET_OBJECTS:rmm-utils>
    $<TARGET_OBJECTS:rmm-tree-stability>
    $<TARGET_OBJECTS:rmm-loader>
    $<TARGET_OBJECTS:rmm-event-collector>
    $<TARGET_OBJECTS:rmm-telemetry-service>
)

add_executable(psme-rmm
    main.cpp
    discovery_thread.cpp
)

generate_default_config_file(
    main.cpp
    agent
    generic
    "AGENT_RMM_CONFIGURATION_FILE"
    ../include/default_configuration.hpp
    ..
)

target_link_libraries(psme-rmm
    ${AGENT_RMM_LIBRARIES}
    ${NET_LIBRARIES}
    rmm-libs
    fru_eeprom
    telemetry
    agent-framework
    ${UUID_LIBRARIES}
    logger
    ${PCA95XX_LIBRARIES}
    configuration
    pthread
    ${CURL_LIBRARIES}
    ${MICROHTTPD_LIBRARIES}
    ${IPMI_LIBRARIES}
    ${IPMITOOL_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    sysfsref
    md5
)
