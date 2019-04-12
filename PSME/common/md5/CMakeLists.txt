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
project(MD5 C CXX)

# TODO: remove old-style flags when all usages are removed
set(MD5_LIBRARIES
    md5
    CACHE PATH "Md5 libraries"
    )

set(MD5_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "Md5 include directory"
    )


add_library(md5 STATIC
    src/md5.c
    )
target_include_directories(md5
    PUBLIC
    include
    )

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_target_properties(md5 PROPERTIES
        COMPILE_FLAGS "-fPIC -Wno-cast-qual -Wno-conversion -Wno-unsafe-loop-optimizations"
        )
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set_target_properties(md5 PROPERTIES
        COMPILE_FLAGS "-fPIC -Wno-cast-qual -Wno-conversion -Wno-cast-align -Wno-reserved-id-macro"
        )
endif ()


