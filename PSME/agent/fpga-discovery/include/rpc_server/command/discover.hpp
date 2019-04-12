/*!
 * @copyright Copyright (c) 2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file discover.hpp
 */

#pragma once



#include "discovery_agent_context.hpp"
#include "agent-framework/command/context_command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "tools/discovery_entries_provider.hpp"

#include <string>



namespace agent {
namespace fpga_discovery {
namespace command {

namespace request {

constexpr const char DISCOVER_CMD_NAME[] = "discover";
constexpr const char HOST_ID_PROPERTY_NAME[] = "host_id";

/*!
 * @brief Class representing Discover command od Discovery Service
 */
class Discover {
public:
    /*!
     * @brief Constructor
     * @param host_id Uuid of the system which assets need to be discovered
     */
    explicit Discover(const Uuid& host_id) : m_host_id(host_id) {}


    /*!
     * @brief Get command name
     * @return Command name
     */
    static std::string get_command() {
        return DISCOVER_CMD_NAME;
    }


    /*!
     * @brief Get host id
     * @return hst uuid for which the command is called
     */
    const Uuid& get_host_id() const {
        return m_host_id;
    }


    /*!
     * @brief Transform request to Json
     * @return created Json value
     */
    json::Json to_json() const {
        json::Json value{};
        value[HOST_ID_PROPERTY_NAME] = m_host_id;
        return value;
    }


    /*!
     * @brief create AddEndpoint from Json
     * @param[in] json the input argument
     * @return new AddEndpoint
     */
    static Discover from_json(const json::Json& json) {
        return Discover(json[HOST_ID_PROPERTY_NAME].get<std::string>());
    }


    /*!
     * @brief Returns procedure scheme
     * @return ProcedureValidator scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_OBJECT,
            HOST_ID_PROPERTY_NAME, VALID_UUID,
            nullptr
        };
        return procedure;
    }


private:
    Uuid m_host_id{};
};

}

namespace response {

static json::Json entries_to_json(const DiscoveryEntriesProvider::DiscoveryEntries& discovery_entries) {
    json::Json json = json::Json::array();
    for (const auto& entry : discovery_entries) {
        json.emplace_back(entry.to_json());
    }
    return json;
}


class Discover {
public:

    /*!
     * @brief Constructor
     */
    explicit Discover() {}


    /*!
     * @brief Transform request to Json
     * @return created Json value
     */
    json::Json to_json() const {
        json::Json value{};
        value = entries_to_json(m_discovery_entries);
        return value;
    }


    /*!
     * @brief Set discovery entries
     * @param entries discovery entries
     */
    void set_discovery_entries(const DiscoveryEntriesProvider::DiscoveryEntries& entries) {
        m_discovery_entries = entries;
    }


private:
    DiscoveryEntriesProvider::DiscoveryEntries m_discovery_entries{};
};

}

using Discover = ::agent_framework::command::ContextCommand<agent::fpga_discovery::DiscoveryAgentContext,
    agent::fpga_discovery::command::request::Discover,
    agent::fpga_discovery::command::response::Discover>;


/*!
 * @brief Discover command handler
 * @param context Agent context
 * @param request Request
 * @param response Reference to the response
 */
void discover(Discover::ContextPtr context,
              const Discover::Request& request,
              Discover::Response& response);

}
}
}
