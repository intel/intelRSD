/*!
 * @brief Remote Switch Info class declaration.
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
 * @file remote_switch_info.hpp
 */

#pragma once

#include "agent-framework/module/model/remote_ethernet_switch.hpp"

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Remote switch info. Need to set switch id and port number.
 */
class RemoteSwitchInfo {
public:
    /*!
     * @brief Default constructor.
     */
    RemoteSwitchInfo();

    /*!
     * @brief Default destructor.
     */
    virtual ~RemoteSwitchInfo();

    /*!
     * @brief Loads neighbor switches ids and store them in neighbor manager.
     * @param[in] id Switch id
     */
    virtual void load_neighbor_switches_id(const std::string& id) = 0;

    /*!
     * @brief Fills response with switch info.
     * @param id Id of the remote switch.
     * @param response Neighbor info
     */
    virtual void fill_response_with_switch_info(const std::string& id,
                                                agent_framework::model::
                                                RemoteEthernetSwitch& response) = 0;

    /*!
     * @brief Get remote switch identifier on a specified port.
     * @param switch_uuid local switch uuid.
     * @param port_identifier port identifier on which to look for neighbor.
     * @return remote switch identifier found on the port.
     */
    virtual std::string get_neighbor_on_port(const std::string& switch_uuid,
                                      const std::string& port_identifier) = 0;
};

}
}
}
