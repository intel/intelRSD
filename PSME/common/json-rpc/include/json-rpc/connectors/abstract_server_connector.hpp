/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file connectors/abstract_server_connector.hpp
 */

/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    abstractserverconnector.h
 * @date    31.12.2012
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#pragma once

#include "json-rpc/handlers/abstract_request_handler.hpp"

namespace json_rpc {

/*! Represents an interface for a server connector */
class AbstractServerConnector {
public:

    AbstractServerConnector();

    virtual ~AbstractServerConnector();

    /*!
     * @brief Starts connector operation
     */
    virtual bool start_listening() = 0;

    /*!
     * @brief Stops connector operation
     */
    virtual void stop_listening() = 0;

    /*!
     * @brief Sets handler on a specific url
     * @param url Url
     * @param handler Handler to be set on the url
     */
    virtual void set_url_handler(const std::string& url, AbstractRequestHandlerPtr handler) = 0;

    /*!
     * @brief Gets handler on a specific url
     * @param url Url
     * @return Handler currently set on the url
     */
    virtual AbstractRequestHandlerPtr get_url_handler(const std::string& url) const = 0;
};

using AbstractServerConnectorPtr = std::shared_ptr<AbstractServerConnector>;

}
