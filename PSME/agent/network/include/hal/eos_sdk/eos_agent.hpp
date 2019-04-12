/*!
 * @brief EOS Client class declaration.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file eos_agent.hpp
 */

#pragma once

#include "agent-framework/generic/singleton.hpp"

#include <eos/sdk.h>
#include <eos/agent.h>
#include <eos/intf.h>
#include <eos/eth_phy_intf.h>
#include <eos/eth_intf.h>
#include <eos/mac_table.h>

#include <thread>
#include <condition_variable>
#include <unordered_map>
#include <string>
#include <vector>

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Eos agent.
 */
class EosAgent : public eos::agent_handler,
                 public eos::intf_handler,
                 public eos::eth_intf_handler,
                 public eos::eth_phy_intf_handler,
                 public eos::mac_table_handler,
                 public agent_framework::generic::Singleton<EosAgent> {
public:
    /*!
     * @brief Default destructor.
     */
    virtual ~EosAgent();

    /*!
     * @brief Callback called from Eos SDK when the SDK is initialized.
     */
    void on_initialized() override;

    /*!
     * @brief Waits until switch agent is initialized.
     */
    void wait_until_initialized();

    /*!
     * @brief Starts switch agent.
     */
    void start();

    /*!
     * @brief Stops switch agent.
     */
    void stop();

    /*!
     * @brief Get switch port list.
     * @return port list
     */
    std::vector<std::string> get_port_list() const;

    /*!
     * @brief Discover port neighbot mac addresses.
     */
    void discover_port_neighbor_mac_addr() const;

    /*!
     * @brief Check admin state of a port.
     *
     * @param intf_name interface name
     *
     * @return true if enabled (port is Up), false otherwise
     */
    bool is_admin_enabled(const std::string& intf_name) const;

    /*!
     * @brief Check operational state of a port.
     *
     * @param intf_name interface name
     *
     * @return true if port is operational Up, false otherwise
     */
    bool is_operational_up(const std::string& intf_name) const;

    /*!
     * @brief Set administrative state of a port.
     *
     * @param intf_name interface name
     * @param enable enable value
     *
     * @return true if port is operational Up, false otherwise
     */
    void set_admin_enable(const std::string& intf_name, const bool enable);

    /*!
     * @brief Get port speed.
     *
     * @param intf_name interface name
     *
     * @return port speed in MBPS
     */
    uint32_t get_speed(const std::string& intf_name);

    /*!
     * @brief Call when a unicast MAC address is added to the MAC table.
     *
     * @param entry entry added
     */
    void on_mac_entry_set(eos::mac_entry_t const & entry);

    /*!
     * @brief Get tagged VLANs on a port. If port is not in trunk mode then
     * empty list is returned.
     *
     * @param intf_name interface name
     *
     * @return vector of VLAN IDs
     */
    std::vector<uint16_t> get_tagged_vlans(const std::string& intf_name);

    /*!
     * @brief Get default VLAN on a port. If port is not in access mode then
     * zero is returned.
     *
     * @param intf_name interface name
     *
     * @return 0 or VLAN ID in range 1-4094
     */
    uint16_t get_default_vlan(const std::string& intf_name);

    /*!
     * @brief Set default VLAN on a port. If port is in trunk mode then
     * previous native VLAN is removed from allowed VLANs and new one is added.
     *
     * @param intf_name interface name
     * @param vid VLAN ID in range 1-4094
     */
    void set_default_vlan(const std::string& intf_name, uint16_t vid);

    /*!
     * @brief Add tagged VLAN to a port. If port is not in trunk mode then
     * exception is thrown.
     *
     * @param intf_name interface name
     * @param vlan_id VLAN ID.
     */
    void add_tagged_vlan(const std::string& intf_name, uint16_t vlan_id);

    /*!
     * @brief Delete tagged VLAN from a port.
     *
     * @param intf_name interface name
     * @param vlan_id VLAN ID.
     */
    void delete_tagged_vlan(const std::string& intf_name, uint16_t vlan_id);

private:
    /*!
     * @brief Default constructor.
     */
    EosAgent();

    friend class agent_framework::generic::Singleton<EosAgent>;
    bool m_started{false};
    bool m_initialized{false};
    std::thread m_thread{};
    std::mutex m_mutex{};
    std::condition_variable m_cv{};
    static constexpr int TIMEOUT_SEC{30};
    std::unordered_map<std::string, uint32_t> m_speed_map{};
};

}
}
}
