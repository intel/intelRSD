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
add_subdirectory(loader)
add_subdirectory(status)

add_library(compute-commands STATIC
    $<TARGET_OBJECTS:compute-command-sdv>
)


add_library(compute-utils
    $<TARGET_OBJECTS:compute-status-impl>
    $<TARGET_OBJECTS:compute-discovery>
    $<TARGET_OBJECTS:compute-loader>
)

add_executable(psme-compute
    main.cpp
)

target_link_libraries(psme-compute
    ${AGENT_COMPUTE_LIBRARIES}
    compute-utils
    ${AGENT_FRAMEWORK_LIBRARIES}
    ${UUID_LIBRARIES}
    ${LOGGER_LIBRARIES}
    ${PCA95XX_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    ${CONFIGURATION_LIBRARIES}
    ${JSONCXX_LIBRARIES}
    ipmi
    ${IPMITOOL_LIBRARIES}
    pthread
    jsonrpccpp-server
    jsonrpccpp-client
    jsonrpccpp-common
    jsoncpp
    ${MICROHTTPD_LIBRARIES}
    ${CURL_LIBRARIES}
    md5
    netlink
    ${NL3_LIBRARIES}
)

