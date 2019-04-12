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
 * @file connectors/http_client_connector.hpp
 */

/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    httpclient.h
 * @date    02.01.2013
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#pragma once

#include "json-rpc/connectors/abstract_client_connector.hpp"
#include <curl/curl.h>

namespace json_rpc {

/*!
 * @brief Implementation of the AbstractClientConnector based on the HTTP
 */
class HttpClientConnector : public AbstractClientConnector {
public:

    /*! Default timeout (ms) time used by the constructor */
    static constexpr long DEFAULT_TIMEOUT_MS = 10000;

    /*!
     * @brief Constructs a http connector
     * @param url Target url
     * @param timeout_ms Timeout (ms) for connection
     */
    HttpClientConnector(const std::string& url, long timeout_ms = DEFAULT_TIMEOUT_MS);

    virtual ~HttpClientConnector();

    /*!
     * @brief Sends client request
     * @param message Message to be send
     * @return Obtained response
     */
    virtual std::string send_request(const std::string& message) override;

    /*!
     * @brief Sets URL
     * @param url New URL
     */
    void set_url(const std::string& url) {
        m_url = url;
    }

private:

    std::string m_url{};
    long m_timeout_ms{};

    using CurlPtr = std::unique_ptr<CURL, void(*)(CURL*)>;
    CurlPtr m_curl;

};

}
