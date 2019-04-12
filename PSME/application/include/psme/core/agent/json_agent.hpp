/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * */

#ifndef PSME_CORE_AGENT_JSON_AGENT_HPP
#define PSME_CORE_AGENT_JSON_AGENT_HPP

#include "agent-framework/module/utils/utils.hpp"

#include "agent.hpp"
#include "agent_unreachable.hpp"
#include "rpc_client.hpp"
#include "logger/logger_factory.hpp"

#include <memory>
#include <atomic>
#include <mutex>

/*! Forward declaration */
class RegisterAgent;

namespace psme {
namespace core {
namespace agent {

class JsonAgent : public Agent {
public:
    using FunctionType = std::function<void(void)>;

    static const unsigned REMOVE_AGENT_AFTER_SECONDS = 60;

    JsonAgent(const std::string& gami_id,
              const std::string& ipv4_address,
              const int port);

    JsonAgent(const std::string& gami_id,
              const std::string& ipv4_address,
              const int port,
              const std::string& version,
              const std::string& vendor,
              const Capabilities& caps);

    template <typename Response, typename Request>
    Response execute(const Request& req) {
        std::lock_guard<std::mutex> lock(m_single_request_mutex);
        try {
            auto res = m_client.CallMethod(Request::get_command(), req.to_json());
            m_connection_error_observed_at = std::experimental::nullopt;

            return Response::from_json(res);
        }
        catch (const json_rpc::JsonRpcException& e) {
            if (json_rpc::common::ERROR_CLIENT_CONNECTOR == e.get_code()) {
                auto now = std::chrono::high_resolution_clock::now();
                if (m_connection_error_observed_at) {
                    auto broken_seconds = std::chrono::duration_cast<std::chrono::seconds>(
                        now - m_connection_error_observed_at.value()).count();

                    log_error("rest", "Agent " << get_gami_id() << " has not responded for "
                                                  << broken_seconds << " seconds.");
                    if (broken_seconds >= JsonAgent::REMOVE_AGENT_AFTER_SECONDS) {
                        clean_resource_for_agent();
                        unregister_agent();
                    }
                }
                else {
                    m_connection_error_observed_at = now;
                }

                throw AgentUnreachable(get_gami_id());
            }
            m_connection_error_observed_at = std::experimental::nullopt;
            throw;
        }
    }

    /*!
     * @brief Method unregisters agent from agent manager
     */
    void unregister_agent();

    /*!
     * Execute function within transaction guard
     *
     * @param[in] transaction_type String representing transaction type identifier, for example: "PatchSystem"
     * @param[in] function Callable object
     * */
    void execute_in_transaction(const std::string& transaction_type, const FunctionType& function);

    /*! @brief Destructor. */
    ~JsonAgent();

private:
    friend class ::RegisterAgent;

    /*!
     * @brief Method clears all resources that came from this agent.
     */
    void clean_resource_for_agent();

    /*!
     * @brief Create connection URL from IPv4 address and port
     *
     * @param ipv4_address agent IPv4 address
     * @param port agent port
     *
     * @return Connection URL
     */
    std::string make_connection_url(const std::string& ipv4_address, const int port) const;

    std::experimental::optional<std::chrono::time_point<std::chrono::system_clock>> m_connection_error_observed_at{};
    json_rpc::AbstractClientConnectorPtr m_connector{};
    json_rpc::JsonRpcRequestInvokerPtr m_invoker{};
    RpcClient m_client;
    std::mutex m_single_request_mutex{};
    std::mutex m_transaction_mutex{};
    std::atomic<std::uint64_t> m_transaction_id{};
};

using JsonAgentSPtr = std::shared_ptr<JsonAgent>;

}
}
}
#endif /* PSME_CORE_AGENT_JSON_AGENT_HPP */
