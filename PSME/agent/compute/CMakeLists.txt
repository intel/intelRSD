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

cmake_minimum_required(VERSION 2.8)
project("PSME Compute" C CXX)


set(AGENT_COMPUTE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(AGENT_COMPUTE_LIBRARIES
    -Wl,--whole-archive compute-commands -Wl,--no-whole-archive
    agent-framework
    utils
    acpi
)

include_directories(
    ${COMMON_INCLUDE_DIRS}
    ${IPMI_INCLUDE_DIRS}
    ${CONFIGURATION_INCLUDE_DIRS}
    ${LOGGER_INCLUDE_DIRS}
    ${DATABASE_INCLUDE_DIRS}
    ${AGENT_FRAMEWORK_INCLUDE_DIRS}
    ${TELEMETRY_INCLUDE_DIRS}
    ${NETLINK_INCLUDE_DIRS}
    ${SMBIOS_INCLUDE_DIRS}
    ${ACPI_INCLUDE_DIRS}
    ${UTILS_INCLUDE_DIRS}
    ${PMEM_INCLUDE_DIRS}

    ${NET_INCLUDE_DIRS}
    ${GENERIC_MDR_INCLUDE_DIRS}
    ${ISCSI_MDR_INCLUDE_DIRS}

    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include_directories(SYSTEM
    ${SAFESTRING_INCLUDE_DIRS}
    ${UUID_INCLUDE_DIRS}
)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(examples)
