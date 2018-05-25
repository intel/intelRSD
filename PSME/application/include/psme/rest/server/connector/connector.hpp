/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * @file connector.hpp
 *
 * @brief HTTP Connector declaration.
 *
 * */

#pragma once



#include "psme/rest/server/connector/connector.hpp"
#include "psme/rest/server/connector/connector_options.hpp"
#include "psme/rest/server/request.hpp"
#include "psme/rest/server/response.hpp"

#include <functional>
#include <memory>



namespace psme {
namespace rest {
namespace server {

/*!
 * @brief Default Connector callback for request processing if none is provided via Connector::set_callback method.
 *
 * This callback returns method not allowed error response.
 * */
void default_callback(const Request& request, Response& response);


/*!
 * @brief Connector represents the interface between external clients
 * sending requests to (and receiving responses from) REST server.
 *
 * Connector is an HTTP server listening on specified port.
 * It processes requests and sends back responses via registered Callback handler.
 * Provided implementations (MHDConnector, MHDSSLConnector) of Connector interface are based on
 * <a href="https://www.gnu.org/software/libmicrohttpd">Libmicrohttpd</a> library.
 */
class Connector {
public:
    /*! @brief Decides if given request will be processed by handler */
    typedef std::function<bool(const Request&, Response&)> AccessCallback;

    /*! @brief Callback handler for processing HTTP requests */
    typedef std::function<void(const Request&, Response&)> Callback;


    /*!
     * @brief Constructor.
     * @param[in] options ConnectorOptions used for Connector initialization.
     */
    explicit Connector(const ConnectorOptions& options);


    /*! @brief Destructor */
    virtual ~Connector();


    /*!
     * @brief Starts HTTP server on specified port and waits for HTTP requests.
     */
    virtual void start() = 0;


    /*!
     * @brief Stops HTTP server.
     */
    virtual void stop() = 0;


    /*!
     * @brief Setter for AccessCallback permitting access to resources.
     * @param[in] callback AccessCallback permitting access to resources.
     */
    void set_access_callback(const AccessCallback& callback);


    /*!
     * @brief Setter for Callback processing HTTP requests
     * @param[in] callback Callback processing HTTP requests
     */
    void set_callback(const Callback& callback);


    /*!
     * @brief ConnectorOptions getter.
     * @return ConnectorOptions used for Connector initialization.
     * */
    const ConnectorOptions& get_options() const;


    /*!
     * @brief Non-throwing Callback executor.
     *
     * @param[in] request HTTP Request object.
     * @param[in] response HTTP Response object.
     */
    void handle(const Request& request, Response& response) noexcept;


    /*!
     * @brief Checks if HTTP request redirection to other Connector is enabled.
     * @return true if HTTP Request redirection to other Connector is enabled.
     */
    bool is_redirect_enabled() const;


private:
    void try_handle(const Request& request, Response& response);

    ConnectorOptions m_options;
    AccessCallback m_access_callback;
    Callback m_callback;
};

/*! Connector Unique Pointer */
using ConnectorUPtr = std::unique_ptr<Connector>;

}
}
}

