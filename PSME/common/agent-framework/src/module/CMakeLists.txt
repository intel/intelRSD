# <license_header>
#
# Copyright (c) 2015-2016 Intel Corporation
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
    status.cpp
    fru_info.cpp
    hard_drive.cpp
    hard_drive_partition.cpp
    storage_controller.cpp
    network_interface.cpp
    module.cpp
    module_manager.cpp
    submodule.cpp
    oem_data.cpp
    ipv4_address.cpp
    ipv6_address.cpp
    neighbor_info.cpp
    logical_drive.cpp
    target.cpp
    iscsi_data.cpp
    collection.cpp
    port.cpp
    vlan.cpp
    resources.cpp
    vlanport.cpp
    block_device.cpp
    target_manager.cpp
    chassis.cpp
    chassis_zone.cpp
    compute_zone.cpp
    processor.cpp
    memory.cpp
)

add_library(module OBJECT ${SOURCES})
