/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file network_components.hpp
 * @brief Network Components interface
 * */

#pragma once
#include "agent-framework/generic/singleton.hpp"
#include "agent-framework/module/managers/generic_manager.hpp"
#include "agent-framework/module/managers/many_to_many_manager.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/module/model/model_network.hpp"

/*! Psme namespace */
namespace agent_framework {
namespace module {

/*!
 * @brief Class for managing network components and subcomponents
 * */
class NetworkComponents : public generic::Singleton<NetworkComponents> {
public:
    using PortVlanManager = GenericManager<model::EthernetSwitchPortVlan>;
    using SwitchManager = GenericManager<model::EthernetSwitch>;
    using PortManager = GenericManager<model::EthernetSwitchPort>;
    using VlanManager = GenericManager<model::Vlan>;
    using RemoteSwitchManager = GenericManager<model::RemoteEthernetSwitch>;
    using PortMembersManager = managers::ManyToManyManager;
    using AclManager = GenericManager<model::Acl>;
    using AclRuleManager = GenericManager<model::AclRule>;
    using AclPortManager = managers::ManyToManyManager;
    using StaticMacManager = GenericManager<model::StaticMac>;

    virtual ~NetworkComponents();


    /*!
     * @brief Get switch manager
     *
     * @return Switch manager
     * */
    SwitchManager& get_switch_manager() {
        return m_switch_manager;
    }

    /*!
     * @brief Get port manager
     *
     * @return Port manager
     * */
    PortManager& get_port_manager() {
        return m_port_manager;
    }

    /*!
     * @brief Get switch port vlan manager
     *
     * @return Switch port vlan manager
     * */
    PortVlanManager& get_port_vlan_manager() {
        return m_port_vlan_manager;
    }

    /*!
     * @brief Get VLAN manager
     *
     * @return VLAN manager
     * */
    VlanManager& get_vlan_manager() {
        return m_vlan_manager;
    }

    /*!
     * @brief Get Remote Switch manager
     *
     * @return Remote Switch manager
     * */
    RemoteSwitchManager& get_remote_switch_manager() {
        return m_remote_switch_manager;
    }


    /*!
     * @brief Get port lag manager
     *
     * @return Reference to a lag many-to-many manager instance
     * */
    PortMembersManager & get_port_members_manager() {
        return m_port_members_manager;
    }

    /*!
     * @brief Get ACL manager
     *
     * @return ACL manager
     * */
    AclManager& get_acl_manager() {
        return m_acl_manager;
    }

    /*!
     * @brief Get Acl Rule manager
     *
     * @return Acl Rule manager
     * */
    AclRuleManager& get_acl_rule_manager() {
        return m_acl_rule_manager;
    }

    /*!
     * @brief Get ACL to Port manager.
     * ACL is ALWAYS a parent, while
     * Port is ALWAYS a child in this relation
     *
     * @return ACL port manager
     * */
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
    PortVlanManager m_port_vlan_manager{};
    VlanManager m_vlan_manager{};
    SwitchManager m_switch_manager{};
    PortManager m_port_manager{};
    RemoteSwitchManager m_remote_switch_manager{};
    PortMembersManager m_port_members_manager{};
    AclManager m_acl_manager{};
    AclRuleManager m_acl_rule_manager{};
    AclPortManager m_acl_port_manager{};
    StaticMacManager m_static_mac_manager{};
};

}
}

