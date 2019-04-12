/*!
 * @brief eAPI Client class declaration.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file eapi_client.hpp
 */

#pragma once
#include "json-rpc/connectors/http_client_connector.hpp"
#include "json-rpc/handlers/json_rpc_request_invoker.hpp"
#include "agent-framework/generic/singleton.hpp"
#include "hal/eos_eapi/eapi_command.hpp"

#include <chrono>
#include <memory>


namespace agent {
namespace network {
namespace hal {

/*!
 * @brief JSONRPC client for communication
 * with Arista switch thru eAPI.
 * */
class EapiClient : public agent_framework::generic::Singleton<EapiClient> {
public:
    /*!
     * @brief Starts switch agent.
     */
    void start();

    /*!
     * @brief Stops switch agent.
     */
    void stop();

    /*!
     * @brief Sends request command
     * @return Response
     * */
    json::Json eapi_request(const EapiCommand& command) const;

    /*!
     * @brief Gets running switch configuration
     * @return Running switch configuration in json format
     */
    json::Json get_running_config() const;

    /*!
     * @brief Cheks whether Global PFC is enabled or disabled on the switch
     * @return true if PFC is enabled, false otherwise
     */
    bool is_switch_pfc_enabled();

    /*!
     * @brief Destructor.
     */
    virtual ~EapiClient();

private:

    /*!
     * @brief Default constructor.
     */
    EapiClient();

    friend class agent_framework::generic::Singleton<EapiClient>;
    bool m_started{false};

    /*!
     * @brief Create connection URL from IPv4 address and port
     * @param ipv4address agent IPv4 address
     * @param port agent port
     * @return  Connection URL
     */
    std::string make_connection_url(const std::string& ipv4address, const int port) const;

    std::string m_url{};
    std::shared_ptr<json_rpc::HttpClientConnector> m_connector{};
    std::shared_ptr<json_rpc::JsonRpcRequestInvoker> m_invoker{};
};

}
}
}

