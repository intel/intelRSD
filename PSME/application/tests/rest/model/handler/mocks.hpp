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
 * @file mocks.hpp
 * @brief Mocks of objects utilized by GenericHandler object.
 * Mocks must proceed inclusion of generic_handler.hpp
 * */

#pragma once
#include "psme/rest/eventing/event.hpp"
#include "psme/core/agent/agent_unreachable.hpp"

#include "agent-framework/module/requests/common/get_managers_collection.hpp"
#include "agent-framework/module/requests/common/get_tasks_collection.hpp"
#include "agent-framework/module/requests/common/get_collection.hpp"

#include <jsonrpccpp/client/connectors/httpclient.h>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include <assert.h>

#define PSME_CORE_AGENT_JSON_AGENT_HPP
#define PSME_AGENT_MANAGER_HPP

namespace agent_framework {
namespace model {
namespace requests {
class GetManagersCollection;
}
}
}

namespace psme {
namespace core {
namespace agent {

struct JsonAgent {
    template<typename Response, typename Request>
    Response execute(const Request &req) {

        m_requests.push_back(req.get_uuid());

        assert(m_responses.size() > m_rsp_idx);
        Json::Value rr;
        auto response_s = m_responses[m_rsp_idx++];
        if (response_s == "[JsonRpcException]") {
            throw psme::core::agent::AgentUnreachable(get_gami_id());
        }
        std::stringstream ss(response_s);
        ss >> rr;
        return Response::from_json(rr);
    }


    template<typename Response>
    Response execute(const agent_framework::model::requests::GetManagersCollection&) {

        m_requests.push_back("GetManagersCollection");

        assert(m_responses.size() > m_rsp_idx);
        Json::Value rr;
        std::stringstream ss(m_responses[m_rsp_idx++]);
        ss >> rr;
        return Response::from_json(rr);
    }


    template<typename Response>
    Response execute(const agent_framework::model::requests::GetTasksCollection&) {

        m_requests.push_back("GetManagersCollection");

        assert(m_responses.size() > m_rsp_idx);
        Json::Value rr;
        std::stringstream ss(m_responses[m_rsp_idx++]);
        ss >> rr;
        return Response::from_json(rr);
    }


    template<typename Response>
    Response execute(
        const agent_framework::model::requests::GetCollection& req) {

        m_requests.push_back(req.get_uuid());

        m_collections.push_back(
            std::make_pair(req.get_name(), req.get_uuid()));

        assert(m_responses.size() > m_rsp_idx);
        Json::Value rr;
        std::stringstream ss(m_responses[m_rsp_idx++]);
        ss >> rr;
        return Response::from_json(rr);
    }

    const std::string get_gami_id() const { return "gami_id"; }

    void clear() {
        m_requests.clear();
        m_rsp_idx = 0;
        m_responses.clear();
        m_collections.clear();
    }

    std::vector<std::pair<std::string, std::string>> m_collections{};
    std::vector<std::string> m_requests{};
    std::vector<std::string> m_responses{};
    size_t m_rsp_idx{0};
};

typedef std::shared_ptr<JsonAgent> JsonAgentSPtr;

struct AgentManager {
    static AgentManager &get_instance() {
        static AgentManager am;
        return am;
    }

    JsonAgentSPtr get_agent(const std::string& /*gami_id*/) {
        if(!m_agent) {
            m_agent = std::make_shared<JsonAgent>();
        }
        return m_agent;
    }

    void remove_agent(const std::string & /*gami_id*/) {
        if(m_agent) {
            m_agent.reset();
        }
    }

    JsonAgentSPtr m_agent { new JsonAgent() };
};
}
}

namespace rest {
namespace eventing {
namespace manager {

/*!
 * SubscriptionManager mock implementation
 */
class SubscriptionManager {
public:
    /*!
     * @brief Returns instance of singleton
     */
    static SubscriptionManager* get_instance() {
        static SubscriptionManager m;
        return &m;
    }

    /*!
     * @brief Notifies subscribers according to event
     *
     * @param event Event
     */
    void notify(const psme::rest::eventing::Event& event) {
        m_events.emplace_back(event);
    }

    void notify(const psme::rest::eventing::EventVec& events) {
        m_events.reserve(m_events.size() + events.size());
        m_events.insert(m_events.end(), events.begin(), events.end());
    }

    void clear() {
        m_events.clear();
    }

    psme::rest::eventing::EventVec m_events {};
};

}
}
}

}
