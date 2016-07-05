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
project(GoogleMockFramework CXX)

message("::${CMAKE_PREFIX_PATH}")

find_path(gtest_include "gtest/gtest.h")

add_library(gmock STATIC
    src/gmock-cardinalities.cc
    src/gmock-internal-utils.cc
    src/gmock-matchers.cc
    src/gmock-spec-builders.cc
    src/gmock.cc
)

add_library(gmock-main STATIC src/gmock_main.cc)

set_target_properties(gmock PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(gmock-main PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(gmock PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${gtest_include}
)

target_include_directories(gmock-main PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${gtest_include}
)

install (TARGETS gmock gmock-main
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install (DIRECTORY include/gmock DESTINATION include)
