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

add_library(command-storage-json OBJECT
    set_component_attributes.cpp
    get_manager_info.cpp
    get_manager_collection.cpp
    get_storage_services_info.cpp
    get_collection.cpp
    get_physical_drive_info.cpp
    get_logical_drive_info.cpp
    add_logical_drive.cpp
    delete_logical_drive.cpp
    get_iscsi_target_info.cpp
    add_iscsi_target.cpp
    delete_iscsi_target.cpp
)

set_psme_command_target_properties(command-storage-json)
