# <license_header>
#
# Copyright (c) 2017-2018 Intel Corporation
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
project("NVME Target" CXX)

set(NVME_TARGET_LIBRARIES
    nvme-target
    CACHE PATH "NVME Target libraries"
    )

set(NVME_TARGET_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "NVME Target include directory"
    )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SYSFSREF_INCLUDE_DIRS}
)

add_library(nvme-target STATIC
    src/base_nvme_target_handler.cpp
    src/sysfs_nvme_target_handler.cpp
)

install(TARGETS nvme-target
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

add_subdirectory(tests)
