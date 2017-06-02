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
 * @file command.hpp
 *
 * @brief Command interface
 * */

#pragma once
#include "command_exception.hpp"

#include <map>
#include <memory>
#include <string>
#include <exception>

/*! PSME Agent Framework */
namespace psme {
/*! Command */
namespace command {

using std::map;
using std::string;
using std::unique_ptr;

/*!
 * @brief Command class interface for all generic commands
 * */
class Command {
private:
    const string m_key;
    Command(const Command&) = delete;
    Command& operator=(const Command&) = delete;
public:
    class Argument;

    /*!
     * @brief Create Command object with unique tag strings
     *
     * @param[in] implementation    Command subgroup tag name to identify
     *                              concrete command implementation to use
     * @param[in] group             Command group tag name  string to identify
     *                              group
     * @param[in] name              Command tag name string to identify command
     * */
    Command(const string& implementation,
            const string& group,
            const string& name);

    /*!
     * @brief Execute command with input/output arguments
     * */
    virtual void execute(const Argument&, Argument&) = 0;

    /*!
     * @brief Create command key for accessing element in the command map
     *
     * @param[in] implementation    Command subgroup tag name to identify
     *                              concrete command implementation to use
     * @param[in] group             Command group tag name  string to identify
     *                              group
     * @param[in] name              Command tag name string to identify command
     * */
    static string create_key(
            const string& implementation,
            const string& group,
            const string& name);

    /*!
     * @brief Destroy command
     * */
    virtual ~Command();

    /*!
     * @brief Command argument
     * */
    class Argument {
    public:
        /*! Default constructor */
        Argument() = default;
        /*! Default copy constructor */
        Argument(const Argument&) = default;
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
        /*!
         * @brief Register unique command
         * */
        Register() { Map::add(*new T); }
    };

    /*!
     * @brief Singleton pattern. This class is used for registering Command
     * objects that will be use later
     * */
    class Map {
    private:
        /*!
         * @brief Singleton pattern. Add unique command to singleton list
         * instance
         *
         * @param[in] command   Add dynamically created Command object
         *                      to the list
         * */
        static void add(Command& command);

        template<class T> friend class Register;
    public:
        /*!
         * @brief JSON commands map with unique key string
         *
         * Key string format: "COMMAND_GROUP:COMMAND_NAME"
         * */
        typedef map<const string, unique_ptr<Command>> command_map_t;

        /*!
         * @brief Singleton pattern. Get commands register instance that
         * contains map with unique JSON commands that are identify by
         * key string: "COMMAND_GROUP:COMMAND_NAME"
         *
         * @return      Map that contains unique JSON commands with key string
         * */
        static command_map_t& get_instance();

        /*!
         * @brief Singleton pattern. Cleanup JSON commands map. This method
         * should be always call before application exit to avoid memory leaks
         * */
        static void cleanup();
    };
};

} /* namespace command */
} /* namespace psme */

