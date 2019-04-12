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
cmake_minimum_required(VERSION 3.4)


set(SOURCES
    src/discovery_manager.cpp
    src/discoverers/storage_drive_discoverer.cpp
    src/discoverers/lvm_volume_discoverer.cpp
    src/discoverers/lvm_storage_pool_discoverer.cpp

    src/builders/storage_drive_builder.cpp
    src/builders/lvm_volume_builder.cpp
    src/builders/lvm_storage_pool_builder.cpp
)

set(ISCSI_SOURCES
    src/discoverers/iscsi/storage_endpoint_discoverer.cpp
    src/discoverers/iscsi/storage_fabric_discoverer.cpp
    src/discoverers/iscsi/storage_zone_discoverer.cpp
)

set(NVME_SOURCES
    src/discoverers/nvme/storage_zone_discoverer.cpp
    src/discoverers/nvme/storage_fabric_discoverer.cpp
    src/discoverers/nvme/storage_endpoint_discoverer.cpp
)

add_library(storage-tgt-discovery STATIC
    ${SOURCES}
    ${ISCSI_SOURCES}
)
target_compile_definitions(storage-tgt-discovery PRIVATE
    ${TGT_FLAGS}
)
target_include_directories(storage-tgt-discovery
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(storage-nvme-discovery STATIC
    ${SOURCES}
    ${NVME_SOURCES}
)
target_include_directories(storage-nvme-discovery
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(storage-tgt-discovery
    PUBLIC
    storage-tgt-common
    storage-stability
    agent-framework-discovery
    agent-framework-database
    utils
)

target_link_libraries(storage-nvme-discovery
    PUBLIC
    storage-nvme-common
    storage-stability
    agent-framework-discovery
    agent-framework-database
    utils
)