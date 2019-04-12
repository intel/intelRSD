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
project("LVM" CXX)

if (CMAKE_CROSSCOMPILING)
    message(WARNING "Not supported for crosscompiling.")
    return()
endif()

add_library(lvm STATIC
    src/lvm_api.cpp
    src/lvm_attribute.cpp
    src/lvm_clone_task.cpp
    src/lvm_erase_task.cpp
    src/lvm_discovery.cpp
    src/lvm_api_impl.cpp

    src/model/logical_volume.cpp
    src/model/volume_group.cpp
    src/model/physical_volume.cpp
    src/model/creation_data.cpp
)

target_include_directories(lvm
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    PRIVATE
    src # PIMPL
)

target_link_libraries(lvm
    PRIVATE
    ${LVM2APP_LIBRARIES}
    ${LVM2DEVMAPPER_LIBRARIES}
    logger
)

add_subdirectory(examples)
