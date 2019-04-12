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

set(COMPILER_PREFIX arm-linux-gnueabihf)
set(COMPILER_NAME gcc-linaro-4.9-2014.11-x86_64_${COMPILER_PREFIX})
set(COMPILER_URL
    http://releases.linaro.org/14.11/components/toolchain/binaries/arm-linux-gnueabihf
)

set(COMPILER_PACKAGE ${COMPILER_NAME}.tar.xz)
set(TOOLS_DIR ${CMAKE_CURRENT_LIST_DIR}/../../tools)
set(DOWNLOAD_DIR ${CMAKE_CURRENT_LIST_DIR}/../../third_party)
set(COMPILER_DIR ${TOOLS_DIR}/${COMPILER_NAME})

file(MAKE_DIRECTORY ${TOOLS_DIR})
file(MAKE_DIRECTORY ${DOWNLOAD_DIR})

if (NOT EXISTS ${DOWNLOAD_DIR}/${COMPILER_PACKAGE})
    message(STATUS "Downloading ARM toolchain...")
    file(DOWNLOAD
        ${COMPILER_URL}/${COMPILER_PACKAGE}
        ${DOWNLOAD_DIR}/${COMPILER_PACKAGE}
        SHOW_PROGRESS
    )
endif()

if (NOT EXISTS ${TOOLS_DIR}/${COMPILER_NAME})
    message(STATUS "Unpacking ARM toolchain...")
    execute_process(COMMAND
        ${CMAKE_COMMAND} -E tar xvf ${DOWNLOAD_DIR}/${COMPILER_PACKAGE}
        WORKING_DIRECTORY ${TOOLS_DIR}
    )
endif()

set(CMAKE_C_COMPILER    ${COMPILER_DIR}/bin/${COMPILER_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER  ${COMPILER_DIR}/bin/${COMPILER_PREFIX}-g++)

set(CMAKE_SYSTEM_NAME       Linux)
set(CMAKE_SYSTEM_VERSION    1)

set(CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH} ${CMAKE_BINARY_DIR} ${COMPILER_DIR}/libc)

# Search in host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# Search libraries and includes in CMAKE_ROOT_PATH only:
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
