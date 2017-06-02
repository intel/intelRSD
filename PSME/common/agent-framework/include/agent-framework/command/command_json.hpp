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
 * @file command_json.hpp
 *
 * @brief Command abstract interface for Json class.
 *
 * Include singleton pattern for registering JSON commands to be use later.
 * */

#pragma once
#include <map>
#include <memory>
#include <string>

#include <jsonrpccpp/server.h>

#include "agent-framework/command/command.hpp"
#include "agent-framework/command/command_exception.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

namespace agent_framework {
namespace command {

using jsonrpc::Procedure;
using jsonrpc::ProcedureValidator;


/*!
 * @brief Command abstract interface for all JSON commands from server side
 * */
class CommandJson {
private:
    /*! @brief Dummy validator to be used for initializing const&
     * @deprecated It should be removed when all commands are refactored
     */
    ProcedureValidator m_procedure_ref __attribute__((deprecated));

    /*! @brief General validator for the command */
    const ProcedureValidator& m_procedure;

    std::unique_ptr<Command> m_command {};
    CommandJson(const CommandJson&) = delete;
    CommandJson& operator=(const CommandJson&) = delete;
protected:

    /*!
     * @brief Create JSON command object with unique tag strings
     * @deprecated should be not used with "new" commands
     * When all command are refactored, procedure_ref should be removed as well
     *
     * @param[in]   procedure   JSON RPC procedure for parsing JSON parameters
     * */
    CommandJson(const Procedure& procedure) __attribute__((deprecated)) :
        m_procedure_ref(procedure),
        m_procedure(m_procedure_ref)
    {}

    /*!
     * @brief Create JSON command object with unique tag strings
     *
     * @param[in]   procedure   JSON RPC procedure for parsing JSON parameters
     * */
    CommandJson(const ProcedureValidator& procedure) :
        m_procedure_ref(static_cast<const Procedure&>(procedure)),
        m_procedure(procedure)
    {}


public:
    /*!
     * @brief JSON RPC method. Implement only JSON method or notification.
     * JSON server will choose only one of them based on JSON Procedure.
     * Method has both JSON request and response object. For error handling
     * throw JsonRpcException object.
     *
     * @param[in]   params  JSON request object
     * @param[out]  result  JSON response object
     * */
    virtual void method(const Json::Value& params, Json::Value& result) = 0;

    /*!
     * @brief JSON RPC notification. Implement only JSON method or notification.
     * JSON server will choose only one of them based on JSON Procedure.
     * Notification has only JSON request object. For error handling throw
     * JsonRpcException object.
     *
     * @param[in]   params  JSON request object
     * */
    virtual void notification(const Json::Value& params) = 0;

    /*!
     * @brief Set Command object to use by CommandJson
     *
     * @param[in]   command     Point to existing Command object
     * */
    void set_command(Command* command) { m_command.reset(command); }

    /*!
     * @brief Get Command object
     *
     * @return Command object
     *
     * @throw command::exception::NotImplemented when command wasn't create
     * */
    Command* get_command() noexcept(false);

    /*!
     * @brief Get JSON Procedure for JSON command server that will using
     * this procedure for parsing incoming JSON request
     *
     * @return  JSON procedure for JSON command server
     * */
    const ProcedureValidator& get_procedure() {
        return m_procedure;
    }

    /*! Destructor */
    virtual ~CommandJson();

    /*!
     * @brief This class is used to register client command that inherit from
     * CommandJson class interface. Create Register as static global object to
     * automatically add Register constructor before main starts
     * */
    template<class T>
    class Register {
    public:
        /*!
         * @brief Register unique JSON command
         * */
        Register() {
            Map::add(
                T::AGENT,
                T::TAG,
                [] { return new T; }
            );
        }
    };

    /*!
     * @brief Singleton pattern. This class is used for registering CommandJson
     * objects that will be use later
     * */
    class Map {
    public:
        /*! Function that creates command dynamically at runtime */
        using CommandCreate = std::function<CommandJson*()>;

        /*! Commands map AGENT:COMMAND */
        using CommandMap = std::map<
            const std::string, std::map<        /* Agents map */
            const std::string, CommandCreate>>; /* Create functions map */

        /*!
         * @brief Singleton pattern. Get commands register instance that
         * contains map with unique JSON commands that are identify by
         * key string: "COMMAND_GROUP:COMMAND_NAME"
         *
         * @return      Map that contains unique JSON commands with key string
         * */
        static CommandMap& get_instance();

        /*!
         * @brief Singleton pattern. Cleanup JSON commands map. This method
         * should be always call before application exit to avoid memory leaks
         * */
        static void cleanup();
    private:
        /*!
         * @brief Singleton pattern. Add JSON command to JSON commands map with
         * unique key string that is encode from CommandJson class
         *
         * @param[in]   agent_type              Agent type
         * @param[in]   command_implementation  Command implementation
         * @param[in]   command_create          Command create function
         * */
        static void add(
                const std::string& agent_type,
                const std::string& command_implementation,
                CommandCreate command_create);

        template<class T> friend class Register;
    };
};

} /* namespace command */
} /* namespace agent_framework */

