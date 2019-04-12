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
project("PSME Hybrid Storage" C CXX)

if (CMAKE_CROSSCOMPILING)
    message(WARNING "Not supported for crosscompiling.")
    return()
endif ()

set(AGENT_STORAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TGT_FLAGS -DTGT_STORAGE)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/storage_config.hpp.in
    ${CMAKE_CURRENT_SOURCE_DIR}/common/include/storage_config.hpp
)

set(SOURCES
    main.cpp
)

add_executable(psme-lvm-nvme ${SOURCES})
add_executable(psme-lvm-iscsi ${SOURCES})

target_compile_definitions(psme-lvm-iscsi PRIVATE
    ${TGT_FLAGS}
)

generate_default_config_file(
    main.cpp
    agent
    storage
    "AGENT_STORAGE_CONFIGURATION_FILE"
    ./default_configuration.hpp
    .
)

set(AGENT_STORAGE_HYBRID_LIBRARIES
    configuration
    database
    logger
)

target_link_libraries(psme-lvm-nvme
    storage-nvme-discovery
    -Wl,--whole-archive  storage-nvme-commands -Wl,--no-whole-archive
    ${AGENT_STORAGE_HYBRID_LIBRARIES}
)

target_link_libraries(psme-lvm-iscsi
    storage-tgt-discovery
    hotswap-iscsi
    -Wl,--whole-archive storage-tgt-commands -Wl,--no-whole-archive
    ${AGENT_STORAGE_HYBRID_LIBRARIES}
)


add_subdirectory(tree_stability)
add_subdirectory(common)
add_subdirectory(command)
add_subdirectory(loader)
add_subdirectory(ioctl)
add_subdirectory(drive_handling)
add_subdirectory(discovery)
add_subdirectory(watcher)

