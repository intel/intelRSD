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
 * @file handlers/abstract_request_invoker.hpp
 */

#pragma once

#include "json-rpc/connectors/abstract_client_connector.hpp"

namespace json_rpc {

/*!
 * @brief Abstraction of a generic request invoker class
 */
class AbstractRequestInvoker {
public:

    AbstractRequestInvoker();

    virtual ~AbstractRequestInvoker();

    /*!
     * @brief Perform client call using the provided connector
     * @param connector Connector to be used
     */
    virtual void call(AbstractClientConnectorPtr connector) = 0;
};

using AbstractRequestInvokerPtr = std::shared_ptr<AbstractRequestInvoker>;

}
