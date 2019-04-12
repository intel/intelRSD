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

set(SOURCES
    # Interface
    switch_port_vlan.cpp
    switch_port_info.cpp
    switch_port_info_impl.cpp
    switch_info.cpp
    switch_info_impl.cpp
    switch_agent.cpp
    remote_switch_info.cpp
    static_mac.cpp
)

set_source_files_properties(
    static_mac.cpp
    COMPILE_FLAGS "-Wno-old-style-cast -Wno-cast-align")

add_library(hal OBJECT ${SOURCES})


if (BUILD_EOS_SDK)
    add_subdirectory(eos_sdk)
endif()

if (BUILD_EOS_EAPI)
    add_subdirectory(eos_eapi)
endif()
