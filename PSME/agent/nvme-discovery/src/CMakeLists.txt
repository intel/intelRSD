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
add_subdirectory(tools)
add_subdirectory(tree_stability)

add_library(discovery-loader
    $<TARGET_OBJECTS:discovery-loaders>
)

add_library(discovery-tree-stability
    $<TARGET_OBJECTS:discovery-tree-stability-objs>
)

add_library(discovery-tools
    $<TARGET_OBJECTS:discovery-tools-objs>
)

add_library(discovery-tools-db
    $<TARGET_OBJECTS:discovery-tools-db-objs>
)

add_library(discovery-tools-context
    $<TARGET_OBJECTS:discovery-tools-context-objs>
)

add_library(discovery-utils
    $<TARGET_OBJECTS:discovery-discovery-impl>
)

set(NVME_COMMON_LIBRARIES
    discovery-loader
    discovery-tools
    discovery-tools-db
    discovery-tools-context
    discovery-tree-stability
    discovery-utils
    ${AGENT_FRAMEWORK_LIBRARIES}
    ${UUID_LIBRARIES}
    ${LOGGER_LIBRARIES}
    ${CONFIGURATION_LIBRARIES}
    ${JSONCXX_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    ${DATABASE_LIBRARIES}
    pthread
    ${MICROHTTPD_LIBRARIES}
    ${CURL_LIBRARIES}
    dl
    ${NVMF_DISCOVERY_LIBRARIES}
)

set(discovery_commands_targets
    $<TARGET_OBJECTS:discovery-command>
)

add_library(discovery-commands STATIC ${discovery_commands_targets})

set(AGENT_DISCOVERY_COMMANDS
    -Wl,--whole-archive discovery-commands -Wl,--no-whole-archive
)

add_executable(psme-nvme-discovery
    main.cpp
)

generate_default_config_file(
    main.cpp
    agent
    generic
    "AGENT_NVME_CONFIGURATION_FILE"
)

target_link_libraries(psme-nvme-discovery
    ${AGENT_DISCOVERY_COMMANDS}
    ${NVME_COMMON_LIBRARIES}
    ${FABRIC_LIBRARIES}
    ${UTILS_LIBRARIES}
    md5
)
