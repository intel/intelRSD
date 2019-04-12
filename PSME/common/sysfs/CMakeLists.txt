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
project("SYSFSREF" CXX)

# TODO: remove old-style flags when all usages are removed
set(SYSFSREF_LIBRARIES
    sysfsref
    CACHE PATH "Utility libraries"
    )

set(SYSFSREF_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "SYSFS include directory"
    )


add_library(sysfsref STATIC
    src/path.cpp
    src/abstract_sysfs_interface.cpp
    src/sysfs_interface.cpp
    )

target_include_directories(sysfsref
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )


add_subdirectory(tests)

