/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file connectors/unix_domain_socket_client_connector.hpp
 */

/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    unixdomainsocketclient.h
 * @date    11.05.2015
 * @author  Alexandre Poirot <alexandre.poirot@legrand.fr>
 * @license See attached LICENSE.txt
 ************************************************************************/

#pragma once



#include "json-rpc/connectors/abstract_client_connector.hpp"
#include "json-rpc/common.hpp"


namespace json_rpc {

/*!
 * @brief Implementation of the AbstractClientConnector based on the Socket
 */
class UnixDomainSocketClientConnector : public AbstractClientConnector {

public:

    /*!
     * @brief Constructs a valid Socket connector
     * @param path default server address
     */
    UnixDomainSocketClientConnector(const std::string& path);


    virtual ~UnixDomainSocketClientConnector();

    /*!
     * @brief Sends client request
     * @param message Message to be send
     * @return Obtained response
     */
    virtual std::string send_request(const std::string& message) override;


private:

    std::string m_path{};

};

}



