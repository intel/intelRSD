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
project("PSME Agent Framework" C CXX)

include(Version.cmake)

configure_file (
  ${CMAKE_CURRENT_SOURCE_DIR}/config/version.hpp.in
  ${CMAKE_CURRENT_SOURCE_DIR}/include/agent-framework/version.hpp
)

set(AGENT_COMMANDS_LIB -Wl,-whole-archive -Wl,-no-whole-archive)
set(AGENT_FRAMEWORK_LIB agent-framework logger)
set(AGENT_FRAMEWORK_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(AGENT_FRAMEWORK_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "Agent framework include dir path"
)

set(AGENT_FRAMEWORK_LIBRARIES
    -Wl,--whole-archive -Wl,--no-whole-archive agent-framework
    CACHE PATH "Agent framework libraries"
)

set(CODE_COVERAGE_IGNORE
    '**/safe-string/*'
    '**/pca95xx/*'
    '**/json-cxx/*'
    '**/logger/*'
    '**/jsonrpccpp/*'
)

include_directories(
    ${PCA95XX_INCLUDE_DIRS}
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
    ${MD5_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
    ${JSONRPCCPP_INCLUDE_DIRS}
)

add_subdirectory(src)
add_subdirectory(tests)
#add_subdirectory(examples)
