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

add_library(chassis-command-sdv OBJECT
    get_manager_info.cpp
    get_collection.cpp
    get_chassis_info.cpp
    get_authorization_certificate.cpp
    get_managers_collection.cpp
    set_component_attributes.cpp
    get_tasks_collection.cpp
    get_task_info.cpp
    get_task_result_info.cpp
    delete_task.cpp
    get_metric_definition_info.cpp
    get_metric_definitions_collection.cpp
    get_metrics.cpp
)

set_psme_command_target_properties(chassis-command-sdv)
