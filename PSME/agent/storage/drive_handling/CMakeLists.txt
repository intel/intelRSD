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
    src/base_drive_reader.cpp
    src/base_drive_handler_factory.cpp
    src/base_drive_handler.cpp
)

set(NVME_SOURCES
    src/nvme_drive_reader.cpp
    src/nvme_drive_handler_factory.cpp
    src/non_functional_drive_handler.cpp
    src/single_namespace_drive_handler.cpp
    src/single_namespace_non_nvme_drive_handler.cpp

    src/utils/nvme/tools.cpp
)

set(TGT_SOURCES
    src/local_drive_reader.cpp
    src/local_drive_handler_factory.cpp
    src/local_drive_handler.cpp
)

add_library(drive-handling-tgt STATIC
    ${SOURCES}
    ${TGT_SOURCES}
)
target_compile_definitions(drive-handling-tgt PRIVATE
    ${TGT_FLAGS}
)

add_library(drive-handling-nvme STATIC
    ${SOURCES}
    ${NVME_SOURCES}
)

target_include_directories(drive-handling-tgt
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(drive-handling-nvme
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(drive-handling-tgt
    PUBLIC
    storage-ioctl
    sysfsref
)

target_link_libraries(drive-handling-nvme
    PUBLIC
    agent-framework-module # TODO: just optional needed
    nvme
    nvme-target
)