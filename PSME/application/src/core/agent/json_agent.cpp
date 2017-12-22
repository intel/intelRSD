/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 */

#include "psme/core/agent/json_agent.hpp"
#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest/eventing/event.hpp"
#include "psme/rest/eventing/manager/subscription_manager.hpp"
#include "psme/rest/endpoints/utils.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "json-rpc/connectors/http_client_connector.hpp"

using namespace agent_framework;
using namespace psme::core::agent;

JsonAgent::JsonAgent(const std::string& gami_id,
                     const std::string& ipv4address,
                     const int port) :
    Agent{gami_id, ipv4address, port},
    m_connector{new json_rpc::HttpClientConnector(make_connection_url(ipv4address, port))},
    m_invoker{new json_rpc::JsonRpcRequestInvoker()},
    m_client{m_connector, m_invoker} {}

JsonAgent::JsonAgent(const std::string& gami_id,
                     const std::string& ipv4address,
                     const int port,
                     const std::string& version,
                     const std::string& vendor,
                     const Capabilities& caps) :
    Agent{gami_id, ipv4address, port, version, vendor, caps},
    m_connector{new json_rpc::HttpClientConnector(make_connection_url(ipv4address, port))},
    m_invoker{new json_rpc::JsonRpcRequestInvoker()},
    m_client{m_connector, m_invoker} {}

JsonAgent::~JsonAgent() {}

std::string JsonAgent::make_connection_url(const std::string& ipv4address, const int port) const {
    return "http://" + ipv4address + ":" + std::to_string(port);
}

void JsonAgent::clean_resource_for_agent() {
    psme::rest::model::handler::HandlerManager::get_instance()->remove_agent_data(get_gami_id());
    log_info(GET_LOGGER("rest"), "Agent's data has been cleared [gami_id: " << m_gami_id << " ]");
}

void JsonAgent::unregister_agent() {
    core::agent::AgentManager::get_instance()->remove_agent(get_gami_id());
    log_info(GET_LOGGER("rest"), "Agent " << m_gami_id << " unregistered");
}

void JsonAgent::execute_in_transaction(const FunctionType& function){
    std::unique_lock<std::mutex> lock(m_transaction_mutex, std::defer_lock);
    auto success = lock.try_lock();
    if (!success) {
        log_info(GET_LOGGER("rest"), "Waiting for an unfinished transaction with agent " << m_gami_id << ".");
        lock.lock();
    }

    function();
}
