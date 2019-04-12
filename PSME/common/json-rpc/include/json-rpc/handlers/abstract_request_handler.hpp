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
 * @file handlers/abstract_request_handler.hpp
 */

#pragma once

#include <string>
#include <memory>

namespace json_rpc {

/*!
 * @brief Represent abstraction of handlers used by AbstractServerConnectors to handle incoming requests
 */
class AbstractRequestHandler {
public:

    AbstractRequestHandler();
    virtual ~AbstractRequestHandler();

    /*!
     * @brief Handle request
     * @param input Request message
     * @return Request response
     */
    virtual std::string handle(const std::string& input) = 0;
};

using AbstractRequestHandlerPtr = std::shared_ptr<AbstractRequestHandler>;

}
