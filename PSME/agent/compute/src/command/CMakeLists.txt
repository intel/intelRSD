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

add_library(compute-command-sdv OBJECT
    get_chassis_info.cpp
    get_collection.cpp
    get_drive_info.cpp
    get_managers_collection.cpp
    get_tasks_collection.cpp
    get_manager_info.cpp
    get_task_info.cpp
    get_task_result_info.cpp
    get_memory_info.cpp
    get_memory_domain_info.cpp
    get_memory_chunks_info.cpp
    get_network_interface_info.cpp
    get_network_device_info.cpp
    get_network_device_function.cpp
    get_processor_info.cpp
    get_storage_controller_info.cpp
    get_storage_subsystem_info.cpp
    get_system_info.cpp
    set_component_attributes.cpp
    set_network_device_function_attributes.cpp
    set_trusted_module_attributes.cpp
    get_trusted_module_info.cpp
    delete_task.cpp
    get_metric_definition_info.cpp
    get_metric_definitions_collection.cpp
    get_metrics.cpp
    get_fan_info.cpp
    get_power_zone_info.cpp
    get_thermal_zone_info.cpp
    get_pcie_device_info.cpp
    handle_system_power_state.cpp
    handle_dcpmem_erase.cpp
    get_pcie_function_info.cpp
    set_performance_configuration.cpp
    delete_drive.cpp
    get_log_service_info.cpp
    get_log_entry_info.cpp
)

set_psme_command_target_properties(compute-command-sdv)
