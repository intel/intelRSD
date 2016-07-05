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
 * @file command/storage/get_physical_drive_info.cpp
 *
 * @brief Generic storage command GetPhysicalDriveInfo implementation
 * */

#include "agent-framework/command/storage/get_physical_drive_info.hpp"

using namespace agent_framework::command::storage;

constexpr const char GetPhysicalDriveInfo::AGENT[];

constexpr const char GetPhysicalDriveInfo::TAG[];

GetPhysicalDriveInfo::~GetPhysicalDriveInfo() { }

GetPhysicalDriveInfo::Request::~Request() { }

GetPhysicalDriveInfo::Response::~Response() { }
