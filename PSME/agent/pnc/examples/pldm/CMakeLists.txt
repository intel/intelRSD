# <license_header>
#
# Copyright (c) 2019 Intel Corporation
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
project(pnc-examples C CXX)

add_executable(pldm_commands main.cpp)

add_library(aardvark-add-libs
    $<TARGET_OBJECTS:pnc-tools>
    $<TARGET_OBJECTS:pnc-nvme>
    $<TARGET_OBJECTS:pnc-gas>
    $<TARGET_OBJECTS:pnc-i2c>
    $<TARGET_OBJECTS:pnc-aardvark>
    )


target_link_libraries(aardvark-add-libs PUBLIC
    pnc-discovery
    pnc-pldm
    pnc-smbus)

target_link_libraries(pldm_commands
    agent-framework
    aardvark-add-libs
    ipmi
    ${IPMITOOL_LIBRARIES}
    ${AARDVARK_LIBRARIES}
    utils
    )
