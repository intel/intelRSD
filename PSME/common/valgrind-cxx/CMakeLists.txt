# <license_header>
#
# Copyright (c) 2015-2016 Intel Corporation
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

cmake_minimum_required(VERSION 2.8)
project("Valgrind C++11" CXX)

include(CheckIncludeFileCXX)

set(VALGRINDCXX_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "VALGRINDCXX include dir path"
)

set(VALGRINDCXX_LIBRARIES
    valgrind-cxx
    CACHE PATH "VALGRINDCXX libraries"
)

add_library(valgrind-cxx STATIC
    src/thread.cpp
)

include_directories(src include)

target_include_directories(valgrind-cxx PUBLIC include)

check_include_file_cxx("valgrind/drd.h" HAVE_VALGRIND)

add_definitions(-DENABLE_VALGRIND_DRD)

set_target_properties(valgrind-cxx PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set_source_files_properties(
        src/thread.cpp
        PROPERTIES
        COMPILE_FLAGS "-Wno-missing-prototypes"
    )
endif()
