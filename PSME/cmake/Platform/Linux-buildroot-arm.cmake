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

# handle absolute and relative paths in BUILDROOT_DIR
if ("$ENV{BUILDROOT_DIR}" STREQUAL "")
    set(BUILDROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../../tools/buildroot)
else()
    string(SUBSTRING "$ENV{BUILDROOT_DIR}" 0 1 out)
    if (out STREQUAL "/")
        set(BUILDROOT_DIR "$ENV{BUILDROOT_DIR}")
    else()
        set(BUILDROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../../$ENV{BUILDROOT_DIR})
    endif()
endif()

if (NOT EXISTS ${BUILDROOT_DIR})
    message(FATAL_ERROR "Buildroot ${BUILDROOT_DIR} directory doesn't exist")
endif()

set(CMAKE_C_COMPILER
    ${BUILDROOT_DIR}/output/host/usr/bin/arm-linux-gcc
)
set(CMAKE_CXX_COMPILER
    ${BUILDROOT_DIR}/output/host/usr/bin/arm-linux-g++
)

set(CMAKE_SYSTEM_NAME       Linux)
set(CMAKE_SYSTEM_VERSION    1)

set(CMAKE_FIND_ROOT_PATH
    ${CMAKE_FIND_ROOT_PATH}
    ${CMAKE_BINARY_DIR}
    ${BUILDROOT_DIR}/output/staging
)

# Search in host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# Search libraries and includes in CMAKE_ROOT_PATH only:
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
