/*!
 * @brief Switch Info Impl class declaration.
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
 * @file switch_info_impl.hpp
 */

#pragma once

#include "agent-framework/module/enum/network.hpp"
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "hal/switch_info.hpp"

namespace agent {
namespace network {
namespace hal {


/*!
 * @brief Switch Info.
 */
class SwitchInfoImpl : public SwitchInfo {
public:

    using VlanId = std::uint32_t;
    using ProtocolId = agent_framework::model::enums::TransportLayerProtocol;

    /*!
     * @brief Default constructor.
     * @param[in] mgmt_port Management port.
     */
    SwitchInfoImpl(const std::string& mgmt_port);

    /*!
     * @brief Copy constructor.
     * @param[in] orig object to be copied.
     */
    SwitchInfoImpl(const SwitchInfoImpl& orig) = delete;

    /*!
     * @brief Copy operator
     * @param[in] orig object to be copied.
     */
    SwitchInfoImpl& operator=(const SwitchInfoImpl &orig) = delete;

    /*!
     * @brief Default destructor.
     */
    virtual ~SwitchInfoImpl();

    /*!
     * @brief Reads information about switch from the HW.
     */
    void read_switch_info();

    /*!
     * @enum PortMode
     * @brief Port interface mode.
     *
     * @var PortMode SwitchInfoImpl::LINK_AGGREGATION_STATIC
     * Link aggregation static mode (loadbalance).
     *
     * @var PortMode SwitchInfoImpl::LINK_AGGREGATION_DYNAMIC
     * Link aggregation dynamic mode (lacp).
     *
     */
    enum class PortMode {
        LINK_AGGREGATION_STATIC,
        LINK_AGGREGATION_DYNAMIC
    };

    /*! brief Logical port type command string. */
    static const constexpr char* STR_LINK_PORT_TYPE_LINK_AGGREGATION = "LinkAggregation";

    /*!
     * @brief Sets PFC enabled attribute.
     * @param[in] enable PFC state.
     * */
    void set_switch_pfc_enabled(bool state);

    /*!
     * @brief Sets LLDP enabled attribute.
     * @param[in] enable LLDP state.
     * */
    void set_switch_lldp_enabled(bool state);

    /*!
     * @brief Adds switch vlan.
     * @param[in] vlan_id Vlan index.
     * */
    void add_switch_vlan(VlanId vlan_id);

    /*!
     * @brief Deletes switch vlan.
     * @param[in] vlan_id Vlan index.
     * */
    void delete_switch_vlan(VlanId vlan_id);

    /*!
     * @brief Check if the VLAN id exists on any port.
     * @param[in] vlan_id Vlan index.
     * */
    bool switch_vlan_exists(VlanId vlan_id);

    /*!
     * @brief Add QoS application protocol.
     * */
    void add_switch_qos_application_protocol(
            ProtocolId protocol, std::uint32_t port, std::uint32_t priority);

    /*!
     * @brief Delete QoS application protocol.
     * */
    void delete_switch_qos_application_protocol(
            ProtocolId protocol, std::uint32_t port, std::uint32_t priority);

    /*!
     * @brief Add QoS bandwidth allocation.
     * */
    void add_switch_qos_bandwidth_allocation(
            std::uint32_t bandwidth_percent, std::uint32_t priority_group);

    /*!
     * @brief Delete QoS bandwidth allocation.
     * */
    void delete_switch_qos_bandwidth_allocation(
            std::uint32_t bandwidth_percent, std::uint32_t priority_group);

    /*!
     * @brief Add QoS priority group mapping.
     * */
    void add_switch_qos_priority_group_mapping(
            std::uint32_t priority, std::uint32_t priority_group);

    /*!
     * @brief Delete QoS priority group mapping.
     * */
    void delete_switch_qos_priority_group_mapping(
            std::uint32_t priority, std::uint32_t priority_group);

    /*!
     * @brief Updates QoS PFC state on a switch.
     * If current state matches new - switch pfc setting will be kept untouched.
     * If current state differs from new - new pfc setting will be set (current will be overwritten).
     * @param[in] current_pfc_state PFC configuration running on switch
     * @param[in] new_pfc_state PFC configuration to be set
     */
    void update_switch_pfc_enabled(bool current_pfc_state, bool new_pfc_state);

    /*!
     * @brief Updates QoS LLDP state on a switch.
     * If current state matches new - switch lldp setting will be kept untouched.
     * If current state differs from new - new lldp setting will be set (current will be overwritten).
     * @param[in] current_lldp_state PFC configuration running on switch
     * @param[in] new_lldp_state PFC configuration to be set
     */
    void update_switch_lldp_enabled(bool current_lldp_state, bool new_lldp_state);

    /*!
     * @brief Updates QoS Application Protocol configuration on a switch.
     * If current config matches new - switch parameters will be kept untouched.
     * If current config differs from new - new parameters will be set, currently running will be deleted.
     * @param[in] current_application_mappings Application Protocol configuration running on switch
     * @param[in] new_application_mappings Application Protocol configuration to be set
     */
    void update_switch_qos_application_protocol(
            const agent_framework::model::attribute::Array<
                  agent_framework::model::attribute::QosApplicationProtocol>& current_application_mappings,
            const agent_framework::model::attribute::Array<
                  agent_framework::model::attribute::QosApplicationProtocol>& new_application_mappings);

    /*!
     * @brief Updates QoS Priority to Priority Group mapping on a switch.
     * If current config matches new - switch parameters will be kept untouched.
     * If current config differs from new - new parameters will be set, currently running will be deleted.
     * @param[in] current_priority_mappings Priority mapping running on switch
     * @param[in] new_priority_mappings Priority mapping to be set
     */
    void update_switch_qos_priority_group_mapping(
            const agent_framework::model::attribute::Array<
                  agent_framework::model::attribute::QosPriorityGroupMapping>& current_priority_mappings,
            const agent_framework::model::attribute::Array<
                  agent_framework::model::attribute::QosPriorityGroupMapping>& new_priority_mappings);

    /*!
     * @brief Updates QoS Bandwidth Allocation on a switch.
     * If current config matches new - switch parameters will be kept untouched.
     * If current config differs from new - new parameters will be set, currently running will be deleted.
     * @param[in] current_bandwidth_allocations Bandwidth Allocation configuration running on switch
     * @param[in] new_bandwidth_allocations Bandwidth Allocation configuration to be set
     */
    void update_switch_qos_bandwidth_allocation(
            const agent_framework::model::attribute::Array<
                  agent_framework::model::attribute::QosBandwidthAllocation>& current_bandwidth_allocations,
            const agent_framework::model::attribute::Array<
                  agent_framework::model::attribute::QosBandwidthAllocation>& new_bandwidth_allocations);
};

}
}
}
