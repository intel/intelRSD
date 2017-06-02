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
#include "command.hpp"
#include "command_exception.hpp"

#include <jsonrpccpp/server.h>

#include <map>
#include <memory>
#include <string>


namespace psme {
namespace command {

using std::map;
using std::string;
using std::unique_ptr;
using jsonrpc::Procedure;

/*!
 * @brief Command abstract interface for all JSON commands from server side
 * */
class CommandJson {
private:
    string m_key;
    Procedure m_procedure;
    Command* m_command = nullptr;
    CommandJson(const CommandJson&) = delete;
    CommandJson& operator=(const CommandJson&) = delete;
protected:
    /*!
     * @brief Create JSON command object with unique tag strings
     *
     * @param[in]   group       Command tag group name string to identify group
     * @param[in]   procedure   JSON RPC procedure for parsing JSON parameters
     * */
    CommandJson(const string& group, const Procedure& procedure);

    /*!
     * @brief Get Command object
     *
     * @return Command object
     * */
    Command* get_command();
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
    inline void set_command(Command* command) { m_command = command; }

    /*!
     * @brief Get JSON Procedure for JSON command server that will using
     * this procedure for parsing incoming JSON request
     *
     * @return  JSON procedure for JSON command server
     * */
    inline Procedure& get_procedure() { return m_procedure; }

    /*!
     * @brief Create command key access for the JSON command map
     *
     * @param[in]   group   Command tag group name string for identify command
     *                      in the group
     * @param[in]   name    Command tag name string for identify command name
     * @return  Command key string for the JSON command map
     * */
    static string create_key(const string& group, const string& name);

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
        Register() { Map::add(new T); }
    };

    /*!
     * @brief Singleton pattern. This class is used for registering CommandJson
     * objects that will be use later
     * */
    class Map {
    private:
        /*!
         * @brief Singleton pattern. Add JSON command to JSON commands map with
         * unique key string that is encode from CommandJson class
         *
         * @param[in]   command Add dynamically created JSON command to
         *                      the JSON commands map
         * */
        static void add(CommandJson* command);

        template<class T> friend class Register;
    public:
        /*!
         * @brief JSON commands map with unique key string
         *
         * Key string format: "COMMAND_GROUP:COMMAND_NAME"
         * */
        typedef map<const string, unique_ptr<CommandJson>> command_map_t;

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

        /*!
         * @brief Set JSON commands implementation for Command::Map
         *
         * @param[in]   implementation  Implementation tag
         * */
        static void set_implementation(const string& implementation);
    };
};

} /* namespace command */
} /* namespace psme */

