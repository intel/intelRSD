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

cmake_minimum_required(VERSION 3.4)
project(Logger C CXX)

# TODO: remove old-style flags when all usages are removed
set(LOGGER_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "Logger include dir path"
)

set(LOGGER_LIBRARIES
    logger
    pthread
    CACHE PATH "Logger libraries"
)

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

target_include_directories(logger
    SYSTEM PRIVATE
    ${SAFESTRING_INCLUDE_DIRS}

    PRIVATE
    src # for PIMPL

    PUBLIC
    include
)

target_link_libraries(logger
    PRIVATE
    ${SAFESTRING_LIBRARIES}

    PUBLIC
    pthread
)

add_subdirectory(tests)