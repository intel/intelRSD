/*!
 * Copyright (c) 2015-2016 Intel Corporation
 *
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
 * @file command/network/get_ethernet_switch_info.cpp
 * @brief Generic network command get ethernet
 * switch information implementation
 * */

#include "agent-framework/command/network/get_ethernet_switch_info.hpp"

using namespace agent_framework::command::network;

constexpr const char GetEthernetSwitchInfo::AGENT[];

constexpr const char GetEthernetSwitchInfo::TAG[];

GetEthernetSwitchInfo::~GetEthernetSwitchInfo() { }

GetEthernetSwitchInfo::Request::~Request() { }

GetEthernetSwitchInfo::Response::~Response() { }
