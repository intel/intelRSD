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

set(SOURCES
    src/get_managers_collection.cpp
    src/get_tasks_collection.cpp
    src/get_manager_info.cpp
    src/get_chassis_info.cpp
    src/get_system_info.cpp
    src/get_network_interface_info.cpp
    src/get_drive_info.cpp
    src/get_storage_service_info.cpp
    src/get_collection.cpp
    src/get_task_info.cpp
    src/get_task_result_info.cpp
    src/get_metric_definition_info.cpp
    src/get_metric_definitions_collection.cpp
    src/get_metrics.cpp
    src/get_fabric_info.cpp
    src/get_zone_info.cpp
    src/get_endpoint_info.cpp
    src/get_storage_pool_info.cpp
    src/get_volume_info.cpp

    src/add_volume.cpp
    src/delete_drive.cpp
    src/delete_storage_pool.cpp
    src/delete_task.cpp
    src/delete_volume.cpp

    src/set_component_attributes.cpp
    src/set_volume_attributes.cpp
    src/set_chassis_attributes.cpp
)

set(TGT_SOURCES
    src/iscsi/add_endpoint.cpp
    src/iscsi/add_zone.cpp
    src/iscsi/add_zone_endpoints.cpp
    src/iscsi/delete_endpoint.cpp
    src/iscsi/delete_zone.cpp
    src/iscsi/delete_zone_endpoints.cpp
    src/iscsi/set_endpoint_attributes.cpp
)

set(NVME_SOURCES
    src/nvme/set_endpoint_attributes.cpp
    # TODO: LVM+NVMe Out of scope
   )

add_library(storage-tgt-commands STATIC
    ${SOURCES}
    ${TGT_SOURCES}
)


set_psme_command_target_properties(storage-tgt-commands)

target_compile_definitions(storage-tgt-commands PRIVATE
    ${TGT_FLAGS}
)

target_include_directories(storage-tgt-commands
    PUBLIC
    ./include
)

target_link_libraries(storage-tgt-commands
    PUBLIC
    storage-tgt-discovery
    agent-framework
)

add_library(storage-nvme-commands STATIC
    ${SOURCES}
    ${NVME_SOURCES}
)
set_psme_command_target_properties(storage-nvme-commands)

target_include_directories(storage-nvme-commands
    PUBLIC
    ./include
)

target_link_libraries(storage-nvme-commands
    PUBLIC
    storage-nvme-discovery
    agent-framework
)
