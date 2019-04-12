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
project("UTILS" CXX)

# TODO: remove after switching to modern cmake in compute agent
set(UTILS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "Utils include directory"
    )

add_library(utils STATIC
    src/byte_buffer.cpp
    src/byte_swap.cpp
    src/conversion.cpp
    src/crypt_utils.cpp
    src/hex_dump.cpp
    src/string_utils.cpp
    src/transaction/default_transaction_handler.cpp
    src/transaction/transaction_handler_factory.cpp
)

target_include_directories(utils
    PUBLIC include
    ${GCRYPT_INCLUDE_DIRS}
)

target_link_libraries(utils
    PRIVATE
    logger
    gcrypt
)

add_subdirectory(tests)
