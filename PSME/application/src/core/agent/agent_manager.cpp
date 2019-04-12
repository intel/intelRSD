/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
*/

#include "psme/core/agent/agent_manager.hpp"
#include "psme/core/agent/json_agent.hpp"
#include "logger/logger_factory.hpp"

#include <algorithm>

using namespace psme::core::agent;

AgentManager::~AgentManager() {}

JsonAgentSPtr AgentManager::get_agent(const std::string& gami_id) const {
    std::lock_guard<std::mutex> lock{m_mutex};
    for (const auto& agent : m_agents) {
        if (agent->get_gami_id() == gami_id) {
            return agent;
        }
    }
    return nullptr;
}

void AgentManager::add_agent(JsonAgentSPtr agent) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_agents.push_back(agent);
}

void AgentManager::remove_agent(const std::string& gami_id) {
    std::lock_guard<std::mutex> lock{m_mutex};
    auto comparer = [&gami_id](const JsonAgentSPtr& agent) {
                        return agent->get_gami_id() == gami_id;
                    };

    m_agents.erase(std::remove_if(m_agents.begin(), m_agents.end(), comparer),
                   m_agents.end());
}

AgentManager::AgentsList AgentManager::get_agents_by_capability(
                                            const std::string& capability) {
    std::lock_guard<std::mutex> lock{m_mutex};

    AgentsList agents;
    for (const auto& agent : m_agents) {
        if (agent->has_capability(capability)) {
            agents.push_back(agent);
        }
    }
    return agents;
}

AgentManager::AgentsList AgentManager::get_agents() {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_agents;
}
