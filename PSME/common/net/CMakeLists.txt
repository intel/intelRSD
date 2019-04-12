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

cmake_minimum_required(VERSION 2.8)
project(net CXX)

set(NET_LIBRARIES
    net
    CACHE PATH "net libraries"
    )

set(NET_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "net include directory"
    )

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(
        src/socket_address.cpp
        PROPERTIES COMPILE_FLAGS "-Wstrict-aliasing=3"
    )
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(
        src/socket.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-unsafe-loop-optimizations"
    )
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set_source_files_properties(
        src/network_change_notifier_impl.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-cast-align -Wno-old-style-cast -Wno-unreachable-code"
    )
    set_source_files_properties(
        src/socket_impl.cpp
        src/socket.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast -Wno-unreachable-code"
    )
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(
        src/network_change_notifier_impl.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-cast-align -Wstrict-aliasing=3 -Wno-logical-op"
    )
endif ()

if (CMAKE_CROSSCOMPILING)
    if (CMAKE_CXX_COMPILER_ID MATCHES Clang|GNU)
        set_source_files_properties(
            src/ipaddress.cpp
            src/network_interface.cpp
            PROPERTIES COMPILE_FLAGS "-Wno-cast-align"
        )
    endif ()
    if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
        set_source_files_properties(
            src/socket_address.cpp
            PROPERTIES COMPILE_FLAGS "-Wno-cast-align"
        )
    endif ()
    if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
        set_source_files_properties(
            src/socket_address.cpp
            PROPERTIES COMPILE_FLAGS "-Wno-cast-align -Wstrict-aliasing=3"
        )
    endif ()
endif ()

add_library(net STATIC
    src/network_interface.cpp
    src/ipaddress.cpp
    src/socket_address.cpp
    src/socket.cpp
    src/socket_impl.cpp
    src/datagram_socket.cpp
    src/datagram_socket_impl.cpp
    src/stream_socket.cpp
    src/server_socket.cpp
    src/stream_socket_impl.cpp
    src/multicast_socket.cpp
    src/paired_socket.cpp
    src/net_exception.cpp
    src/network_change_notifier.cpp
    src/network_change_notifier_impl.cpp)

target_include_directories(net
    PRIVATE
    ${SAFESTRING_INCLUDE_DIRS}

    PUBLIC
    include
    )

target_link_libraries(net
    PRIVATE
    logger
    )


#add_subdirectory(examples)
add_subdirectory(tests)
