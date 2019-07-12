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

add_library(spdk-commands STATIC
    src/get_managers_collection.cpp
    src/get_manager_info.cpp
    src/get_collection.cpp
    src/get_chasis_info.cpp
    src/get_fabric_info.cpp
    src/get_storage_service_info.cpp
    src/get_drive_info.cpp
    src/get_volume_info.cpp
    src/get_network_interface_info.cpp
    src/get_storage_pool_info.cpp
    src/get_endpoint_info.cpp
    src/get_system_info.cpp
    src/get_zone_info.cpp
    src/get_metric_definitions_collection.cpp
    src/get_tasks_collection.cpp
    src/get_task_info.cpp
    src/get_task_result_info.cpp
    src/get_metrics.cpp

    src/set_component_attributes.cpp
    src/set_volume_attributes.cpp
    src/set_chassis_attributes.cpp
    src/set_endpoint_attributes.cpp

    src/add_zone.cpp
    src/add_volume.cpp
    src/add_zone_endpoints.cpp
    src/add_endpoint.cpp

    src/delete_zone.cpp
    src/delete_volume.cpp
    src/delete_endpoint.cpp
    src/delete_zone_endpoints.cpp

    src/command/helpers/helpers.cpp

    src/delete_storage_pool.cpp
    src/delete_drive.cpp)



target_link_libraries(spdk-commands
    PUBLIC
    spdk-discovery

    PRIVATE
    agent-framework-database
)

target_include_directories(spdk-commands
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_psme_command_target_properties(spdk-commands)
