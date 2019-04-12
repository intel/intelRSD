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
project(Ipmi C CXX)

set(IPMI_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "IPMI include dir path"
    )

set(IPMI_LIBRARIES
    ipmi
    CACHE PATH "IPMI libraries"
    )

include_directories(
    ${COMMON_INCLUDE_DIRS}
    ${LOGGER_INCLUDE_DIRS}
    include
)

include_directories(SYSTEM
    ${IPMITOOL_INCLUDE_DIRS}
    )

add_subdirectory(src)

add_custom_target(ipmi-doc-all
    COMMAND doxygen doxygen.config
    COMMAND ${CMAKE_COMMAND} -E copy_directory doc ${CMAKE_BINARY_DIR}/doc/ipmi
    COMMAND ${CMAKE_COMMAND} -E remove_directory doc
    DEPENDS ipmi
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

add_custom_target(ipmi-doc-clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory doc/ipmi
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )


install(DIRECTORY include/ipmi DESTINATION include)

add_subdirectory(tests)
