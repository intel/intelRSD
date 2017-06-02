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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(api)
add_subdirectory(utils)
add_subdirectory(command)
add_subdirectory(loader)
add_subdirectory(status)
add_subdirectory(discovery)
add_subdirectory(tree_stability)

add_library(network-utils
    $<TARGET_OBJECTS:network-loaders>
    $<TARGET_OBJECTS:network-utils-fm10000>
    $<TARGET_OBJECTS:network-tree-stability>
    $<TARGET_OBJECTS:network-discovery-impl>
)

set(NETWORK_COMMON_LIBRARIES
    network-utils
    ${NET_LIBRARIES}
    ${AGENT_FRAMEWORK_LIBRARIES}
    ${UUID_LIBRARIES}
    ${LOGGER_LIBRARIES}
    ${CONFIGURATION_LIBRARIES}
    ${JSONCXX_LIBRARIES}
    ${PCA95XX_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    pthread
    jsonrpccpp-server
    jsonrpccpp-common
    jsonrpccpp-client
    jsoncpp
    ${MICROHTTPD_LIBRARIES}
    ${CURL_LIBRARIES}
    dl
)

if (NL3_FOUND)
    set(network_commands_targets_fm10000
        $<TARGET_OBJECTS:network-command-fm10000>
    )

    if (network_commands_targets_fm10000)
        add_library(network-commands-fm10000 STATIC ${network_commands_targets_fm10000})

        set(AGENT_NETWORK_FM10000_LIBRARIES
            -Wl,--whole-archive network-commands-fm10000 -Wl,--no-whole-archive
        )
    endif()

    add_library(network-hw-api-fm10000
        $<TARGET_OBJECTS:api-fm10000>
        $<TARGET_OBJECTS:api-fm10000-netlink>
        $<TARGET_OBJECTS:api-fm10000-dcrp>
        $<TARGET_OBJECTS:api-fm10000-lldp>
     )

    add_library(network-discovery-fm10000
        $<TARGET_OBJECTS:network-discovery-impl>
    )

    add_library(network-status-fm10000
        $<TARGET_OBJECTS:network-status-impl>
    )

    add_library(network-loader-fm10000
        $<TARGET_OBJECTS:network-loaders>
    )

    add_executable(psme-network-fm10000
        main.cpp
    )

    target_link_libraries(psme-network-fm10000
        ${AGENT_NETWORK_FM10000_LIBRARIES}
        network-hw-api-fm10000
        network-status-fm10000
        network-discovery-fm10000
        ${NL3_LIBRARIES}
        ${NL3_GENL_LIBRARIES}
        ${NETWORK_COMMON_LIBRARIES}
        ${NETLINK_LIBRARIES}
        md5
    )

endif()
