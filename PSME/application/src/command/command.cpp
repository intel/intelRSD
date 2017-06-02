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
 * @file command.cpp
 *
 * @brief Command implementation
 * */

#include "psme/command/command.hpp"

#include <utility>

using namespace psme::command;

static Command::Map::command_map_t* g_command_map = nullptr;

Command::Map::command_map_t& Command::Map::get_instance() {
    if (nullptr == g_command_map) {
        g_command_map = new command_map_t{};
    }
    return *g_command_map;
}

void Command::Map::cleanup() {
    delete g_command_map;
    g_command_map = nullptr;
}

void Command::Map::add(Command& command) {
    auto& command_map = get_instance();

    if (command_map.end() != command_map.find(command.m_key)) {
        delete &command;
        return;
    }

    command_map[command.m_key] = unique_ptr<Command>(&command);
}

string Command::create_key(
        const string& implementation,
        const string& group,
        const string& name) {
    return implementation + ":" + group + ":" + name;
}

Command::Command(
        const string& implementation,
        const string& group,
        const string& name)
    : m_key(create_key(implementation, group, name)) { }

Command::~Command() { }

Command::Argument::~Argument() { }
