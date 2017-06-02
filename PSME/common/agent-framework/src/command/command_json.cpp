/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file command_json.cpp
 *
 * @brief JSON command class implementation
 * */

#include "agent-framework/command/command_json.hpp"
#include "agent-framework/exceptions/exception.hpp"

#include <utility>
#include <iostream>

using namespace agent_framework::command;

static CommandJson::Map::CommandMap* g_command_map = nullptr;

CommandJson::Map::CommandMap& CommandJson::Map::get_instance() {
    if (nullptr == g_command_map) {
        g_command_map = new CommandMap {};
    }
    return *g_command_map;
}

void CommandJson::Map::cleanup() {
    delete g_command_map;
    g_command_map = nullptr;
}

void CommandJson::Map::add(
        const std::string& agent_type,
        const std::string& command_name,
        CommandCreate command_create) {
    CommandMap& command_map = get_instance();

    command_map[agent_type][command_name] = command_create;
}

Command* CommandJson::get_command() {
    if (nullptr == m_command.get()) {
        THROW(::agent_framework::exceptions::NotImplemented, "rpc",
            "Agent command not implemented!");
    }
    return m_command.get();
}

CommandJson::~CommandJson() { }
