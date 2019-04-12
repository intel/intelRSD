/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * */

#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/psme_commands.hpp"
#include "agent-framework/exceptions/exception.hpp"

#include "psme/rest/server/error/error_factory.hpp"
#include "psme/core/agent/agent_manager.hpp"

#include "configuration/configuration.hpp"

using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace psme::core::agent;
using namespace psme::rest::error;

using configuration::Configuration;

namespace {

const char GAMI_API_VERSION[] = "1.0.0";

bool are_vectors_same(const std::vector<std::string>& v1, const std::vector<std::string>& v2) {
    return (v1.size() != v2.size()) ? false : std::is_permutation(v1.begin(), v1.end(), v2.begin());
}

std::vector<std::string> capabilities_to_string_vector(const Capabilities& caps) {
    std::vector<std::string> result{};
    for (const auto& cap : caps) {
        result.emplace_back(cap.get_name());
    }
    return result;
}

Capabilities string_vector_to_capabilities(const std::vector<std::string>& str_caps) {
    Capabilities caps{};
    for (const auto& str_cap : str_caps) {
        caps.emplace_back(Capability(str_cap));
    }
    return caps;
}

JsonAgentSPtr get_agent_by_address_and_port(const std::string& address, int port) {

    auto agent_manager = AgentManager::get_instance();

    for (JsonAgentSPtr agent : agent_manager->get_agents()) {
        if (agent->get_ipv4address() == address && agent->get_port() == port) {
            return agent;
        }
    }
    return nullptr;
}

void handle_attach(const Attach::Request& request, Attach::Response& response) {

    if (!request.is_valid()) {
        throw json_rpc::JsonRpcException(json_rpc::common::ERROR_RPC_INVALID_REQUEST, "Invalid data format");
    }

    log_info("rest", "Registration: Request.IP = " << request.get_ipv4_address()
                                         << " gamiId = " << request.get_gami_id()
                                         << " port = " << request.get_port()
                                         << " capabilities = " << request.get_capabilities().to_json().dump());

    JsonAgentSPtr agent = get_agent_by_address_and_port(request.get_ipv4_address(), request.get_port());

    // check if agent was found
    if (agent) {
        auto agent_caps = capabilities_to_string_vector(agent->get_capabilities());

        // check if agent is already registered
        if (agent->get_gami_id() == request.get_gami_id()
            && agent->get_version() == ::GAMI_API_VERSION
            && agent->get_vendor() == request.get_vendor()
            && are_vectors_same(request.get_capabilities().get_array(), agent_caps)) {

            // agent already registered - reconnection - ok
            log_info("rest", "Reconnection detected");
        }
        else {
            // conflict
            std::string message = "Registration conflict: another agent is already connected on " +
                request.get_ipv4_address() + ":" + std::to_string(request.get_port());

            log_warning("rest", message);
            throw ServerException(ErrorFactory::create_conflict_error(message));
        }
    }
    else {
        // agent was not found, register a new one
        agent = std::make_shared<JsonAgent>(
            request.get_gami_id(),
            request.get_ipv4_address(),
            request.get_port(),
            request.get_version(),
            request.get_vendor(),
            string_vector_to_capabilities(request.get_capabilities().get_array())
        );

        AgentManager::get_instance()->add_agent(agent);
        log_info("registration", "Agent " << request.get_gami_id() << " registered.");
    }

    // prepare response
    const json::Json& configuration = Configuration::get_instance().to_json();
    const auto& eventing_address = configuration.value("eventing", json::Json::object()).value("address", std::string{});
    const auto& eventing_port = configuration.value("eventing", json::Json::object()).value("port", int{});

    response.set_ipv4_address(eventing_address);
    response.set_version(::GAMI_API_VERSION);
    response.set_port(eventing_port);
}

}

REGISTER_COMMAND(Attach, ::handle_attach);
