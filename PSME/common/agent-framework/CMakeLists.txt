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
project("PSME Agent Framework" C CXX)

include(Version.cmake)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config/version.hpp.in
  ${CMAKE_CURRENT_SOURCE_DIR}/include/agent-framework/version.hpp
)

set(AGENT_FRAMEWORK_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(AGENT_FRAMEWORK_INCLUDE_DIRS
    ${JSON_INCLUDE_DIRS}
    ${JSON_RPC_INCLUDE_DIRS}
    ${OPTIONAL_INCLUDE_DIRS}
    ${COMMON_INCLUDE_DIRS}
    ${DATABASE_INCLUDE_DIRS}
    ${CONFIGURATION_INCLUDE_DIRS}
    ${LOGGER_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "Agent framework include dir path"
)

set(CODE_COVERAGE_IGNORE
    '**/safe-string/*'
    '**/pca95xx/*'
    '**/json/*'
    '**/logger/*'
)

add_library(agent-framework STATIC
    src/version.cpp
    src/logger_loader.cpp
    src/signal.cpp
    )

target_link_libraries(agent-framework
    PUBLIC
    agent-framework-module
    agent-framework-command
    agent-framework-threading
    agent-framework-registration
    agent-framework-eventing
    agent-framework-tree-stability
    agent-framework-discovery-manager
    agent-framework-validators
    agent-framework-action
    agent-framework-state-machine

    PRIVATE
    pthread
)

target_include_directories(agent-framework
    PUBLIC
    include
)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(examples)

# Modern CMake targets:
add_subdirectory(discovery)
add_subdirectory(database)
