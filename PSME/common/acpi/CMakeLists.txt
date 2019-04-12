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
project("ACPI" CXX)

# TODO: remove after switching to modern cmake in compute agent
set(ACPI_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "Acpi include directory"
    )

add_library(acpi STATIC
    src/acpi_parser.cpp
    src/acpi_entry_point.cpp
    src/acpi_table_entry_point.cpp
    )

target_include_directories(acpi
    PUBLIC
    include
    )

target_link_libraries(acpi
    PUBLIC
    generic_mdr
    utils
    )
