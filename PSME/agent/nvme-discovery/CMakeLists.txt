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

cmake_minimum_required(VERSION 3.4)
project("PSME NVMe Discovery" C CXX)

if (CMAKE_CROSSCOMPILING OR "${CMAKE_TARGET_ARCH}" STREQUAL "32")
  message(WARNING "Not supported for arm or 32bit arch")
  return()
endif()


include_directories(
    ${LOGGER_INCLUDE_DIRS}
    ${DATABASE_INCLUDE_DIRS}
    ${NVMFPP_INCLUDE_DIRS}
    ${JSON_INCLUDE_DIRS}
    ${JSON_RPC_INCLUDE_DIRS}
    ${JSONCXX_INCLUDE_DIRS}
    ${AGENT_FRAMEWORK_INCLUDE_DIRS}
    ${CONFIGURATION_INCLUDE_DIRS}
    ${COMMON_INCLUDE_DIRS}
    ${NETLINK_INCLUDE_DIRS}
    ${NVME_INCLUDE_DIRS}
    ${PARTITION_INCLUDE_DIRS}
    ${SYSFSREF_INCLUDE_DIRS}
    ${DATABASE_INCLUDE_DIRS}
    ${NVME_TARGET_INCLUDE_DIRS}
    ${NVMF_DISCOVERY_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include_directories(SYSTEM
    ${SAFESTRING_INCLUDE_DIRS}
    ${UUID_INCLUDE_DIRS}
    ${NL3_INCLUDE_DIRS}
    ${FABRIC_INCLUDE_DIRS}
)

add_subdirectory(src)
add_subdirectory(tests)
