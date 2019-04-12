# <license_header>
#
# Copyright (c) 2018-2019 Intel Corporation
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
project("PersistentMemory" CXX)

# TODO: remove old-style flags when all usages are removed
set(PMEM_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "Pmem include dir path"
    )

add_library(pmem STATIC
    src/dcpmem_parser.cpp
    src/dcpmem_entry_point.cpp
    src/dcpmem_command_entry_point.cpp
    src/memory_topology.cpp
    )

target_include_directories(pmem
    SYSTEM PRIVATE
    ${IPMI_INCLUDE_DIRS}
    ${AGENT_FRAMEWORK_INCLUDE_DIRS}
    PUBLIC
    include
    )

target_link_libraries(pmem
    PRIVATE
    ${IPMI_LIBRARIES}
    PUBLIC
    generic_mdr
    utils
    )
