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
 * @file switch_port_info_impl.hpp
 *
 * @brief Switch port parameters
 * */

#pragma once

#include "hal/switch_port_info.hpp"

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Switch Port Info.
 * */
class SwitchPortInfoImpl : public hal::SwitchPortInfo {
public:

    /*!
     * @brief Default constructor.
     *
     * @param[in] port Port identifier.
     * */
    SwitchPortInfoImpl(const PortIdentifier& port);

    /*!
     * @brief Copy constructor.
     *
     * @param[in] orig object to be copied.
     * */
    SwitchPortInfoImpl(const SwitchPortInfoImpl& orig) = delete;

    /*!
     * @brief Copy operator
     *
     * @param[in] orig object to be copied.
     * */
    SwitchPortInfoImpl& operator=(const SwitchPortInfoImpl &orig) = delete;

    /*!
     * @brief Default destructor.
     * */
    virtual ~SwitchPortInfoImpl();

    /*!
     * @brief Gets port attribute from a switch.
     *
     * @param[in] attr Port attribute to get.
     * @param[out] value Return value.
     * */
    void get_switch_port_attribute(PortAttributeType attr,
                                   PortAttributeValue& value);

    /*!
     * @brief Sets port attribute on a switch.
     *
     * @param[in] attr Port attribute to set.
     * @param[in] value Value to set.
     * */
    void set_switch_port_attribute(PortAttributeType attr,
                                   const PortAttributeValue& value);

    /*!
     * @brief Gets port identifier.
     *
     * @return Port identifier.
     * */
    const PortIdentifier& get_port_identifier() const {
        return m_port_identifier;
    }

    /*!
     * @brief Add member to the interface.
     *
     * @param[in] port Logical Port interface to be added to.
     * @param[in] member port interface to add.
     * */
    static void add_member(const PortIdentifier& port, const PortIdentifier& member);

    /*!
     * @brief Remove member from the interface.
     *
     * @param[in] member port interface to remove from.
     * */
    static void remove_member(const PortIdentifier& member);

    /*!
     * @brief Get master port identifier.
     *
     * @return Master port identifier.
     * */
    PortIdentifier get_master_port() const;

    /*!
     * @brief Get member identifiers on master port.
     *
     * @return Member identifiers.
     * */
    PortList get_port_members() const;

    /*!
     * @brief Get vlan info list on the port.
     *
     * @return Vlan info list.
     * */
    VlanInfoList get_vlans() const;

    /*!
     * @brief Check if port is member port.
     *
     * @return true if port is member otherwise false.
     * */
    bool is_member() const;

    /*!
     * @brief check if VLAN exists on the port.
     *
     * @param[in] vid VLAN id to check.
     *
     * @return true if VLAN exists.
     * */
    bool vlan_exists(VlanId vid) const;

    /*!
     * @brief Get switch port duplex attribute.
     *
     * @return Port duplex.
     * */
    Duplex get_switch_port_duplex();

private:

    using SwAttr = uint32_t;
    using SwAttrValue = uint32_t;

    /*!
     * @brief Sets port attribute on a switch thu Netlink.
     *
     * @param[in] swattr Port attribute to set.
     * @param[in] value Value to set.
     * */
    void set_switch_port_attr(SwAttr swattr, SwAttrValue value);

    /*!
     * @brief Get switch port max frame size attribute.
     *
     * @return Port mac frame size.
     * */
    uint32_t get_switch_port_max_frame_size();

    /*!
     * @brief Get switch port autoneg attribute.
     *
     * @return Port autoneg (true/false).
     * */
    bool get_switch_port_autoneg();

    /*!
     * @brief Set switch port autoneg attribute.
     *
     * @param[in] enable Port auto-negotiation.
     * */
    void set_switch_port_autoneg(bool enable);

    /*!
     * @brief Get vlan list on the port.
     * */
    void get_switch_vlan_list();

    /*!
     * @brief Get default vlan id on the port.
     * */
    void get_switch_port_default_vid();

    /*!
     * @brief Gets switch port link state thru Netlink api.
     * */
    void get_switch_port_link_state();

    /*!
     * @brief Sets switch port link state thru Netlink api.
     * */
    void set_switch_port_link_state();

    /*!
     * @brief Gets port speed from a switch thru Netlink api.
     *
     * @return Port speed.
     * */
    uint32_t get_switch_port_speed();

    /*!
     * @brief Sets port speed on a switch thru Netlink api.
     *
     * @param[in] speed Port speed.
     * */
    void set_switch_port_speed(uint32_t speed);

    /*!
     * @brief Get neighbor port identifier.
     *
     * @return Neighbor port identifier.
     */
    std::string get_neigh_port_identifier() const;

    PortIdentifier m_port_identifier;
    int m_ifindex{0};
};

}
}
}
