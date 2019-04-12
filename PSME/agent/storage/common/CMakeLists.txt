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
    src/storage_agent_context.cpp
    src/utils/utils.cpp
)

set(ISCSI_SOURCES
    src/utils/iscsi/utils.cpp
    src/utils/iscsi/iscsi_errors.cpp
)

add_library(storage-tgt-common STATIC
    ${SOURCES}
    ${ISCSI_SOURCES}
)

target_include_directories(storage-tgt-common
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(storage-tgt-common PRIVATE
    ${TGT_FLAGS}
)

target_link_libraries(storage-tgt-common
    PUBLIC
    storage-tgt-loader
    drive-handling-tgt
    agent-framework-database
    agent-framework
    single-interface-reader
    sysfsref
    lvm
)

add_library(storage-nvme-common STATIC
    ${SOURCES})

target_include_directories(storage-nvme-common
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(storage-nvme-common
    PUBLIC
    storage-nvme-loader
    drive-handling-nvme
    agent-framework
    udev-interface-reader
    sysfsref
    lvm
)