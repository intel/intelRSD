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
 *
 * @file command/registration/definition/register_agent.cpp
 * @brief RegisterAgent command definition
 * */

#include "agent-framework/logger_ext.hpp"
#include "agent-framework/exceptions/exception.hpp"

#include "psme/command/registration/definition/tag.hpp"
#include "psme/command/registration/register_agent.hpp"
#include "psme/core/agent/json_agent.hpp"
#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest/server/error/error_factory.hpp"

#include "configuration/configuration.hpp"



using configuration::Configuration;

using namespace psme;
using namespace psme::command;
using namespace psme::rest;

namespace {

const char GAMI_API_VERSION[] = "1.0.0";

std::string capabilities_to_string(const core::agent::Capabilities& caps) {
    std::ostringstream capabilities_stream;
    capabilities_stream << "[";
    auto it = caps.cbegin();
    if (it != caps.cend()) {
        capabilities_stream << it->get_name();
    }
    for (++it; it != caps.cend(); ++it) {
        capabilities_stream << "," << it->get_name();
    }
    capabilities_stream << "]";
    return capabilities_stream.str();
}

}

/*! Register agent implementation */
class RegisterAgent : public registration::RegisterAgent {
public:
    using registration::RegisterAgent::execute;


    RegisterAgent() : registration::RegisterAgent(registration::definition::TAG) {}


    void execute(const Request& request, Response& response) {
        if (!request.is_valid()) {
            throw command::exception::InvalidArgument();
        }

        register_agent(request);
        set_response(response);

        log_info(GET_LOGGER("registration"), "Agent " << request.get_gami_id() << " registered.");
    }


    ~RegisterAgent();


private:
    void set_response(Response& response) {
        const json::Value& configuration = Configuration::get_instance().to_json();
        const auto& eventing_address = configuration["eventing"]["address"].as_string();
        const auto& eventing_port = configuration["eventing"]["port"].as_int();

        response.set_ipv4address(eventing_address);
        response.set_version(GAMI_API_VERSION);
        response.set_port(eventing_port);
    }


private:
    std::string register_agent(const Request& request) {
        auto agent_manager = core::agent::AgentManager::get_instance();

        auto agents = agent_manager->get_agents();
        bool conflict = false;

        log_info(GET_LOGGER("rest"), "Registration: Request.IP = " << request.get_ipv4address()
                                             << " gamiId = " << request.get_gami_id()
                                             << " port = " << request.get_port()
                                             << " capabilities = "
                                             << capabilities_to_string(request.get_capabilities()));

        for (const auto& agent: agents) {
            // check if ip:port is not already taken by other agent
            if (agent->get_ipv4address() == request.get_ipv4address()
                && agent->get_port() == request.get_port()) {
                conflict = true;
            }
            // is it reconnect?
            if (conflict
                && agent->get_gami_id() == request.get_gami_id()
                && agent->get_version() == GAMI_API_VERSION
                && agent->get_vendor() == request.get_vendor()
                && agent->get_capabilities() == request.get_capabilities()) {
                return agent->get_gami_id();
            }
        }

        if (conflict) {
            auto message = "Other agent is already connected on " +
                           request.get_ipv4address() + ":" + std::to_string(request.get_port());

            log_warning(GET_LOGGER("rest"), message);
            throw error::ServerException(error::ErrorFactory::create_conflict_error(message));
        }

        auto agent = std::make_shared<core::agent::JsonAgent>(request.get_gami_id(),
                                                              request.get_ipv4address(),
                                                              request.get_port());

        agent->m_version = request.get_version();
        agent->m_vendor = request.get_vendor();
        agent->m_capabilities = request.get_capabilities();

        agent_manager->add_agent(agent);
        return agent->get_gami_id();
    }
};


RegisterAgent::~RegisterAgent() {}


static Command::Register<RegisterAgent> g;
