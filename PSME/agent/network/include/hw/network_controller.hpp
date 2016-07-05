/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file hw/network_controller.hpp
 *
 * @brief Interface for Network Controller. Implement this interface to
 * handle connections and sending/receiving commands.
 * */

#ifndef AGENT_NETWORK_NETWORK_CONTROLLER_HPP
#define AGENT_NETWORK_NETWORK_CONTROLLER_HPP

#include <memory>
#include <map>
#include <vector>

namespace agent {
namespace network {
namespace hw {

 /*!
 * @brief NetworkController interafce.
 * */
class NetworkController {
public:
    using UUID = std::string;
    using UUIDList = std::vector<std::string>;
    using SwitchId = uint32_t;
    using SwitchPortId = uint32_t;
    using SwitchVlanId = uint32_t;
    using SwitchPortIdentifier = std::string;

    /*!
     * @brief Default constructor
     */
    NetworkController();

    /*!
     * @brief Default destructor
     */
    virtual ~NetworkController();

    /* Default switch id */
    static constexpr SwitchId DEFAULT_SWITCH_ID = 0;

    /*!
     * @brief Add physical port.
     * @param[in] switch_uuid Switch UUID.
     * @param[in] port_identifier Port identifier.
     */
    void add_physical_port(const std::string& switch_uuid,
                           const std::string& port_identifier);

    /*!
     * @brief Get port UUID if it's exists.
     * @param[in] port_identifier Port identifier.
     * @param[out] port_uuid Port UUID.
     * @return true if port exists otherwise false.
     */
    bool get_port_uuid_by_identifier(const std::string& port_identifier,
                                            std::string& port_uuid) const;

    /*!
     * @brief Show manager information (debug).
     */
    void show_network_manager_info();
private:
    std::vector<std::string> m_mesh_ports{};

};

}
}
}
#endif	/* AGENT_NETWORK_NETWORK_CONTROLLER_HPP */
