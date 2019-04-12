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
 * @file authentication_factory.hpp
 *
 * @brief Declaration of authentication factory interface for REST server.
 * */

#pragma once



#include "psme/rest/server/connector/connector.hpp"



namespace psme {
namespace rest {
namespace security {
namespace authentication {

/*!
 * @brief Authentication factory.
 *
 * Creates new Authentication instances based on provided options.
 */
class AuthenticationFactory {

public:
    /*! @brief Destructor */
    virtual ~AuthenticationFactory();


    /*!
     * @brief Creates new Authentication instances based on connector options.
     * @param[in] options connector options for authentication.
     * @return Vector of pointers to created Authentication instances.
     */
    std::vector<AuthenticationUPtr> create_authentication(const server::ConnectorOptions& options) const;
};

}
}
}
}
