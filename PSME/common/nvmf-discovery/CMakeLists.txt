# <license_header>
#
# Copyright (c) 2018 Intel Corporation
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
project("NVMF Discovery" CXX)

if (CMAKE_CROSSCOMPILING OR "${CMAKE_TARGET_ARCH}" STREQUAL "32")
  message(WARNING "Not supported for arm or 32bit arch")
  return()
endif()

set(NVMF_DISCOVERY_LIBRARIES
    nvmf-discovery
    CACHE PATH "NVMF Discovery libraries"
)

set(NVMF_DISCOVERY_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "NVMF Discovery include directory"
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${NVME_INCLUDE_DIRS}
    ${LOGGER_INCLUDE_DIRS}
    ${COMMON_INCLUDE_DIRS}
    ${UTILS_INCLUDE_DIRS}
)

include_directories(SYSTEM
    ${FABRIC_INCLUDE_DIRS}
)

add_library(nvmf-discovery STATIC
    src/fabric.cpp
    src/connection_listener.cpp
    src/memory_buffer.cpp
    src/fabric_exception.cpp
    src/fabric_endpoint.cpp
    src/discovery_controller.cpp
    src/discovery_entries_provider.cpp
    src/discovery_service.cpp
)

install(TARGETS nvmf-discovery
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

add_subdirectory(tests)
