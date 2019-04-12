# <license_header>
#
# Copyright (c) 2016-2019 Intel Corporation
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

add_library(pnc-commands STATIC
    get_manager_info.cpp
    get_collection.cpp
    get_tasks_collection.cpp
    get_chassis_info.cpp
    get_managers_collection.cpp
    get_drive_info.cpp
    get_fabric_info
    get_switch_info.cpp
    get_endpoint_info.cpp
    get_port_info.cpp
    get_zone_info.cpp
    get_pcie_device_info.cpp
    get_pcie_function_info.cpp
    get_storage_subsystem_info.cpp
    get_system_info.cpp
    get_task_info.cpp
    get_task_result_info.cpp
    add_endpoint.cpp
    add_zone.cpp
    delete_endpoint.cpp
    delete_zone.cpp
    add_zone_endpoints.cpp
    delete_zone_endpoints.cpp
    delete_task.cpp
    set_component_attributes.cpp
    get_metric_definition_info.cpp
    get_metric_definitions_collection.cpp
    get_metrics.cpp
    get_processor_info.cpp
    delete_drive.cpp)

target_link_libraries(pnc-commands PRIVATE
    agent-framework-discovery
)

set_psme_command_target_properties(pnc-commands)
