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

add_library(fpgaof-commands STATIC
    src/get_chasis_info.cpp
    src/get_collection.cpp
    src/get_endpoint_info.cpp
    src/get_fabric_info.cpp
    src/get_manager_info.cpp
    src/get_managers_collection.cpp
    src/get_metric_definitions_collection.cpp
    src/get_metrics.cpp
    src/get_network_interface_info.cpp
    src/get_processor_info.cpp
    src/get_system_info.cpp
    src/get_tasks_collection.cpp
    src/get_zone_info.cpp
    src/get_task_info.cpp
    src/get_task_result_info.cpp

    src/set_component_attributes.cpp
    src/set_chassis_attributes.cpp
    src/set_endpoint_attributes.cpp
    src/set_processor_attributes.cpp

    src/add_zone.cpp
    src/add_zone_endpoints.cpp
    src/add_endpoint.cpp

    src/delete_zone.cpp
    src/delete_endpoint.cpp
    src/delete_zone_endpoints.cpp
)


target_link_libraries(fpgaof-commands
    PUBLIC
    fpgaof-discovery
    fpgaof-fpga

    PRIVATE
    agent-framework-database
)

target_include_directories(fpgaof-commands
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_psme_command_target_properties(fpgaof-commands)
