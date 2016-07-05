# <license_header>
#
# Copyright (c) 2015-2016 Intel Corporation
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
project("SafeString" C CXX)

add_library(safe-string STATIC
    src/abort_handler_s.c
    src/ignore_handler_s.c
    src/memcpy_s.c
    src/strncpy_s.c
    src/strnlen_s.c
    src/safe_mem_constraint.c
    src/safe_str_constraint.c
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include/safe-string
)

target_include_directories(safe-string
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_definitions(-DSTDC_HEADERS)

set_target_properties(safe-string PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(SAFESTRING_COMPILE_FLAGS
        -std=gnu11
        -Wno-unused-parameter
        -Wno-suggest-attribute=noreturn
        -Wno-pedantic
        -Wno-conversion
        -Wno-sign-conversion
    )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set(SAFESTRING_COMPILE_FLAGS
        -std=gnu11
        -Wno-reserved-id-macro
        -Wno-missing-noreturn
        -Wno-unused-parameter
        -Wno-extra-semi
        -Wno-sign-conversion
        -Wno-disabled-macro-expansion
    )
endif()

string(REGEX REPLACE ";" " " SAFESTRING_COMPILE_FLAGS "${SAFESTRING_COMPILE_FLAGS}")

set_source_files_properties(
    src/abort_handler_s.c
    src/ignore_handler_s.c
    src/memcpy_s.c
    src/strncpy_s.c
    src/strnlen_s.c
    src/safe_mem_constraint.c
    src/safe_str_constraint.c
    PROPERTIES
    COMPILE_FLAGS ${SAFESTRING_COMPILE_FLAGS}
)

install(DIRECTORY include/safe-string DESTINATION include)

install(TARGETS
    safe-string
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
