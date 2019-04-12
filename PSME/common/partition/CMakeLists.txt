# <license_header>
#
# Copyright (c) 2017-2019 Intel Corporation
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
project("Partition" CXX)

add_library(partition STATIC
    src/abstract_drive_interface.cpp
    src/file_drive_interface.cpp
    src/thread_safe_drive_interface_proxy.cpp
    src/abstract_partition_table.cpp
    src/mbr_partition_table.cpp
    src/gpt_partition_table.cpp
)

target_include_directories(partition
    SYSTEM PRIVATE ${SAFESTRING_INCLUDE_DIRS}
    PUBLIC include
)

target_link_libraries(partition
    PRIVATE ${SAFESTRING_LIBRARIES} crc uuid
    PUBLIC utils
)

add_subdirectory(tests)