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
 * @file handlers/json_rpc_request_invoker.hpp
 */

#pragma once

#include "json-rpc/handlers/abstract_request_invoker.hpp"
#include "json-rpc/common.hpp"
#include "json-rpc/json_rpc_request.hpp"

namespace json_rpc {

/*!
 * @brief Implements JsonRpc request invoker, this invoker may be used for single Json RPC calls.
 * Batch calls are not supported.
 */
class JsonRpcRequestInvoker : public AbstractRequestInvoker {
public:

    JsonRpcRequestInvoker(): m_request(JsonRpcRequest::make_notification_request({})) {}

    virtual ~JsonRpcRequestInvoker() {}

    /*!
     * @brief Prepares a method request that may be called later
     * @param method Method name
     * @param params Method parameters
     */
    void prepare_method(const std::string& method, const json::Json& params);

    /*!
     * @brief Prepares a notification request that may be called later
     * @param method Notification name
     * @param params Notification parameters
     */
    void prepare_notification(const std::string& method, const json::Json& params);

    /*!
     * @brief Perform client call using the provided connector, throws on errors
     * @param connector Connector to be used
     */
    virtual void call(AbstractClientConnectorPtr connector) override;

    /*!
     * @brief Gets the result of the last method call
     * @return Results of the last method call
     */
    const json::Json& get_result() {
        return m_result;
    }

private:

    long m_last_id = 0;

    JsonRpcRequest m_request;
    json::Json m_result{};

    bool m_is_notification{};

};

using JsonRpcRequestInvokerPtr = std::shared_ptr<JsonRpcRequestInvoker>;

}
