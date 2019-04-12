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
 * @file rest_server.hpp
 *
 * @brief Declaration of REST Server
 * */

#pragma once

#include "server/connector/connector_options_loader.hpp"
#include "server/connector/connector_factory.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <vector>

namespace psme {
namespace rest {
namespace server {


/*!
 * @brief Responsible for starting the REST server handling REST API requests.
 *
 * Depending on the configuration server may open several ports on which
 * it listens for connections. Each port is managed by separate Connector.
 * Default configuration (default_configuration.hpp #DEFAULT_CONFIGURATION)
 * creates two Connectors for HTTP on port 8888 and HTTPS on port 8443.
 * */
class RestServer final {
public:
    /*!
     * @brief Constructor
     *
     * Reads REST server configuration and initializes Connectors.
     *
     * @throw std::runtime_error If no valid connector configuration is found.
     */
    explicit RestServer();

    /*! @brief Destructor */
    ~RestServer();

    /*! @brief Starts REST server connectors.
     *
     * After success each server's Connector listens on specified port
     * for REST API requests.
     *
     * @throw std::runtime_error If could not start all connectors.
     */
    void start();

    /*! @brief Stops REST server connectors. */
    void stop();

private:
    std::vector<ConnectorUPtr> m_connectors{};
};

}
}
}
