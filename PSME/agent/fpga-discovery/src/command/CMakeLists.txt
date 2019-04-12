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

target_sources(psme-fpga-discovery PRIVATE
    src/command/get_managers_collection.cpp
    src/command/get_manager_info.cpp
    src/command/get_collection.cpp
#    src/command/get_chassis.info.cpp
#    src/command/get_system_info.cpp
    src/command/get_fabric_info.cpp
    src/command/get_endpoint_info.cpp
    src/command/get_zone_info.cpp
    src/command/add_zone.cpp
    src/command/add_endpoint.cpp
    src/command/delete_endpoint.cpp
    src/command/delete_zone.cpp
    src/command/delete_zone_endpoints.cpp
    src/command/add_zone_endpoints.cpp
#    src/command/set_component_attributes.cpp
#    src/command/set_chassis_attributes.cpp
#    src/command/get_task_info.cpp
#    src/command/get_task_result_info.cpp
    src/command/get_tasks_collection.cpp
#    src/command/get_metric_definition_info.cpp
    src/command/get_metric_definitions_collection.cpp
    src/command/get_metrics.cpp
)