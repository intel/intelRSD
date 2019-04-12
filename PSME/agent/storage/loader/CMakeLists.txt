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

add_library(storage-tgt-loader STATIC
    src/tgt/storage_loader.cpp
    src/tgt/iscsi_configuration.cpp
)

target_compile_definitions(storage-tgt-loader PRIVATE
    ${TGT_FLAGS}
)

target_link_libraries(storage-tgt-loader
    PUBLIC
    agent-framework
    json
    tgt
)

target_include_directories(storage-tgt-loader
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(storage-nvme-loader STATIC
    src/nvme/storage_loader.cpp
    src/nvme/nvme_configuration.cpp
)

target_include_directories(storage-nvme-loader
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(storage-nvme-loader
    PUBLIC
    agent-framework
    json
)