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
 *
 * @file switch_info.hpp
 *
 * @brief Switch port parameters
 * */

#pragma once
#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include <cstring>

#include <linux/if_ether.h>

using std::uint8_t;
using std::uint32_t;
using std::string;

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Switch info. Need to set/get switch information.
 */
class SwitchInfo {
public:

    using PortIdentifier = std::string;

    /*!
     * @brief Default constructor.
     */
    SwitchInfo() = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~SwitchInfo();

    /*!
     * @brief Gets CPU port MAC address
     * @return CPU port MAC address.
     */
    string get_mac_address() const {
        return format_mac_address(m_mac_address);
    }

    /*!
     * @brief Sets CPU MAC address
     * @param addr CPU port MAC address.
     */
    void set_mac_address(const uint8_t* addr);

    /*!
     * @brief Reads information about switch from the HW.
     */
    virtual void read_switch_info() = 0;

    /*!
     * @brief Read switch port from the HW.
     */
    virtual void read_switch_port_list() = 0;

    /*!
     * @brief Formats MAC address in the form of XX:XX:XX:XX:XX:XX
     * @param addr MAC address.
     */
    static string format_mac_address(const uint8_t* addr);

    /*!
     * @brief Gets switch id
     * @return switch id.
     */
    uint16_t get_switch_id() const {
        return m_switch_id;
    }

    /*!
     * @brief Set switch id
     * @param[in] switch_id Switch Id.
     */
    void set_switch_id(uint16_t switch_id) {
        m_switch_id = switch_id;
    }

    /*!
     * @brief Gets switch enabled flag
     * @return enabled flag.
     */
    bool get_is_enabled() const {
        return m_is_enabled;
    }

    /*!
     * @brief Sets switch enabled flag
     * @param is_enabled switch enabled plag.
     */
    void set_is_enabled(const bool is_enabled) {
        m_is_enabled = is_enabled;
    }

    /*!
     * @brief Get switch port list
     * @return Port list.
     */
    const std::vector<std::string>& get_port_list() const {
        return m_port_list;
    }

    /*!
     * @brief Set switch port list
     * @param[in] ports switch port list.
     */
    void set_port_list(const std::vector<std::string>& ports) {
        m_port_list = ports;
    }

    /*!
     * @brief Set management port name
     * @param ifname interface name to use.
     */
    void set_management_port(const std::string& ifname) {
        m_mgmt_port = ifname;
    }

    /*!
     * @brief Get management port
     * @return Management port
     */
    const std::string& get_management_port() {
        return m_mgmt_port;
    }

    /*!
     * @brief Reads mac address of the management port.
     */
    void read_mgmt_mac_address();

private:
    uint16_t m_switch_id{0};
    bool m_is_enabled{false};
    uint8_t m_mac_address[ETH_ALEN]{};
    std::string m_mgmt_port{};
    std::vector<std::string> m_port_list{};
};

}
}
}
