/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file client_cert_authentication.hpp
 *
 * @brief Declaration of client certificate based authentication for REST server.
 * */

#pragma once



#include "authentication.hpp"
#include "psme/rest/server/http_headers.hpp"



/*! forward declarations */
class MHD_Connection;

namespace psme {
namespace rest {
namespace security {
namespace authentication {

/*!
 * @brief BasicAuthentication represents basic authentication method in HTTP.
 *
 * BasicAuthentication has one basic method (perform) which performs basic authentication and return its status.
 */
class ClientCertAuthentication : public Authentication {

public:
    /*!
     * ClientCertAuthentication constructor.
     * @param hostname std::string containing name of the host for certificate validation.
     */
    ClientCertAuthentication(const std::string& hostname) : m_hostname(hostname) {}


    /*!
     * @brief Performs basic authentication and returns authentication status.
     *
     * @param connection MHD_Connections struct type from microhttpd library returned by every connection callbacks for
     * each connection.
     * @param url Url of the resource requested by client.
     * @param response Response object to set and send if basic authentication fails.
     * @return AuthStatus indicating basic authentication result - FAIL if authentication failed, SUCCESS if succeeded.
     */
    AuthStatus perform(MHD_Connection* connection, const std::string& url, server::Response& response) override;


private:
    std::string m_hostname;
};

}
}
}
}