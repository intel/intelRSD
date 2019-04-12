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
project("AgentFrameworkDatabase" CXX)

add_library(agent-framework-database STATIC
    src/database_keys.cpp
)

set(AGENT_FRAMEWORK_DB_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "Agent framework db include dir path"
    )

target_include_directories(agent-framework-database
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(agent-framework-database
    PUBLIC
    agent-framework-module # TODO: only model from framework?
    database
)