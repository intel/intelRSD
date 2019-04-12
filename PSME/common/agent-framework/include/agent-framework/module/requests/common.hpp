/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * @file requests/common.hpp
 * @brief Common header for common requests
 * */

#pragma once
#include "agent-framework/module/requests/common/get_collection.hpp"
#include "agent-framework/module/requests/common/get_managers_collection.hpp"
#include "agent-framework/module/requests/common/get_manager_info.hpp"
#include "agent-framework/module/requests/common/set_component_attributes.hpp"
#include "agent-framework/module/requests/common/get_chassis_info.hpp"
#include "agent-framework/module/requests/common/get_processor_info.hpp"
#include "agent-framework/module/requests/common/get_drive_info.hpp"
#include "agent-framework/module/requests/common/delete_drive.hpp"
#include "agent-framework/module/requests/common/get_system_info.hpp"
#include "agent-framework/module/requests/common/get_storage_subsystem_info.hpp"
#include "agent-framework/module/requests/common/get_pcie_device_info.hpp"
#include "agent-framework/module/requests/common/get_pcie_function_info.hpp"
#include "agent-framework/module/requests/common/get_tasks_collection.hpp"
#include "agent-framework/module/requests/common/get_task_info.hpp"
#include "agent-framework/module/requests/common/get_task_result_info.hpp"
#include "agent-framework/module/requests/common/delete_task.hpp"
#include "agent-framework/module/requests/common/get_metric_definitions_collection.hpp"
#include "agent-framework/module/requests/common/get_metrics.hpp"
#include "agent-framework/module/requests/common/get_metric_definition_info.hpp"
#include "agent-framework/module/requests/common/get_fabric_info.hpp"
#include "agent-framework/module/requests/common/get_zone_info.hpp"
#include "agent-framework/module/requests/common/add_zone.hpp"
#include "agent-framework/module/requests/common/delete_zone.hpp"
#include "agent-framework/module/requests/common/add_zone_endpoints.hpp"
#include "agent-framework/module/requests/common/delete_zone_endpoints.hpp"
#include "agent-framework/module/requests/common/get_endpoint_info.hpp"
#include "agent-framework/module/requests/common/add_endpoint.hpp"
#include "agent-framework/module/requests/common/delete_endpoint.hpp"
#include "agent-framework/module/requests/common/get_network_interface_info.hpp"
#include "agent-framework/module/utils/is_requested_metric.hpp"
