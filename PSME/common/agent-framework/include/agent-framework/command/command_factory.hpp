/*!
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
 *
 * @file command_factory.hpp
 *
 * @brief Commands factory interface
 * */

#pragma once
#include <agent-framework/command/command.hpp>
#include <agent-framework/command/command_json.hpp>

#include <json/json.hpp>

#include <set>
#include <tuple>
#include <string>

/*! Agent Framework */
namespace agent_framework {
/*! Command */
namespace command {

/*!
 * @brief %Command factory class that creates commands registered by
 * Command::Register and CommandJson::Register
 * */
class CommandFactory {
public:
    /*! Vector of JSON commands */
    using CommandsJsonVector = std::vector<std::unique_ptr<CommandJson>>;

    /*! Vector of commands */
    using CommandsVector = std::vector<std::unique_ptr<Command>>;

    /*!
     * @brief Singleton pattern. Get CommandFactory instance
     *
     * @return  CommandFactory instance. Create if not exist otherwise
     *          return existed instance
     * */
    static CommandFactory& get_instance();

    /*!
     * @brief %Command factory cleanup
     *
     * This method SHOULD be called before application exit
     * */
    static void cleanup();

    /*!
     * @brief Add agent commands
     *
     * @param[in]   agent   Agent name like 'Storage', 'Network' or 'Compute'
     * @param[in]   implementation  Implementation type like 'OpenIPMI'
     * */
    void add_commands(
            const std::string& agent,
            const std::string& implementation);

    /*!
     * @brief Add not existing command or overide previous set
     *
     * @param[in]   agent   Agent name
     * @param[in]   implementation  Implementation type
     * @param[in]   name    Command name like GetComponent
     * */
    void add_command(
            const std::string& agent,
            const std::string& implementation,
            const std::string& name) {
        m_commands_set.emplace(std::make_tuple(agent, implementation, name));
    }

    /*!
     * @brief Add commands from json configuration
     *
     * @param[in]   configuration   JSON configuration
     * */
    void add_commands(const json::Value& configuration);

    /*!
     * @brief Create commands
     *
     * @return Vector of created commands
     * */
    CommandsJsonVector create() const;

    /*!
     * @brief Create initialization
     *
     * @return Vector of created initializers
     * */
    CommandsVector create_initialization();
private:
    CommandFactory() {}

    using CommandsSet = std::set<std::tuple<
        const std::string,     /* Agent */
        const std::string,     /* Implementation */
        const std::string      /* Command */
    >>;

    CommandsSet m_commands_set{};
};

} /* namespace command */
} /* namespace agent_framework */

