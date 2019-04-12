# <license_header>
#
# Copyright (c) 2017-2019 Intel Corporation
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
project(IscsiMdr CXX)


set(ISCSI_MDR_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        CACHE PATH "iSCSI MDR include directory"
        )

set(ISCSI_MDR_LIBRARIES
        iscsi_mdr ipmi
        CACHE PATH "iSCSI MDR libraries"
)

include_directories(
        ${GENERIC_MDR_INCLUDE_DIRS}
        ${IPMI_INCLUDE_DIRS}
        include
        )

include_directories(SYSTEM
        ${SAFESTRING_INCLUDE_DIRS}
        )

add_library(iscsi_mdr STATIC
        src/iscsi_mdr_parser.cpp
        src/iscsi_structs_ostream.cpp
        src/iscsi_mdr_entry_point.cpp
        src/iscsi_mdr_10_entry_point.cpp
        src/iscsi_mdr_builder_base.cpp
        src/iscsi_mdr_attempt_builder.cpp
        src/iscsi_mdr_version_builder.cpp
        )

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(
         src/iscsi_mdr_builder_base.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-unsafe-loop-optimizations"
    )
endif()

target_link_libraries(iscsi_mdr
	generic_mdr
	)

install(TARGETS iscsi_mdr
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )

add_subdirectory(tests)
