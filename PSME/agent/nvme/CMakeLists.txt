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

cmake_minimum_required(VERSION 3.4)
project("PSME NVMe" C CXX)

if (CMAKE_CROSSCOMPILING)
    message(WARNING "Not supported for crosscompiling")
    return()
endif ()

add_executable(psme-gpt-nvme "")

target_include_directories(psme-gpt-nvme
    SYSTEM PRIVATE
    ${NL3_INCLUDE_DIRS}

    PRIVATE
    ${DATABASE_INCLUDE_DIRS}
    ${AGENT_FRAMEWORK_INCLUDE_DIRS}
    ${NETLINK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(src)

target_link_libraries(psme-gpt-nvme
    PRIVATE
    agent-framework
    partition
    nvme
    ${NVMFPP_LIBRARIES}
    netlink
    ${NET_LIBRARIES}
    telemetry
    ${NL3_LIBRARIES}
    nvme-target
    uuid
    logger
    configuration
    ${SAFESTRING_LIBRARIES}
    pthread
    ${MICROHTTPD_LIBRARIES}
    ${CURL_LIBRARIES}
    udev
    md5
)

add_subdirectory(tests)
