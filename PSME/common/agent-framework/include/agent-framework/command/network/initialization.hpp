/*!
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
 *
 * @file command/network/initialization.hpp
 * @brief Initialization command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_NETWORK_INITIALIZATION_HPP
#define AGENT_FRAMEWORK_COMMAND_NETWORK_INITIALIZATION_HPP

#include "agent-framework/command/command.hpp"

namespace agent_framework {
namespace command {
namespace network {

/*!
 * @brief Initialization/deinitialization command
 *
 * Initialization class that is called before any other commands.
 * Destructor is used as deinitialization before agent exit.
 * Execute method is never invoked
 * */
class Initialization : public Command {
public:
    /*! Tag string for identify agent */
    static constexpr const char AGENT[] = "Network";

    /*! Tag string for identify command */
    static constexpr const char TAG[] = "initialization";

    /*! Deinitialization */
    virtual ~Initialization();

private:
    /*!
     * @brief Execute command with input/output arguments. Not used
     * */
    void execute(const Argument&, Argument&) override final { }
};

}
}
}

#endif /* AGENT_FRAMEWORK_COMMAND_NETWORK_INITIALIZATION_HPP */
