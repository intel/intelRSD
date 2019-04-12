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
project(ssdp CXX)

# TODO: remove old-style flags when all usages are removed
set(SSDP_LIBRARIES
    ssdp
    CACHE PATH "ssdp libraries"
    )

set(SSDP_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "ssdp include directory"
    )


if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(
        src/ssdp_packet.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-unsafe-loop-optimizations"
    )
    set_source_files_properties(
        src/message_queue.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-logical-op"
    )
endif ()

add_library(ssdp
    STATIC
    src/ssdp_packet.cpp
    src/ssdp_packet_validator.cpp
    src/ssdp_packet_factory.cpp
    src/ssdp_packet_factory_impl.cpp
    src/ssdp_packet_parser.cpp
    src/ssdp_service.cpp
    src/ssdp_service_impl.cpp
    src/message_queue.cpp
    src/uri.cpp
    src/notification.cpp
    )

target_include_directories(ssdp
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
target_link_libraries(ssdp
    PRIVATE logger
    PUBLIC net
    )
add_subdirectory(tests)
