/*!
 * @brief Switch Port Info Impl class declaration.
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
 * @file agent/network/include/hal/switch_port_info_impl.hpp
 */

#pragma once

#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "hal/switch_port_info.hpp"

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Switch Port Info.
 * */
class SwitchPortInfoImpl : public hal::SwitchPortInfo {
public:
    using PrioritiesList = agent_framework::model::attribute::Array<std::uint32_t>;
    using DcbxState = agent_framework::model::enums::DcbxState;

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

    /*!
     * @brief Sets PFC enabled attribute.
     *
     * @param[in] enable PFC state.
     * */
    void set_switch_port_pfc_enabled(bool state);

    /*!
     * @brief Sets port PFC priorities
     *
     * @param[in] priorities vector of priorities to set
     * @param[in] priorities vector of priorities to remove
     */
    void set_port_pfc_priorities(const std::vector<std::uint32_t>& to_set,
                                 const std::vector<std::uint32_t>& to_remove);

    /*!
     * @brief Update LLDP enabled attribute.
     *
     * @param[in] current_lldp_state current LLDP state.
     * @param[in] new_lldp_state new LLDP state.
     * */
    void update_switch_port_lldp_enabled(bool current_lldp_state, bool new_lldp_state);

    /*!
     * @brief Update DCBX State attribute.
     *
     * @param[in] current_dcbx_state current DCBX state.
     * @param[in] new_dcbx_state new DCBX state.
     * */
    void update_switch_port_dcbx_state(const DcbxState current_dcbx_state, const DcbxState new_dcbx_state);

    /*!
     * @brief Update PFC enabled attribute.
     *
     * @param[in] current_pfc_state current PFC state.
     * @param[in] new_pfc_state new PFC state.
     * */
    void update_switch_port_pfc_enabled(bool current_pfc_state, bool new_pfc_state);

    /*!
     * @brief Updates QoS PFC enabled (no-drop) priorities on a port.
     * If current priority settings matches new - port pfc settings will be kept untouched.
     * If current priority settings differs from new - new pfc settings will be set (current will be overwritten).
     * @param[in] current_pfc_priorities PFC configuration running on switch
     * @param[in] new_pfc_priorities PFC configuration to be set
     */
    void update_switch_port_pfc_priorities(const PrioritiesList& current_pfc_priorities,
                                           const PrioritiesList& new_pfc_priorities);

private:

    /*!
     * @brief Analyze list of requested and present priorities and return lists to set and to remove
     * @param[in] requested list of requested priorities
     * @param[in] existing list of existing priorities
     * @return tuple of lists of priorities to set and to remove
     */
    std::tuple<std::vector<std::uint32_t>, std::vector<std::uint32_t>>
    get_priorities_to_set_to_remove(const PrioritiesList& requested,
                                    const PrioritiesList& existing);

    /*!
     * @brief Get vlan list on the port.
     * */
    void get_switch_vlan_list();

    /*!
     * @brief Get default vlan id on the port.
     * */
    void get_switch_port_default_vid();

    /*!
     * @brief Gets switch port link state.
     * */
    void get_switch_port_link_state();

    /*!
     * @brief Sets switch port link state.
     * */
    void set_switch_port_link_state();

    /*!
     * @brief Gets port speed from the switch.
     *
     * @return Port speed.
     * */
    uint32_t get_switch_port_speed();

    /*!
     * @brief Gets PFC enabled state.
     *
     * @return PFC enabled state.
     * */
    void get_switch_port_pfc_enabled();

    /*!
     * @brief Get neighbor port identifier.
     *
     * @return Neighbor port identifier.
     */
    std::string get_neigh_port_identifier() const;

    /*!
     * @brief Sets LLDP enabled attribute.
     *
     * @param[in] enable LLDP state.
     * */
    void set_switch_port_lldp_enabled(bool state);

    /*!
     * @brief Sets DCBX State attribute.
     *
     * @param[in] dcbx_state DCBX state to set
     * */
    void set_switch_port_dcbx_state(DcbxState dcbx_state);

    PortIdentifier m_port_identifier;
    int m_ifindex{0};
};

}
}
}
