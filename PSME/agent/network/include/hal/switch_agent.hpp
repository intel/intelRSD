/*!
 * @brief Switch Agent class declaration.
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
 * @file switch_agent.hpp
 */

#pragma once

#include "agent-framework/generic/singleton.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <vector>
#include <string>

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Switch agent
 */
class SwitchAgent : public agent_framework::generic::Singleton<SwitchAgent> {
public:
    /*!
     * @brief Default destructor.
     **/
    virtual ~SwitchAgent();

    /*!
     * @brief Start the agent.
     **/
    void start();

    /*!
     * @brief Waits until switch agent is initialized.
     **/
    void wait_until_initialized();

    /*!
     * @brief Stop the agent.
     **/
    void stop();

    /*!
     * @brief Get switch port list.
     * @return port list
     **/
    virtual std::vector<std::string> get_port_list() const;

    /*!
     * @brief Discover port neighbor mac addresses.
     **/
    virtual void discover_port_neighbor_mac_addr() const;

    /*!
     * Get running configuration from the switch
     * This works only with with eos_aepi
     * @return switch configuration in a json format
     */
    virtual json::Json get_running_config() const;

    /*!
     * @brief Get info about PFC enabled or disabled
     * @return true is global pfc is enabled, false otherwise
     */
    virtual bool is_switch_pfc_enabled() const;

private:
    friend class agent_framework::generic::Singleton<SwitchAgent>;
    SwitchAgent() {}
    bool m_started{false};
};

}
}
}
