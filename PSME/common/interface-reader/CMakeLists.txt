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
project("Interface Reader" CXX)

if (CMAKE_CROSSCOMPILING)
    message(WARNING "Not supported for crosscompiling.")
    return()
endif()

add_library(udev-interface-reader STATIC
    src/base_interface_reader.cpp
    src/udev_interface_reader.cpp
)

target_include_directories(udev-interface-reader
    PUBLIC
    include
)

target_link_libraries(udev-interface-reader
    PRIVATE
    net-udev
    logger
)


add_library(single-interface-reader STATIC
    src/base_interface_reader.cpp
    src/single_interface_reader.cpp
)

target_include_directories(single-interface-reader
    PUBLIC
    include
)

target_link_libraries(single-interface-reader
    PRIVATE
    net
    logger
)