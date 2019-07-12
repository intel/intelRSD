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

set(SOURCES
    chassis_builder.cpp
    drive_builder.cpp
    manager_builder.cpp
    memory_builder.cpp
    memory_domain_builder.cpp
    memory_chunks_builder.cpp
    network_device_builder.cpp
    network_device_function_builder.cpp
    network_interface_builder.cpp
    processor_builder.cpp
    storage_subsystem_builder.cpp
    system_builder.cpp
    trusted_module_builder.cpp
    power_zone_builder.cpp
    thermal_zone_builder.cpp
    pcie_device_builder.cpp
    pcie_function_builder.cpp
    log_service_builder.cpp

    platform_specific/purley/purley_chassis_builder.cpp
    platform_specific/purley/purley_system_builder.cpp
)

add_library(compute-discovery-builders OBJECT ${SOURCES})
