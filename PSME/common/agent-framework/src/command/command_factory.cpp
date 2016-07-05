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
 * @file command_factory.cpp
 *
 * @brief Command factory implementation
 * */

#include "agent-framework/command/command_factory.hpp"
#include "agent-framework/command/command.hpp"
#include "agent-framework/command/command_json.hpp"
#include "agent-framework/logger_ext.hpp"

#include <cstring>
#include <exception>

using namespace agent_framework::command;
using agent_framework::command::CommandJson;

static std::nullptr_t command_not_implemented() { return nullptr; }

static CommandFactory* g_command_factory = nullptr;

CommandFactory& CommandFactory::get_instance() {
    if (nullptr == g_command_factory) {
        g_command_factory = new CommandFactory();
    }

    return *g_command_factory;
}

void CommandFactory::cleanup() {
    delete g_command_factory;
    g_command_factory = nullptr;
}

void CommandFactory::add_commands(const std::string& agent,
        const std::string& implementation)
{
    try {
        const auto& command_json_map = CommandJson::Map::get_instance()
            .at(agent);

        for (const auto& command_json : command_json_map) {
            m_commands_set.emplace(
                    std::make_tuple(agent, implementation, command_json.first));
        }
    } catch (...) {
        log_warning(GET_LOGGER("command"), "Cannot add commands from "
            << agent << "::"
            << implementation);
    }
}

void CommandFactory::add_commands(const json::Value& configuration) {
    std::string global_agent{};
    std::string global_implementation{};

    if (configuration["agent"]["type"].is_string()) {
        global_agent = configuration["agent"]["type"].as_string();
    }

    if (configuration["commands"]["*"]["implementation"].is_string()) {
        global_implementation = configuration["commands"]["*"]["implementation"]
            .as_string();
    }

    if (!global_agent.empty() && !global_implementation.empty()) {
        add_commands(global_agent, global_implementation);
    }

    const auto& commands = configuration["commands"];

    if (commands.is_object()) {
        std::string agent{};
        std::string implementation{};

        for (auto it = commands.cbegin(); it != commands.cend(); ++it) {
            if (0 == std::strncmp(it.key(), "*", 1)) {
                continue;
            }

            const auto& setting = *it;

            if (setting["agent"].is_string()) {
                agent = setting["agent"].as_string();
            }
            else { agent = global_agent; }

            if (setting["implementation"].is_string()) {
                implementation = setting["implementation"].as_string();
            }
            else { implementation = global_implementation; }

            add_command(agent, implementation, it.key());
        }
    }
}

CommandFactory::CommandsJsonVector CommandFactory::create() const {
    CommandsJsonVector commands{};
    CommandJson::Map::CommandCreate create_json_command{};
    Command::Map::CommandCreate create_command{};

    log_debug(GET_LOGGER("command"), "Creating commands for agent...");

    for (const auto& command_set : m_commands_set) {
        try {
            create_json_command = CommandJson::Map::get_instance()
                .at(std::get<0>(command_set))
                .at(std::get<2>(command_set));
        }
        catch (...) {
            log_warning(GET_LOGGER("command"), "Not implemented JSON command "
                << std::get<0>(command_set) << "::"
                << std::get<2>(command_set) << "()");

            /* Get next command */
            continue;
        }

        try {
            create_command = Command::Map::get_instance()
                .at(std::get<0>(command_set))
                .at(std::get<1>(command_set))
                .at(std::get<2>(command_set));
            log_debug(GET_LOGGER("command"), "Created command "
                << std::get<0>(command_set) << "::"
                << std::get<1>(command_set) << "::"
                << std::get<2>(command_set) << "()");
        }
        catch (...) {
            create_command = command_not_implemented;
            log_warning(GET_LOGGER("command"), "Created command "
                << std::get<0>(command_set) << "::"
                << std::get<1>(command_set) << "::"
                << std::get<2>(command_set) << "() - no implementation!");
        }

        commands.emplace_back(create_json_command());
        commands.back()->set_command(create_command());
    }

    return commands;
}

CommandFactory::CommandsVector CommandFactory::create_initialization() {
    CommandsVector commands{};
    Command::Map::CommandCreate create_command{};
    CommandsSet commands_set;

    log_debug(GET_LOGGER("command"), "Creating initialization for agent...");

    for (const auto& command_set : m_commands_set) {
        commands_set.emplace(
                std::get<0>(command_set),
                std::get<1>(command_set),
                "initialization");
    }

    for (const auto& command_set : commands_set) {
        try {
            create_command = Command::Map::get_instance()
                .at(std::get<0>(command_set))
                .at(std::get<1>(command_set))
                .at(std::get<2>(command_set));
            log_debug(GET_LOGGER("command"), "Created "
                << std::get<0>(command_set) << "::"
                << std::get<1>(command_set) << "::"
                << std::get<2>(command_set) << "()");
        }
        catch (...) {
            /* Initialization not found */
            log_warning(GET_LOGGER("command"), "Not found "
                << std::get<0>(command_set) << "::"
                << std::get<1>(command_set) << "::"
                << std::get<2>(command_set) << "()");
            continue;
        }

        commands.emplace_back(create_command());
    }

    return commands;
}
