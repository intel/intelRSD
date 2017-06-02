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
 * @file eventing_server.hpp
 *
 * @brief Declaration of EventingServer class
 * */
#include "psme/command/command_json_server.hpp"
#include "logger/logger_factory.hpp"

#include <jsonrpccpp/server.h>
#include <jsonrpccpp/server/connectors/httpserver.h>

#include <memory>
#include <string>

namespace json {
    /* Forward declaration */
    class Value;
}

/*! PSME namespace */
namespace psme {
/*! Application namespace */
namespace app {
/*! Eventing namespace */
namespace eventing {

/*! @brief EventingServer class */
class EventingServer {
public:
    /*!
     * @brief EventingServer constructor
     *
     * @param config Configuration JSON object
     */
    explicit EventingServer(const ::json::Value& config);

    /*! @brief EventingServer destructor */
    ~EventingServer();

    /*! @brief Start server */
    void start();

    /*! @brief Stop server */
    void stop();

    /*!
     * @brief Add commands to server
     * @param[in] command_map JSON command map with unique key string
     * */
    void add(const psme::command::CommandJson::Map::command_map_t& command_map);

private:
    jsonrpc::HttpServer m_http_server;
    psme::command::CommandJsonServer  m_command_json_server;
};

}
}
}
