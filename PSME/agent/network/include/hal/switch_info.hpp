/*!
 * @brief Switch Info class declaration.
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
 * @file switch_info.hpp
 */

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

    using SwitchId = std::uint16_t;
    using PortIdentifier = std::string;
    using PortList = std::vector<std::string>;

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
     * @brief Formats MAC address in the form of XX:XX:XX:XX:XX:XX
     * @param addr MAC address.
     */
    static string format_mac_address(const uint8_t* addr);

    /*!
     * @brief Gets switch id
     * @return switch id.
     */
    SwitchId get_switch_id() const {
        return m_switch_id;
    }

    /*!
     * @brief Set switch id
     * @param[in] switch_id Switch Id.
     */
    void set_switch_id(SwitchId switch_id) {
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
    const PortList& get_port_list() const {
        return m_port_list;
    }

    /*!
     * @brief Set switch port list
     * @param[in] ports switch port list.
     */
    void set_port_list(const PortList& ports) {
        m_port_list = ports;
    }

    /*!
     * @brief Set management port name
     * @param ifname interface name to use.
     */
    void set_management_port(const PortIdentifier& ifname) {
        m_mgmt_port = ifname;
    }

    /*!
     * @brief Get management port
     * @return Management port
     */
    const PortIdentifier& get_management_port() {
        return m_mgmt_port;
    }

    /*!
     * @brief Reads mac address of the management port.
     */
    void read_mgmt_mac_address();

private:
    SwitchId m_switch_id{0};
    bool m_is_enabled{false};
    PortList m_port_list{};
    PortIdentifier m_mgmt_port{};
    uint8_t m_mac_address[ETH_ALEN]{};
};

}
}
}
