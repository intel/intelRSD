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
 * @file command.hpp
 *
 * @brief Command interface
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_COMMAND_HPP
#define AGENT_FRAMEWORK_COMMAND_COMMAND_HPP

#include <map>
#include <memory>
#include <string>
#include <exception>
#include <functional>

#include "agent-framework/command/command_exception.hpp"

/*! Agent Framework */
namespace agent_framework {
/*! Command */
namespace command {

using std::map;
using std::string;
using std::unique_ptr;

/*!
 * @brief Command class interface for all generic commands
 * */
class Command {
public:
    class Argument;

    /*!
     * @brief Execute command with input/output arguments
     * */
    virtual void execute(const Argument&, Argument&) = 0;

    /*!
     * @brief Destroy command
     * */
    virtual ~Command();

    /*!
     * @brief Command argument
     * */
    class Argument {
    public:
        /*! Default assigment constructor */
        Argument& operator=(const Argument&) = default;

        /*! Command virtual destructor */
        virtual ~Argument();
    };

    /*!
     * @brief This class is used to register client command that inherit from
     * generic command class. Create Register as static global object to
     * automatically add Register constructor before main starts.
     * */
    template<class T>
    class Register {
    public:
       Register(const char* implementation) {
            if (nullptr == implementation) { return; }

            Map::add(
                T::AGENT,
                implementation,
                T::TAG,
                [] { return new T; }
            );
        }
    };

    /*!
     * @brief Singleton pattern. This class is used for registering Command
     * objects that will be use later
     * */
    class Map {
    public:
        /*! Function that creates command dynamically at runtime */
        using CommandCreate = std::function<Command*()>;

        /*! Commands map AGENT:IMPLEMENTATION:COMMAND */
        using CommandMap = std::map<
            const std::string, std::map<        /* Agents map */
            const std::string, std::map<        /* Implementations map */
            const std::string, CommandCreate>>>;/* Create functions map */

        /*!
         * @brief Singleton pattern. Get commands register instance that
         * contains map with unique command create methods that are identify by
         * key: "AGENT:IMPLEMENTATION:COMMAND"
         *
         * @return      Map that contains unique commands with key
         * */
        static CommandMap& get_instance();

        /*!
         * @brief Singleton pattern. Cleanup commands map. This method
         * should be always call before application exit to avoid memory leaks
         * */
        static void cleanup();
    private:
        /*!
         * @brief Singleton pattern. Add command to commands map with
         * unique key string that is encode from Command class
         *
         * @param[in]   agent_type              Agent name
         * @param[in]   command_implementation  Implementation name
         * @param[in]   command_name            Command name
         * @param[in]   command_create          Method that dynamically create
         *                                      command at runtime
         * */
        static void add(
            const std::string& agent_type,
            const std::string& command_implementation,
            const std::string& command_name,
            CommandCreate command_create);

        template<class T> friend class Register;
    };
};

} /* namespace command */
} /* namespace agent_framework */

#endif /* AGENT_FRAMEWORK_COMMAND_COMMAND_HPP */
