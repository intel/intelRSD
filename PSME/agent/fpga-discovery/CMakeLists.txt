# <license_header>
#
# Copyright (c) 2019 Intel Corporation
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
project("PSME FPGA Discovery" C CXX)

if (CMAKE_CROSSCOMPILING OR "${CMAKE_TARGET_ARCH}" STREQUAL "32")
  message(WARNING "Not supported for arm or 32bit arch")
  return()
endif()

add_executable(psme-fpga-discovery "")

target_include_directories(psme-fpga-discovery
    SYSTEM PRIVATE

    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(src)

target_link_libraries(psme-fpga-discovery
    PRIVATE
    agent-framework
    agent-framework-database
    agent-framework-discovery
    )