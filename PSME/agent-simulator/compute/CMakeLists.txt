# <license_header>
#
# Copyright (c) 2015-2017 Intel Corporation
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
project("PSME Compute Simulator" C CXX)

if (NOT LIBXMLPP_FOUND)
    message(WARNING "Simulator not compiled:: LIBXML not found (should be Fedora Linux)")
    return()
endif()

set(PSME_COMPUTE_SIMULATOR_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PSME_COMPUTE_SIMULATOR_LIBRARIES
    -Wl,--whole-archive compute-commands-simulator -Wl,--no-whole-archive
)

include_directories(
    ${AGENT_FRAMEWORK_INCLUDE_DIRS}
    ${JSONCXX_INCLUDE_DIRS}
    ${LOGGER_INCLUDE_DIRS}
    ${CONFIGURATION_INCLUDE_DIRS}
    ${COMMON_INCLUDE_DIRS}

    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

include_directories(SYSTEM
    ${SAFESTRING_INCLUDE_DIRS}
    ${UUID_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
    ${JSONRPCCPP_INCLUDE_DIRS}
    ${GLIBMM_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${LIBXMLPP_INCLUDE_DIRS}
)

add_subdirectory(src)
