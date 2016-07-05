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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(command)
add_subdirectory(discovery)
add_subdirectory(status)

add_library(network-commands-stubs STATIC
    $<TARGET_OBJECTS:network-command-stubs>
)

add_library(network-stubs-utils
    $<TARGET_OBJECTS:network-stubs-discovery-impl>
    $<TARGET_OBJECTS:network-stubs-status-impl>
)

add_executable(psme-network-stubs
    generate_data.cpp
    main.cpp
    generate_data.cpp
)

target_link_libraries(psme-network-stubs
    ${AGENT_NETWORK_LIBRARIES}
    network-stubs-utils
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
    ${MD5_LIBRARIES}
)
