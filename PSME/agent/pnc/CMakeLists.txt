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
project("PSME Pnc" C CXX)

if (CMAKE_CROSSCOMPILING)
    message(WARNING "Not supported for crosscompiling")
    return()
endif()

set(AGENT_PNC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(AGENT_PNC_SHARED_LIBRARIES_PATH "/opt/psme/lib/psme-pnc")
set(AGENT_PNC_LIBRARIES -Wl,--whole-archive pnc-commands -Wl,--no-whole-archive -Wl,-rpath,${AGENT_PNC_SHARED_LIBRARIES_PATH})

include_directories(
    ${LOGGER_INCLUDE_DIRS}
    ${DATABASE_INCLUDE_DIRS}
    ${IPMI_INCLUDE_DIRS}
    ${FRU_EEPROM_INCLUDE_DIRS}
    ${SYSFSREF_INCLUDE_DIRS}
    ${AGENT_FRAMEWORK_INCLUDE_DIRS}
    ${AGENT_FRAMEWORK_DB_INCLUDE_DIRS}
    ${COMMON_INCLUDE_DIRS}
    ${CONFIGURATION_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OPAE11PP_INCLUDE_DIRS}
    ${UUID_INCLUDE_DIRS}
    ${UTILS_INCLUDE_DIRS}
)

include_directories(SYSTEM
    ${SAFESTRING_INCLUDE_DIRS}
    ${OPAE11_INCLUDE_DIRS}
)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(examples)
