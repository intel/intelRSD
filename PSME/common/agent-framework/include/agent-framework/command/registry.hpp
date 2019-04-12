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
 * @file registry.hpp
 *
 * @brief Registry implementation
 * */

#pragma once
#include "agent-framework/generic/singleton.hpp"
#include "agent-framework/command/command.hpp"

#include <vector>

/*
 * This macro registers the command in the Registry.
 *
 * Parameter CMD_HANDLER is the Handler Type (i.e. name of the command whose
 * implementation we add). Available Handler Types are listed in this file
 * below. Parameter CMD_FUNC is a function with the implementation.
 *
 * This macro creates a dummy static variable that is initialized with the
 * return value of the add_handler<>() template function. This ensures that
 * this call (and the method registration) is performed as soon as the agent
 * application starts.
 *
 * */

#pragma once

#define REGISTER_COMMAND(CMD_HANDLER, CMD_FUNC)                              \
    class dummy_##CMD_HANDLER {                                              \
        static bool m_dummy;                                                 \
    };                                                                       \
    bool dummy_##CMD_HANDLER::m_dummy =                                      \
        agent_framework::command::Registry::get_instance()->                 \
        add_handler<CMD_HANDLER>(CMD_FUNC)

/*! Agent Framework */
namespace agent_framework {
/*! Command */
namespace command {

/*!
 * @brief Registry of JSON RPC Commands
 * */
class Registry : public agent_framework::generic::Singleton<Registry> {
public:

    using Commands = std::vector<std::shared_ptr<CommandBase>>;

    virtual ~Registry();

    /*!
     * @brief Adds command handler to the registry
     * @param[in] command_handler Command handler
     * @return Dummy value used for command registration, always true
     */
    template <typename T>
    bool add_handler(typename T::HandlerFunc command_handler) {
        log_info("agent", "Method " +
                 T::Request::get_command() + " has been registered");
        m_commands.emplace_back(std::shared_ptr<CommandBase>(new T(command_handler)));
        return true;
    }

    /*!
     * @brief Returns collections of all commands
     * @return Vector containing all registered commands
     */
    virtual const Commands& get_commands() const {
        return m_commands;
    }

private:
    Commands m_commands {};
};

}
}
