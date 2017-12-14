# <license_header>
#
# Copyright (c) 2016-2017 Intel Corporation
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
project(Netlink CXX)

if (CMAKE_CROSSCOMPILING)
    message(WARNING "Not supported for crosscompiling.")
    return()
endif()

set(NETLINK_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "Netlink include dir path"
)

set(NETLINK_LIBRARIES
    netlink
    CACHE PATH "Netlink libraries"
)

include_directories(
    ${LOGGER_INCLUDE_DIRS}
    ${AGENT_FRAMEWORK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include_directories(SYSTEM
    ${NL3_INCLUDE_DIRS}
)

add_subdirectory(src)

install (DIRECTORY include/netlink DESTINATION include)
