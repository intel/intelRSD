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
 * @file api/switch_vlan.hpp
 *
 * @brief Vlan functionality.
 * */

#ifndef AGENT_NETWORK_SWITCH_VLAN_HPP
#define AGENT_NETWORK_SWITCH_VLAN_HPP

#include <cstdint>
#include <vector>
#include <array>
#include <string>

namespace agent {
namespace network {
namespace api {

/*! Switch Vlan class */
class SwitchVlan {
public:

    using SwitchId = std::uint8_t;
    using VlanId = std::uint16_t;
    using PortId = std::uint16_t;
    using PortIdentifier = std::string;
    using SwitchIdentifier = std::string;
    using VlanList = std::vector<std::string>;

    /*! Vlan port info class */
    class VlanPortInfo {
    private:
        bool m_tagged;
    public:
        /*!
         * @brief Set vlan tag.
         * @param[in] tagged Vlan tag (false=untagged,true=tagged).
         */
        void set_tagged(bool tagged) { m_tagged = tagged; }

        /*!
         * @brief Gets tag info.
         * @return Tag info (false=untagged,true=tagged).
         */
        bool get_tagged() const {
            return m_tagged;
        }
    };

    /*!
     * @brief Default constructor.
     */
    SwitchVlan() = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~SwitchVlan();

    /*!
     * @brief Add port to vlan.
     * @param[in] port Port identifier.
     * @param[in] tagged Vlan tag(false/true).
     */
    void add_port(const PortIdentifier& port, bool tagged);

    /*!
     * @brief Remove port from vlan.
     * @param[in] port Port identifier.
     */
    void remove_port(const PortIdentifier& port);

    /*!
     * @brief Get vlan port information.
     * @param[in] port Port identifier.
     * @param[out] info Vlan port information.
     */
    void get_info(const PortIdentifier& port, VlanPortInfo& info);

    /*!
     * @brief Gets Vlan present flag
     * @return present flag.
     */
    bool get_is_present() const {
        return m_is_present;
    }

    /*!
     * @brief Sets Vlan present flag
     * @param is_present Port present plag.
     */
    void set_is_present(const bool is_present) {
        m_is_present = is_present;
    }

    /*!
     * @brief Gets switch index.
     * @return Switch index.
     */
    SwitchId get_switch_id() const {
        return m_switch_id;
    }

    /*!
     * @brief Set switch id.
     * @param[in] switch_id Switch Id.
     */
    void set_switch_id(SwitchId switch_id) {
        m_switch_id = switch_id;
    }

    /*!
     * @brief Gets vlan index.
     * @return Vlan index.
     */
    VlanId get_vlan_id() const {
        return m_vlan_id;
    }

    /*!
     * @brief Set vlan index.
     * @param[in] vlan_id Vlan Id.
     */
    void set_vlan_id(VlanId vlan_id) {
        m_vlan_id = vlan_id;
    }

protected:
    /*!
     * @brief Add switch port to vlan.
     * @param[in] port Port identifier.
     * @param[in] tagged Vlan tag(false/true).
     */
    virtual void add_switch_port(const PortIdentifier& port, bool tagged);

    /*!
     * @brief Remove switch port from vlan.
     * @param[in] port Port identifier.
     */
    virtual void remove_switch_port(const PortIdentifier& port);

    /*!
     * @brief Get switch vlan port information.
     * @param[in] port Port identifier.
     * @param[out] info Vlan port information.
     */
    virtual void get_switch_info(const PortIdentifier& port, VlanPortInfo& info);

private:
    bool m_is_present{false};
    SwitchId m_switch_id{0};
    VlanId m_vlan_id{0};
};

}
}
}
#endif /* AGENT_NETWORK_SWITCH_VLAN_HPP */
