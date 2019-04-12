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
 *
 *
 * @file agent_manager.hpp
 *
 * @brief AgentManager inteface for storing registered agents
 * */

#ifndef PSME_AGENT_MANAGER_HPP
#define PSME_AGENT_MANAGER_HPP

#include "json_agent.hpp"

#include "agent-framework/generic/singleton.hpp"
#include <unordered_map>
#include <stdexcept>
#include <mutex>

namespace psme {
namespace core {
/*! Register Agents */
namespace agent {

/*!
 * @brief AgentManager store agents after registration
 */
class AgentManager : public agent_framework::generic::Singleton<AgentManager> {
public:
    AgentManager() = default;
    virtual ~AgentManager();

    /*! Agents list */
    using AgentsList = std::vector<JsonAgentSPtr>;

    /*!
     * @brief Get agent service for given id
     *
     * @param gami_id agent id
     *
     * @return On successed pointer to agent service is returned, otherwise
     * nullptr
     */
    JsonAgentSPtr get_agent(const std::string& gami_id) const;


    /*!
     * @brief Return agents with given capability
     *
     * @param capability agent capability
     *
     * @return Agents list
     */
    AgentsList get_agents_by_capability(const std::string& capability);

    /*!
     * @brief Return all registered agents
     * @return Agents list
     */
    AgentsList get_agents();

    /*!
     * @brief Add new agent service object to manager
     *
     * @param agent pointer do new agent object
     */
    void add_agent(JsonAgentSPtr agent);

    // #<{(|!
    //  * @brief Remove disconnected agents
    //  *
    //  * Agent is disconnected if is unavailable for more than timeout seconds
    //  *
    //  * @param timeout Number of unavailability seconds after which agent
    //  *                is considered disconnected
    //  * @return Disconnected agents list
    //  |)}>#
    // AgentsList remove_disconnected_agents(const std::uint32_t timeout);

    /*!
     * @brief Remove agent service
     *
     * @param gami_id agent service id
     */
    void remove_agent(const std::string& gami_id);

    /*!
     * @brief Call GAMI method on agent.
     * @param[in] gami_id Agent's GAMI ID
     * @param[in] request Class representing JSON request
     * @return Response from agent
     */
    template<typename Response, typename Request>
    Response call_method(const std::string& gami_id,
                         const Request& request) const {
        auto agent = get_agent(gami_id);
        if(agent) {
            return agent->execute<Response>(request);
        }

        throw std::runtime_error("Could not find agent for GAMI ID = '" + gami_id + "'!");
    }

private:
    AgentManager(const AgentManager&) = delete;
    AgentManager& operator=(AgentManager&) = delete;
    AgentManager(AgentManager&&) = delete;
    AgentManager& operator=(AgentManager&&) = delete;


    AgentsList m_agents{};
    mutable std::mutex m_mutex{};
};


}
}
}
#endif /* PSME_AGENT_MANAGER_HPP */
