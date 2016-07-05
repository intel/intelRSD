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
 * @file command/storage/delete_logical_drive.cpp
 *
 * @brief Generic storage command DeleteLogicalDrive implementation
 * */

#include "agent-framework/command/storage/delete_logical_drive.hpp"

using namespace agent_framework::command::storage;

constexpr const char DeleteLogicalDrive::AGENT[];

constexpr const char DeleteLogicalDrive::TAG[];

DeleteLogicalDrive::~DeleteLogicalDrive() { }

DeleteLogicalDrive::Request::~Request() { }

DeleteLogicalDrive::Response::~Response() { }
