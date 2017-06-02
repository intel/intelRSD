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
 * @file command_json_server.hpp
 *
 * @brief Command JSON Server implementation for JSON RPC C++ library
 * */

#pragma once
#include "command_json.hpp"

#include <jsonrpccpp/server.h>

#include <map>
#include <memory>
#include <string>
#include <functional>


namespace psme {
namespace command {

using std::map;
using std::string;
using std::function;
using std::unique_ptr;

using namespace jsonrpc;

/*!
 * @brief Command JSON server
 *
 * It will call added JSON RPC methods/notifications based on JSON Procedure
 * object for all incoming JSON RPC request objects
 * */
class CommandJsonServer : public IProcedureInvokationHandler {
public:
    /*! JSON RPC method */
    typedef function<void(const Json::Value& parameter, Json::Value& result)>
        method_function_t;

    /*! JSON RPC notification */
    typedef function<void(const Json::Value& parameter)>
        notification_function_t;

    /*!
     * @brief Create command JSON server. On default server is stoppped
     *
     * @param[in]   connector   Server connector
     * @param[in]   type        JSON RPC version
     * */
    CommandJsonServer(AbstractServerConnector& connector,
            serverVersion_t type = JSONRPC_SERVER_V2);

    /*!
     * @brief Add JSON RPC method
     *
     * @param[in]   proc    JSON procedure for parsing request by JSON server
     * @param[in]   method  JSON RPC method function
     * */
    void add(const Procedure& proc, const method_function_t& method);

    /*!
     * @brief Add JSON RPC notification
     *
     * @param[in]   proc    JSON procedure for parsing request by JSON server
     * @param[in]   notification  JSON RPC notification function
     * */
    void add(const Procedure& proc, const notification_function_t& notification);

    /*!
     * @brief Add JSON methods and notifications that are in Command JSON map
     *
     * @param[in]   command_map Command JSON map that includes JSON RPC
     *                          methods and notifications
     * */
    void add(const CommandJson::Map::command_map_t& command_map);

    /*! Start command JSON server */
    void start();

    /*! Stop command JSON server (on default) */
    void stop();

    /*! Destructor */
    virtual ~CommandJsonServer();

protected:
    /*!
     * @brief JSON RPC method handler called by JSON server
     *
     * @param[in]   proc    JSON procedure
     * @param[in]   input   JSON RPC request object
     * @param[out]  output  JSON RPC response object
     * */
    virtual void HandleMethodCall(Procedure& proc,
            const Json::Value& input, Json::Value& output);

    /*!
     * @brief JSON RPC notification handler called by JSON server
     *
     * @param[in]   proc    JSON procedure
     * @param[in]   input   JSON RPC request object
     * */
    virtual void HandleNotificationCall(Procedure& proc,
            const Json::Value& input);

private:
    AbstractServerConnector& m_connection;
    unique_ptr<IProtocolHandler> m_handler;
    map<string, method_function_t> m_methods;
};

} /* namespace command */
} /* namespace psme */

