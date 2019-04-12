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
project(Base64 CXX)

# TODO: remove old-style flags when all usages are removed

set(BASE64_LIBRARIES
    base64
    CACHE PATH "Base64 libraries"
    )

set(BASE64_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "Base64 include directory"
    )

add_library(base64 STATIC
    src/base64.cpp
    )

target_include_directories(base64
    PUBLIC
    include
    )

if (CMAKE_CXX_COMPILER_ID MATCHES GNU|Clang)
    set_source_files_properties(
        src/base64.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-conversion"
    )
endif ()