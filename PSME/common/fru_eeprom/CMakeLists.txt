# <license_header>
#
# Copyright (c) 2016-2019 Intel Corporation
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
project(FruEeprom CXX)

# TODO: remove old-style flags when all usages are removed

set(FRU_EEPROM_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "FRU EEPROM include directory"
    )

add_library(fru_eeprom STATIC
    src/fru_eeprom_parser.cpp
    )

target_include_directories(fru_eeprom
    SYSTEM PRIVATE
    ${SAFESTRING_INCLUDE_DIRS}

    PUBLIC
    include
    )
target_link_libraries(fru_eeprom
    PRIVATE
    ${SAFESTRING_LIBRARIES}
    )

add_subdirectory(tests)
