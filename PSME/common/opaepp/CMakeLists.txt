# <license_header>
#
# Copyright (c) 2018-2019 Intel Corporation
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
project("OPAEPP" CXX)

if (CMAKE_CROSSCOMPILING)
    message(WARNING "Not supported for crosscompiling")
    return()
endif ()

set(OPAEPP_COMMON_SOURCES
    src/device.cpp
    src/opae/opae_cpp_device_reader.cpp
    src/opae/opae_cpp_device_updater.cpp
    )

# For recent OPAE version

add_library(opaepp STATIC
    ${OPAEPP_COMMON_SOURCES}

    src/opae-proxy/opae_proxy_device_reader.cpp
    src/opae-proxy/opae_proxy_host_api.cpp
    src/opae-proxy/opae_proxy_context.cpp
    src/opae-proxy/utils/opae_proxy_host_api_utils.cpp
    )

set(OPAEPP_INCLUDE_DIRS
    ${AGENT_FRAMEWORK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${UUID_INCLUDE_DIRS}
    ${OPAE_INCLUDE_DIRS}
    CACHE PATH "OPAE C++ wrapper include directory"
    )

target_include_directories(opaepp
    SYSTEM PUBLIC
    ${OPAE_INCLUDE_DIRS}
    )

target_include_directories(opaepp
    PRIVATE
    ${LOGGER_INCLUDE_DIRS}

    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(opaepp PRIVATE
    ${OPAE_LIBRARIES}
    agent-framework
    uuid)

target_link_libraries(opaepp PRIVATE
    -luuid)

# For OPAE 1.1

add_library(opae11pp STATIC
    ${OPAEPP_COMMON_SOURCES}
    )


set(OPAE11PP_INCLUDE_DIRS
    ${AGENT_FRAMEWORK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${UUID_INCLUDE_DIRS}
    ${OPAE11_INCLUDE_DIRS}
    CACHE PATH "OPAE 1.1 C++ wrapper include directory"
    )

target_include_directories(opae11pp
    SYSTEM PUBLIC
    ${OPAE11_INCLUDE_DIRS}
    ${SAFESTRING_INCLUDE_DIRS}

    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    PRIVATE
    ${LOGGER_INCLUDE_DIRS}

    )

target_link_libraries(opae11pp PRIVATE
    ${SAFESTRING_LIBRARIES}
    agent-framework
    uuid

    PUBLIC
    ${OPAE11_LIBRARIES}
    )

target_link_libraries(opae11pp PRIVATE
    -luuid)

