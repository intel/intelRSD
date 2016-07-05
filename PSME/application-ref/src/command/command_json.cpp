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
 * @file command_json.cpp
 *
 * @brief JSON command class implementation
 * */

#include "psme/command/command_json.hpp"
#include "logger/logger_factory.hpp"

#include <utility>
#include <iostream>

using namespace psme::command;

static CommandJson::Map::command_map_t* g_command_map = nullptr;

CommandJson::Map::command_map_t& CommandJson::Map::get_instance() {
    if (nullptr == g_command_map) {
        g_command_map = new command_map_t{};
    }
    return *g_command_map;
}

void CommandJson::Map::cleanup() {
    delete g_command_map;
    g_command_map = nullptr;
}

void CommandJson::Map::add(CommandJson* command) {
    auto& command_map = get_instance();

    if (command_map.end() != command_map.find(command->m_key)) {
        delete command;
        return;
    }

    command_map[command->m_key] = unique_ptr<CommandJson>(command);
}

string CommandJson::create_key(
        const string& group,
        const string& name) {
    return group + ":" + name;
}

CommandJson::CommandJson(const string& group, const Procedure& procedure) :
    m_key(create_key(group, procedure.GetProcedureName())),
    m_procedure(procedure) { }

Command* CommandJson::get_command() {
    if (nullptr == m_command) {
        log_error(GET_LOGGER("command"), "Command not implemented");
        throw command::exception::NotImplemented();
    }

    return m_command;
}

void CommandJson::Map::set_implementation(const string& implementation) {
    for (const auto& command_pair : CommandJson::Map::get_instance()) {
        auto command_key = implementation + ":" + command_pair.first;
        if (Command::Map::get_instance().find(command_key) !=
                Command::Map::get_instance().cend()) {
            command_pair.second.get()->set_command(
                Command::Map::get_instance().at(command_key).get());
        }
    }
}

CommandJson::~CommandJson() { }
