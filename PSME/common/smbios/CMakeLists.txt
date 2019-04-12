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
project(Smbios CXX)

set(SMBIOS_LIBRARIES
	generic_mdr
	logger
        )

set(SMBIOS_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        CACHE PATH "Smbios include directory"
        )

include_directories(
	${GENERIC_MDR_INCLUDE_DIRS}
	${LOGGER_INCLUDE_DIRS}
        include
        )

include_directories(SYSTEM
        ${SAFESTRING_INCLUDE_DIRS}
        )

add_library(smbios STATIC
        src/smbios_parser.cpp
        src/smbios_entry_point.cpp
        src/smbios_21_entry_point.cpp
        src/smbios_30_entry_point.cpp
        src/utilities/conversions.cpp
        )

target_link_libraries(smbios
	${SMBIOS_LIBRARIES}
	logger
	)

install(TARGETS smbios
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )

add_subdirectory(tests)
