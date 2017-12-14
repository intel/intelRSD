# <license_header>
#
# Copyright (c) 2015-2017 Intel Corporation
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
project(Logger C CXX)

set(LOGGER_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "Logger include dir path"
)

set(LOGGER_LIBRARIES
    logger pthread
    CACHE PATH "Logger libraries"
)

include_directories(src include)

add_library(logger STATIC
    src/logger.c
    src/logger.cpp
    src/logger_options.cpp
    src/logger_factory.cpp
    src/logger_alloc.c
    src/logger_buffer.c
    src/logger_color.c
    src/logger_level.c
    src/logger_list.c
    src/logger_stream.c
    src/logger_stream.cpp
    src/logger_time.c
    src/stream/logger_stream_config.c
    src/stream/logger_stream_file.c
    src/stream/logger_stream_socket.c
    src/stream/logger_stream_standard.c
)

add_subdirectory(tests)

add_custom_target(logger-doc-all
    COMMAND doxygen doxygen.config
    COMMAND ${CMAKE_COMMAND} -E copy_directory doc ${CMAKE_BINARY_DIR}/doc/logger
    COMMAND ${CMAKE_COMMAND} -E remove_directory doc
    DEPENDS logger
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(logger-doc-clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory doc/logger
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

if (CMAKE_C_COMPILER_ID MATCHES GNU|Clang)
    set_source_files_properties(
        src/logger.c
        src/logger_alloc.c
        src/logger_buffer.c
        src/logger_color.c
        src/logger_level.c
        src/logger_list.c
        src/logger_stream.c
        src/logger_time.c
        src/stream/logger_stream_config.c
        src/stream/logger_stream_file.c
        src/stream/logger_stream_socket.c
        src/stream/logger_stream_standard.c
        PROPERTIES
        COMPILE_FLAGS "-std=gnu11"
    )
endif()

include_directories(SYSTEM
    ${SAFESTRING_INCLUDE_DIRS}
)

target_link_libraries(logger
    ${SAFESTRING_LIBRARIES}
)

install (TARGETS logger
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install (DIRECTORY include/logger DESTINATION include)
