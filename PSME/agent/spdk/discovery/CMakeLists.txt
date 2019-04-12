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
    src/periodic_discovery_manager.cpp

    src/discoverers/spdk_fabric_discoverer.cpp
    src/discoverers/spdk_drive_discoverer.cpp
    src/discoverers/spdk_volume_discoverer.cpp
    src/discoverers/spdk_storage_pool_discoverer.cpp
    src/discoverers/spdk_endpoint_discoverer.cpp
    src/discoverers/spdk_zone_discoverer.cpp

    src/builders/spdk_volume_builder.cpp
    src/builders/spdk_drive_builder.cpp
    src/builders/spdk_storage_pool_builder.cpp
    src/builders/spdk_endpoint_builder.cpp
)


add_library(spdk-discovery STATIC ${SOURCES})

target_include_directories(spdk-discovery
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(spdk-discovery
    PUBLIC
    spdk-common
    spdk-tree-stability
    agent-framework-discovery
    agent-framework-database
    utils
)

