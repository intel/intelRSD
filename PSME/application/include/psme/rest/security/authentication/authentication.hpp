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
 * @file authentication.hpp
 *
 * @brief Declaration of authentication base class for REST server.
 * */

#pragma once



#include <memory>
#include "psme/rest/server/response.hpp"



/*! forward declarations */
struct MHD_Connection;

namespace psme {
namespace rest {
namespace security {
namespace authentication {

/*!
 * @brief Enum for authentication status.
 */
enum class AuthStatus {
    FAIL,
    SUCCESS
};

/*!
 * @brief Authentication represents authentication method in HTTP and is base class
 * for specific authentication methods.
 *
 * Authentication has one basic method (perform) which performs specific authentication and return authentication status.
 */
class Authentication {

public:
    /*!
     * @brief Performs authentication and returns authentication status.
     *
     * @param connection MHD_Connections struct type from microhttpd library returned by every connection callbacks for
     * each connection.
     * @param url Url of the resource requested by client.
     * @param response Response object to set and send if authentication fails.
     * @return AuthStatus indicating authentication result - FAIL if authentication failed, SUCCESS if succeeded.
     */
    virtual AuthStatus perform(MHD_Connection* connection, const std::string& url, server::Response& response) = 0;

};

/*! Authentication Unique Pointer */
using AuthenticationUPtr = std::unique_ptr<Authentication>;

}
}
}
}
