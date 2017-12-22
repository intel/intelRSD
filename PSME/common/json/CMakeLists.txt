# <license_header>
#
# Copyright (c) 2017 Intel Corporation
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
project("JSON" CXX)

set(JSON_LIBRARIES
        json
        CACHE PATH "JSON libraries"
        )

set(JSON_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        CACHE PATH "JSON include directory"
        )

include_directories(
        include
        )

include_directories(SYSTEM
        include
        )

add_library(json STATIC
        src/json-wrapper.cpp
        )

install(TARGETS json
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )

#add_subdirectory(tests)
