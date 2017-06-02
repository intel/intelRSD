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

set(SOURCES
        manager_and_chassis.cpp
        system.cpp
        bios.cpp
        processor.cpp
        memory.cpp
        network.cpp
        network_device_and_function.cpp
        cable_id.cpp)

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(system.cpp
            PROPERTIES COMPILE_FLAGS "-Wno-unsafe-loop-optimizations"
            )
endif()

add_library(compute-discovery-basic OBJECT ${SOURCES})
