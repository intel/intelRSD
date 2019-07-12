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

set(SOURCES
    src/discovery_manager.cpp

    src/discoverers/fpgaof_fabric_discoverer.cpp
    src/discoverers/fpgaof_endpoint_discoverer.cpp
    src/discoverers/fpgaof_zone_discoverer.cpp
    src/discoverers/fpgaof_system_discoverer.cpp
    src/discoverers/fpgaof_chassis_discoverer.cpp
    src/discoverers/fpgaof_manager_discoverer.cpp
    src/discoverers/fpgaof_processor_discoverer.cpp
    src/discoverers/fpgaof_opaeproxy_interface_discoverer.cpp

    src/builders/fpgaof_endpoint_builder.cpp
    src/builders/fpgaof_system_builder.cpp
    src/builders/fpgaof_chassis_builder.cpp
    src/builders/fpgaof_manager_builder.cpp
    src/builders/fpgaof_processor_builder.cpp
    src/builders/fpgaof_opaeproxy_interface_builder.cpp)


add_library(fpgaof-discovery STATIC ${SOURCES})

target_include_directories(fpgaof-discovery
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(fpgaof-discovery
    PUBLIC
    fpgaof-common
    fpgaof-utils
    fpgaof-tree-stability
    agent-framework-discovery
    agent-framework-database
    utils
    PRIVATE
    net
)
