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
 * @file connectors/http_server_connector.hpp
 */

/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    httpserver.h
 * @date    31.12.2012
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#pragma once



#include "json-rpc/connectors/abstract_server_connector.hpp"

#include <microhttpd.h>

#include <map>



namespace json_rpc {

/*!
 * @brief Implementation of the AbstractServerConnector based on the HTTP
 */
class HttpServerConnector : public AbstractServerConnector {
public:

    /*! Default number of threads to be used by the server */
    static constexpr unsigned DEFAULT_THREADS_NUM = 50;


    /*!
     * @brief Constructs a valid HTTP connector
     * @param port Port to listen on
     * @param threads Number of threads to be used by the connector
     */
    HttpServerConnector(unsigned short port, unsigned threads = DEFAULT_THREADS_NUM);


    /*!
     * @brief Constructs a valid HTTP connector
     * @param port Port to listen on
     * @param address IPv4 address to restrict connection from it
     * @param threads Number of threads to be used by the connector
     * */
    HttpServerConnector(unsigned short port, const std::string& address, unsigned threads = DEFAULT_THREADS_NUM);


    virtual ~HttpServerConnector();


    /*!
     * @brief Starts connector operation
     */
    virtual bool start_listening() override;


    /*!
     * @brief Stops connector operation
     */
    virtual void stop_listening() override;


    /*!
     * @brief Sets handler on a specific url
     * @param url Url
     * @param handler Handler to be set on the url
     */
    virtual void set_url_handler(const std::string& url, AbstractRequestHandlerPtr handler) override {
        m_handlers[url] = handler;
    }


    /*!
     * @brief Gets handler on a specific url
     * @param url Url
     * @return Handler currently set on the url
     */
    virtual AbstractRequestHandlerPtr get_url_handler(const std::string& url) const override {
        if (m_handlers.find(url) != m_handlers.end()) {
            return m_handlers.at(url);
        }
        return nullptr;
    }


private:

    unsigned short m_port{};
    std::string m_address{};
    unsigned m_threads{};
    bool m_is_running{false};
    std::map<std::string, AbstractRequestHandlerPtr> m_handlers{};

    using MHDDaemonUPtr = std::unique_ptr<MHD_Daemon, void (*)(MHD_Daemon*)>;
    MHDDaemonUPtr m_daemon;

};

}
