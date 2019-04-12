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

cmake_minimum_required(VERSION 3.4)
project(UUID C CXX)

# TODO: remove old-style flags when all usages are removed
set(UUID_LIBRARIES
    uuid
    ${SAFESTRING_LIBRARIES}
    CACHE PATH "UUID libraries"
)

set(UUID_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "UUID include directory"
)

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(uuid-1.6.2/uuid.c
        PROPERTIES COMPILE_FLAGS "-Wno-aggregate-return"
    )
    set_source_files_properties(uuid-1.6.2/uuid_ui64.c
        PROPERTIES COMPILE_FLAGS "-Wno-aggregate-return"
    )
    set_source_files_properties(uuid-1.6.2/uuid_ui128.c
        PROPERTIES COMPILE_FLAGS "-Wno-aggregate-return"
    )
    set_source_files_properties(uuid-1.6.2/uuid_md5.c
        PROPERTIES COMPILE_FLAGS "-Wno-unsafe-loop-optimizations"
    )
    set_source_files_properties(uuid-1.6.2/uuid_mac.c
        PROPERTIES COMPILE_FLAGS "-Wno-strict-aliasing"
    )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set_source_files_properties(uuid-1.6.2/uuid++.cc
        PROPERTIES COMPILE_FLAGS "-Wno-reserved-id-macro -Wno-extra-semi -Wno-deprecated -Wno-old-style-cast"
    )
    set_source_files_properties(uuid-1.6.2/uuid_mac.c
        PROPERTIES COMPILE_FLAGS "-Wno-reserved-id-macro -Wno-disabled-macro-expansion"
    )
    set_source_files_properties(uuid-1.6.2/uuid_md5.c
        PROPERTIES COMPILE_FLAGS "-Wno-reserved-id-macro -Wno-disabled-macro-expansion"
    )
    set_source_files_properties(uuid-1.6.2/uuid_str.c
        PROPERTIES COMPILE_FLAGS "-Wno-reserved-id-macro -Wno-disabled-macro-expansion -Wno-double-promotion"
    )
    set_source_files_properties(uuid-1.6.2/uuid_time.c
        PROPERTIES COMPILE_FLAGS "-Wno-disabled-macro-expansion -Wno-reserved-id-macro -Wno-implicit-function-declaration"
    )
    set_source_files_properties(uuid-1.6.2/uuid.c
        PROPERTIES COMPILE_FLAGS "-Wno-reserved-id-macro -Wno-disabled-macro-expansion"
    )
    set_source_files_properties(uuid-1.6.2/uuid_prng.c
        PROPERTIES COMPILE_FLAGS "-Wno-reserved-id-macro -Wno-disabled-macro-expansion"
    )
    set_source_files_properties(uuid-1.6.2/uuid_sha1.c
        PROPERTIES COMPILE_FLAGS "-Wno-reserved-id-macro -Wno-disabled-macro-expansion"
    )
    set_source_files_properties(uuid-1.6.2/uuid_ui128.c
        PROPERTIES COMPILE_FLAGS "-Wno-reserved-id-macro -Wno-disabled-macro-expansion -Wno-conditional-uninitialized -Wno-conversion"
    )
    set_source_files_properties(uuid-1.6.2/uuid_ui64.c
        PROPERTIES COMPILE_FLAGS "-Wno-reserved-id-macro -Wno-disabled-macro-expansion -Wno-conditional-uninitialized -Wno-conversion"
    )
endif()

add_library(uuid STATIC
    uuid-1.6.2/uuid++.cc
    uuid-1.6.2/uuid.c
    uuid-1.6.2/uuid_mac.c
    uuid-1.6.2/uuid_md5.c
    uuid-1.6.2/uuid_prng.c
    uuid-1.6.2/uuid_sha1.c
    uuid-1.6.2/uuid_str.c
    uuid-1.6.2/uuid_time.c
    uuid-1.6.2/uuid_ui128.c
    uuid-1.6.2/uuid_ui64.c
    src/uuid.cpp
)

target_include_directories(uuid
    SYSTEM PRIVATE
    ${SAFESTRING_INCLUDE_DIRS}

    PRIVATE
    uuid-1.6.2

    PUBLIC
    include
)

target_link_libraries(uuid
    PRIVATE
    ${SAFESTRING_LIBRARIES}
)
