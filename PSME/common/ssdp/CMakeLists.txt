# <license_header>
#
# Copyright (c) 2015-2017 Intel Corporation
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
project(ssdp CXX)

set(SSDP_LIBRARIES
    ssdp
    CACHE PATH "ssdp libraries"
)

set(SSDP_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "ssdp include directory"
)

include_directories(
    include
)

include_directories(
    ${LOGGER_INCLUDE_DIRS}
    ${NET_INCLUDE_DIRS}
)

set(SRC
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

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(
        src/ssdp_packet.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-unsafe-loop-optimizations"
    )
endif()

add_library(ssdp STATIC ${SRC})

install(TARGETS ssdp
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

#add_subdirectory(examples)
add_subdirectory(tests)
