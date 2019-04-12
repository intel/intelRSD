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
project("TGT" C CXX)

if (CMAKE_CROSSCOMPILING)
    message(WARNING "Not supported for crosscompiling.")
    return()
endif()

add_library(tgt STATIC
    src/manager.cpp
    src/request.cpp
    src/response.cpp
    src/target_data.cpp
    src/target_parser.cpp
    src/tokenizer.cpp
    src/iscsi_data.cpp

    src/config/tgt_config.cpp
    src/config/tgt_target_config.cpp
    src/config/tgt_target_lun_config.cpp
)

target_include_directories(tgt
    PRIVATE
    ${SAFESTRING_INCLUDE_DIRS}
    ${NET_INCLUDE_DIRS}
    ${LOGGER_INCLUDE_DIRS}

    PUBLIC
    include
)

target_link_libraries(tgt
    PRIVATE
    ${SAFESTRING_LIBRARIES}
    ${NET_LIBRARIES}
    ${LOGGER_LIBRARIES}
)