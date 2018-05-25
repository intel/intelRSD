# <license_header>
#
# Copyright (c) 2017-2018 Intel Corporation
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
add_subdirectory(loader)
add_subdirectory(discovery)
add_subdirectory(telemetry)
add_subdirectory(tools)
add_subdirectory(tree_stability)

add_library(nvme-loader
    $<TARGET_OBJECTS:nvme-loaders>
)

add_library(nvme-tree-stability
    $<TARGET_OBJECTS:nvme-tree-stability-objs>
)

add_library(nvme-tools
    $<TARGET_OBJECTS:nvme-tools-objs>
)

add_library(nvme-tools-db
    $<TARGET_OBJECTS:nvme-tools-db-objs>
)

add_library(nvme-tools-dir
    $<TARGET_OBJECTS:nvme-tools-dir-objs>
)

add_library(nvme-tools-context
    $<TARGET_OBJECTS:nvme-tools-context-objs>
)

add_library(nvme-utils
    $<TARGET_OBJECTS:nvme-discovery-impl>
    $<TARGET_OBJECTS:nvme-tools-drive-discovery>
)

add_library(nvme-telemetry
    $<TARGET_OBJECTS:nvme-telemetry-objs>
)

set(NVME_COMMON_LIBRARIES
    nvme-loader
    nvme-tools
    nvme-tools-db
    nvme-tools-context
    nvme-tools-dir
    nvme-utils
    nvme-tree-stability
    nvme-telemetry
    ${NVMFPP_LIBRARIES}
    ${SYSFSREF_LIBRARIES}
    ${NETLINK_LIBRARIES}
    ${NET_LIBRARIES}
    ${AGENT_FRAMEWORK_LIBRARIES}
    ${TELEMETRY_LIBRARIES}
    ${UUID_LIBRARIES}
    ${LOGGER_LIBRARIES}
    ${CONFIGURATION_LIBRARIES}
    ${JSONCXX_LIBRARIES}
    ${PCA95XX_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    ${DATABASE_LIBRARIES}
    pthread
    udev
    ${MICROHTTPD_LIBRARIES}
    ${CURL_LIBRARIES}
    ${NL3_LIBRARIES}
    ${NVME_TARGET_LIBRARIES}
    ${CRC_LIBRARIES}
    dl
)

set(nvme_commands_targets
    $<TARGET_OBJECTS:nvme-command>
)

add_library(nvme-commands STATIC ${nvme_commands_targets})

set(AGENT_NVME_COMMANDS
    -Wl,--whole-archive nvme-commands -Wl,--no-whole-archive
)

add_executable(psme-nvme
    main.cpp
)

generate_default_config_file(
    main.cpp
    agent
    generic
    "AGENT_NVME_CONFIGURATION_FILE"
)

target_link_libraries(psme-nvme
    ${AGENT_NVME_COMMANDS}
    nvme-utils
    ${NVME_LIBRARIES}
    ${NVMFPP_LIBRARIES}
    ${PARTITION_LIBRARIES}
    ${NVME_COMMON_LIBRARIES}
    md5
)
