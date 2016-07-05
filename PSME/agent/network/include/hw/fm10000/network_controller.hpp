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
 * @file hw/fm10000/network_controller.hpp
 *
 * @brief Implementation of network interface
 * */

#ifndef AGENT_NETWORK_FM10000_NETWORK_CONTROLLER_HPP
#define AGENT_NETWORK_FM10000_NETWORK_CONTROLLER_HPP

#include "hw/network_controller.hpp"
#include <string>

namespace agent {
namespace network {
namespace hw {
namespace fm10000 {

/*!
 * @brief Implementation of NetworkController interface for fm10000 switch
 * This class hides the public interface of Network Controller and provide new, specialized to
 * certain commands.
 * */
class NetworkController: public agent::network::hw::NetworkController {
public:

    /*!
     * Default constructor.
     */
    NetworkController();

    /**
     * Copy constructor disabled.
     * @param orig object to copy.
     */
    NetworkController(const NetworkController& orig) = delete;

    /**
     * Copy operator disabled
     * @param orig object to copy.
     */
    NetworkController& operator=(const NetworkController& orig) = delete;

    /*!
     * @brief Initialize switch ports.
     */
    void init_switch_ports(const std::string& switch_uuid);

    /*!
     * @brief Initialize switch vlan ports.
     */
    void init_switch_vlan_ports();

    /*!
     * @brief Initialize switch ACLs
     * @param switch_uuid switch UUID.
     */
    void init_acls(const std::string& switch_uuid);

    /*!
     * @brief Initialize switch static MACs.
     * @param switch_uuid switch UUID.
     */
    void init_static_macs(const std::string& switch_uuid);

    /*!
     * @brief Initializes fm10000 switch. Need to be run once before any operations.
     */
    void initialize();

    /*!
     * @brief Deinitializes fm10000 switch. Need to be run at the end of the application to free resources.
     */
    void deinitialize();

    /*!
     * @brief Checks if fm10000 switch was initialized.
     *
     * @return true if initialize() was called successfuly, otherwise false.
     */
    bool is_initialized();

    /*!
     * Destructor.
     */
    virtual ~NetworkController();

    /*!
     * @brief Add logical port.
     * @param[in] lag_identifier Lag identifier.
     * @param[in] switch_uuid Switch UUID.
     * @return Logical port UUID.
     */
    UUID add_logical_port(const std::string& lag_identifier,
                           const std::string& switch_uuid);

    /*!
     * @brief Get logical port members list.
     * @param[in] lag_identifier Lag identifier.
     * @return Logical port member list.
     */
    std::vector<std::string> get_logical_port_members(
                            const std::string& lag_identifier) const;

    /*!
     * @brief Get logical port identifier
     * @param[in] port_identifier Port identifier.
     * @return Logical port identifier.
     */
    std::string get_lag_identifier(const std::string& port_identifier) const;

    /*!
     * @brief Check if port is member port.
     * @param[in] port_identifier Switch port identifier.
     * @return Member port = true, otherwise false.
     */
    bool is_member_port(const std::string& port_identifier) const;

    /*!
     * @brief Check if lag is empty.
     * @param[in] lag_identifier Lag identifier.
     * @return Empty = true, otherwise false.
     */
    bool is_logical_port_empty(const std::string& lag_identifier) const;

    /*!
     * @brief Init vlans on the port.
     * @param[in] port_identifier Port identifier.
     */
    void init_switch_vlan_port(const std::string& port_identifier);

    /*!
     * @brief Validate port members.
     * @param[in] ports List of ports to validate.
     * @param[in] lag_port_speed Speed of the port under the Lag.
     *                           INVALID_PORT_SPEED indicates no ports under
     *                           the Lag.
     */
    void validate_port_members(const UUIDList& ports,
                               int lag_port_speed = INVALID_PORT_SPEED);

    /*!
     * @brief Save ACL name for given UUID.
     * @param[in] uuid UUID of the ACL.
     * @param[in] name ACL name.
     */
    void add_acl_name (const std::string& uuid, const std::string& name) {
        m_acl_map[uuid] = name;
    }

    /*!
     * @brief Get ACL name for given ACL UUID if exist.
     * @param[in] uuid UUID of the ACL.
     * @return ACL name if found in the map, UUID if not.
     */
    const std::string& get_acl_name (const std::string& uuid) {
        if (m_acl_map.find(uuid) != m_acl_map.end()) {
            return m_acl_map[uuid];
        }
        return uuid;
    }

private:
    bool m_is_initialized{false};
    /* Invalid port speed */
    static constexpr int INVALID_PORT_SPEED = -1;
    std::map<std::string, std::string> m_acl_map{};
};

}
}
}
}
#endif	/* AGENT_NETWORK_FM10000_NETWORK_CONTROLLER_HPP */
