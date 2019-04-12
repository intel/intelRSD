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
 *
 * @file eventing_server.hpp
 *
 * @brief Declaration of EventingServer class
 * */

#pragma once

#include "json-rpc/connectors/http_server_connector.hpp"
#include "agent-framework/command/command_server.hpp"
#include "agent-framework/command/registry.hpp"

#include <memory>
#include <string>


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
    explicit EventingServer(const ::json::Json& config);

    /*! @brief EventingServer destructor */
    ~EventingServer();

    /*! @brief Start server */
    void start();

    /*! @brief Stop server */
    void stop();

private:
    std::shared_ptr<json_rpc::AbstractServerConnector> m_connector;
    std::shared_ptr<agent_framework::command::CommandServer> m_server;
};

}
}
}
