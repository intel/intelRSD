/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @section DESCRIPTION
 *
 * @file storage_commands.cpp
 *
 * @brief Declarations of all storage commands
 * */

#pragma once
#include "agent-framework/module-ref/model/model_storage.hpp"
#include "agent-framework/module-ref/requests/common.hpp"
#include "agent-framework/module-ref/requests/storage.hpp"
#include "agent-framework/module-ref/responses/common.hpp"

namespace agent_framework {
namespace command_ref {

// declarations of get collection methods
using GetCollection = Command<model::requests::GetCollection, model::attribute::Array<model::attribute::SubcomponentEntry>>;
using GetManagerCollection = Command<model::requests::GetManagerCollection, model::attribute::Array<model::attribute::ManagerEntry>>;

// declarations of all get info methods
using GetIscsiTargetInfo = Command<model::requests::GetIscsiTargetInfo, model::IscsiTarget>;
using GetLogicalDriveInfo = Command<model::requests::GetLogicalDriveInfo, model::LogicalDrive>;
using GetManagerInfo = Command<model::requests::GetManagerInfo, model::Manager>;
using GetPhysicalDriveInfo = Command<model::requests::GetPhysicalDriveInfo, model::PhysicalDrive>;
using GetStorageServicesInfo = Command<model::requests::GetStorageServicesInfo, model::StorageService>;

// declarations of all set methods
using SetComponentAttributes = Command<model::requests::SetComponentAttributes, model::responses::SetComponentAttributes>;
using SetIscsiTarget = Command<model::requests::SetIscsiTarget, model::responses::SetIscsiTarget>;

// declarations of all add methods
using AddIscsiTarget = Command<model::requests::AddIscsiTarget, model::responses::AddIscsiTarget>;
using AddLogicalDrive = Command<model::requests::AddLogicalDrive, model::responses::AddLogicalDrive>;

// declarations of all delete methods
using DeleteIscsiTarget = Command<model::requests::DeleteIscsiTarget, model::responses::DeleteIscsiTarget>;
using DeleteLogicalDrive = Command<model::requests::DeleteLogicalDrive, model::responses::DeleteLogicalDrive>;

}
}

