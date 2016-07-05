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
 * @file network_manager.hpp
 * @brief Network manager interface
 * */

#ifndef AGENT_FRAMEWORK_MODULE_NETWORK_MANAGER_HPP
#define AGENT_FRAMEWORK_MODULE_NETWORK_MANAGER_HPP

#include "agent-framework/generic/singleton.hpp"
#include "agent-framework/module-ref/managers/generic_manager.hpp"
#include "agent-framework/module-ref/managers/many_to_many_manager.hpp"
#include "agent-framework/module-ref/managers/utils/manager_utils.hpp"
#include "agent-framework/module-ref/model/chassis.hpp"
#include "agent-framework/module-ref/model/manager.hpp"
#include "agent-framework/module-ref/model/network_interface.hpp"
#include "agent-framework/module-ref/model/port_vlan.hpp"
#include "agent-framework/module-ref/model/remote_switch.hpp"
#include "agent-framework/module-ref/model/switch.hpp"
#include "agent-framework/module-ref/model/switch_port.hpp"
#include "agent-framework/module-ref/model/vlan.hpp"
#include "agent-framework/module-ref/model/acl.hpp"
#include "agent-framework/module-ref/model/acl_rule.hpp"
#include "agent-framework/module-ref/model/static_mac.hpp"

/*! Psme namespace */
namespace agent_framework {
namespace module {

using namespace agent_framework::model;

/*!
 * @brief Class for managing network components and subcomponents
 * */
class NetworkManager : public generic::Singleton<NetworkManager> {
public:
    using ModuleManager = GenericManager<model::Manager>;
    using PortVlanManager = GenericManager<model::PortVlan>;
    using SwitchManager = GenericManager<model::Switch>;
    using PortManager = GenericManager<model::SwitchPort>;
    using VlanManager = GenericManager<model::Vlan>;
    using RemoteSwitchManager =
        GenericManager<model::RemoteSwitch>;
    using ChassisManager = GenericManager<model::Chassis>;
    using PortMembersManager = managers::ManyToManyManager;
    using AclManager = GenericManager<model::Acl>;
    using AclRuleManager = GenericManager<model::AclRule>;
    using AclPortManager = managers::ManyToManyManager;
    using StaticMacManager = GenericManager<model::StaticMac>;

    virtual ~NetworkManager();

    /*!
     * @brief Get module manager
     * @return Module manger
     */
    ModuleManager& get_module_manager() {
        return m_module_manager;
    }

    /*!
     * @brief Get switch manager
     * @return Switch manager
     */
    SwitchManager& get_switch_manager() {
        return m_switch_manager;
    }

    /*!
     * @brief Get port manager
     * @return Port manager
     */
    PortManager& get_port_manager() {
        return m_port_manager;
    }

    /*!
     * @brief Get switch port vlan manager
     * @return Switch port vlan manager
     */
    PortVlanManager& get_port_vlan_manager() {
        return m_port_vlan_manager;
    }

    /*!
     * @brief Get VLAN manager
     * @return VLAN manager
     */
    VlanManager& get_vlan_manager() {
        return m_vlan_manager;
    }

    /*!
     * @brief Get Remote Switch manager
     * @return Remote Switch manager
     */
    RemoteSwitchManager& get_remote_switch_manager() {
        return m_remote_switch_manager;
    }

    /*!
     * @brief Get chassis manager
     * @return Reference to a chassis manager instance
     * */
    ChassisManager& get_chassis_manager() {
        return m_chassis_manager;
    }

    /*!
     * @brief Get port lag manager
     * @return Reference to a lag many-to-many manager instance
     * */
    PortMembersManager& get_port_members_manager() {
        return m_port_members_manager;
    }

    /*!
     * @brief Get ACL manager
     *
     * @return ACL manager
     */
    AclManager& get_acl_manager() {
        return m_acl_manager;
    }

    /*!
     * @brief Get Acl Rule manager
     * @return Acl Rule manager
     */
    AclRuleManager& get_acl_rule_manager() {
        return m_acl_rule_manager;
    }

    /*!
     * @brief Get ACL to Port manager.
     * ACL is ALWAYS a parent, while
     * Port is ALWAYS a child in this relation
     *
     * @return ACL port manager
     */
    AclPortManager& get_acl_port_manager() {
        return m_acl_port_manager;
    }

    /*!
     * @brief Get Static MAC manager.
     *
     * @return Static MAC manager
     */
    StaticMacManager& get_static_mac_manager() {
        return m_static_mac_manager;
    }

private:
    ModuleManager m_module_manager{};
    PortVlanManager m_port_vlan_manager{};
    VlanManager m_vlan_manager{};
    SwitchManager m_switch_manager{};
    PortManager m_port_manager{};
    RemoteSwitchManager m_remote_switch_manager{};
    ChassisManager m_chassis_manager{};
    PortMembersManager m_port_members_manager{};
    AclManager m_acl_manager{};
    AclRuleManager m_acl_rule_manager{};
    AclPortManager m_acl_port_manager{};
    StaticMacManager m_static_mac_manager{};
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_NETWORK_MANAGER_HPP */
