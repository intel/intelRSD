/*!
 * @brief Provides class for stabilizing network agent resource tree
 *
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
 * @file network_tree_stabilizer.hpp
 * */



#pragma once



#include "agent-framework/tree_stability/tree_stabilizer.hpp"

#include <string>



namespace agent {
namespace network {

/*!
 * @brief Tree stability managing class for network agent.
 * */
class NetworkTreeStabilizer : public agent_framework::TreeStabilizer {
public:
    /*! Destructor */
    virtual ~NetworkTreeStabilizer();


    /*!
     * Stabilizes compute agent resource tree starting from the module.
     *
     * @param[in] module_uuid Temporary UUID of module to stabilize
     *
     * @return Module persistent UUID
     * */
    virtual const std::string stabilize(const std::string& module_uuid) override;


    /*!
     * Stabilize ethernet switch and all resources below.
     *
     * @param[in] switch_uuid UUID of switch to stabilize
     *
     * @return Switch persistent UUID
     * */
    const std::string stabilize_switch(const std::string& switch_uuid);


    /*!
     * Stabilize switch port and all resources below.
     *
     * @param[in] port_uuid UUID of port to stabilize
     *
     * @return Switch port persistent UUID
     * */
    const std::string stabilize_port(const std::string& port_uuid);


    /*!
     * Stabilize static mac and all resources below.
     *
     * @param[in] static_mac_uuid UUID of static mac to stabilize
     *
     * @return Static MAC persistent UUID
     * */
    const std::string stabilize_static_mac(const std::string& static_mac_uuid);


    /*!
     * Stabilize port VLAN and all resources below.
     *
     * @param[in] port_vlan_uuid UUID of port VLAN to stabilize
     *
     * @return Port VLAN persistent UUID
     * */
    const std::string stabilize_port_vlan(const std::string& port_vlan_uuid);


    /*!
     * Stabilize ACL and all resources below.
     *
     * @param[in] acl_uuid UUID of ACL to stabilize
     *
     * @return ACL persistent UUID
     * */
    const std::string stabilize_acl(const std::string& acl_uuid);


    /*!
     * Stabilize ACL rule and all resources below.
     *
     * @param[in] acl_rule_uuid UUID of ACL rule to stabilize
     *
     * @return ACL rule persistent UUID
     * */
    const std::string stabilize_acl_rule(const std::string& acl_rule_uuid);


    /*!
     * Stabilize chassis and all resources below.
     *
     * @param[in] chassis_uuid UUID of chassis to stabilize
     *
     * @return Chassis persistent UUID
     * */
    const std::string stabilize_chassis(const std::string& chassis_uuid);
};

}
}
