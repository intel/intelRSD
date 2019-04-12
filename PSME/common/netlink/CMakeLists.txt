# <license_header>
#
# Copyright (c) 2016-2019 Intel Corporation
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
project(Netlink CXX)

if (CMAKE_CROSSCOMPILING)
    message(WARNING "Not supported for crosscompiling.")
    return()
endif()

# TODO: remove old-style flags when all usages are removed
set(NETLINK_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "Netlink include dir path"
)

add_library(netlink STATIC
    src/nl_socket.cpp
    src/nl_message.cpp
    src/nl_exception.cpp
    src/nl_exception_invalid_input.cpp
    src/nl_exception_invalid_ifname.cpp
    src/rt_message.cpp
    src/neigh_message.cpp
    src/fdb_info_message.cpp
    src/utils.cpp
)

target_include_directories(netlink
    PUBLIC
    include
)

target_include_directories(netlink SYSTEM
    PRIVATE
    ${NL3_INCLUDE_DIRS}
)

target_link_libraries(netlink
    PRIVATE
    logger
)
