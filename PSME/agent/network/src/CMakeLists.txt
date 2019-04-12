# <license_header>
#
# Copyright (c) 2015-2019 Intel Corporation
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

add_subdirectory(utils)
add_subdirectory(command)
add_subdirectory(loader)
add_subdirectory(discovery)
add_subdirectory(tree_stability)
add_subdirectory(hal)

if (BUILD_EOS_SDK)
    add_library(network-utils
        $<TARGET_OBJECTS:network-loaders>
        $<TARGET_OBJECTS:network-utils-impl>
        $<TARGET_OBJECTS:network-tree-stability>
        $<TARGET_OBJECTS:network-discovery-impl>
        $<TARGET_OBJECTS:eos-sdk-hal-impl>
        $<TARGET_OBJECTS:eos-eapi-hal-impl>
        $<TARGET_OBJECTS:hal>
    )
else()
    add_library(network-utils
        $<TARGET_OBJECTS:hal>
    )
endif()

if (BUILD_EOS_SDK)
    set(network_commands_targets
        $<TARGET_OBJECTS:network-command>
    )

    if (network_commands_targets)
        add_library(network-commands STATIC ${network_commands_targets})

        set(AGENT_NETWORK_COMMANDS
            -Wl,--whole-archive network-commands -Wl,--no-whole-archive
        )
    endif()

    add_executable(psme-network
        main.cpp
    )

    generate_default_config_file(
        main.cpp
        agent
        generic
        "AGENT_NETWORK_CONFIGURATION_FILE"
        ../include/default_configuration.hpp
        ..
    )

    target_link_libraries(psme-network
        eos
        ${AGENT_NETWORK_COMMANDS}
        network-utils
        agent-framework
        ${NET_LIBRARIES}
        ${UUID_LIBRARIES}
        ${LOGGER_LIBRARIES}
        configuration
        ${PCA95XX_LIBRARIES}
        ${SAFESTRING_LIBRARIES}
        pthread
        ${MICROHTTPD_LIBRARIES}
        ${CURL_LIBRARIES}
        dl
        md5
    )
endif()
