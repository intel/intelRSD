/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file generic_client.hpp
 * @brief Declaration of GenericClient class
 * */

#ifndef AGENT_FRAMEWORK_CLIENT_GENERIC_CLIENT_HPP
#define AGENT_FRAMEWORK_CLIENT_GENERIC_CLIENT_HPP

#include <jsonrpccpp/client.h>

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace generic {

/*! @brief Generic Client */
class GenericClient : public jsonrpc::Client {
public:
    /*!
     * @brief Generic Client class constructor.
     *
     * @param conn Reference to handling client object
     * @param type Version of client
     * */
    GenericClient(jsonrpc::IClientConnector &conn,
                  jsonrpc::clientVersion_t type = jsonrpc::JSONRPC_CLIENT_V2)
        : jsonrpc::Client(conn, type) {}

    /*! @brief Generic Client class destructor. */
    virtual ~GenericClient();
};

}
}
#endif /* AGENT_FRAMEWORK_CLIENT_GENERIC_CLIENT_HPP */
