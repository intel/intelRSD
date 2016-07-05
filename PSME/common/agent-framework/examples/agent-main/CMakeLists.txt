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

set (SOURCES
    main.cpp
)

include_directories(
    ${LOGGER_INCLUDE_DIRS}
    ${AGENT_FRAMEWORK_DIR}/src/module
    ${AGENT_FRAMEWORK_DIR}/src/state_machine
    ${AGENT_FRAMEWORK_DIR}/src/status
    ${AGENT_FRAMEWORK_DIR}/src/client
    ${AGENT_FRAMEWORK_DIR}/src/registration
    ${AGENT_FRAMEWORK_DIR}/src/eventing
)

add_executable(agent-main ${SOURCES})

target_link_libraries(agent-main
    ${AGENT_COMMANDS_LIB}
    ${AGENT_FRAMEWORK_LIB}
    ${CONFIGURATION_LIBRARIES}
    ${JSONCXX_LIBRARIES}
    ${LOGGER_LIBRARIES}
    ${UUID_LIBRARIES}
    ${PCA95XX_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    pthread
    curl
    microhttpd
    jsonrpccpp-server
    jsonrpccpp-client
    jsonrpccpp-common
    jsoncpp
)
