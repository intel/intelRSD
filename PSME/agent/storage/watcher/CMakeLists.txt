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

set(SOURCES
    src/hotswap_manager.cpp
    src/hotswap_watcher.cpp
    src/ip_watcher.cpp
)

add_library(hotswap-iscsi STATIC
    ${SOURCES})

target_compile_definitions(hotswap-iscsi PRIVATE
    ${TGT_FLAGS}
)

target_include_directories(hotswap-iscsi
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(hotswap-iscsi PUBLIC
    storage-tgt-discovery
)