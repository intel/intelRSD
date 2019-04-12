/*!
 * @brief RmmTreeStabilizer class interface.
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
 * @file rmm_tree_stabilizer.hpp
 */

#pragma once



#include "agent-framework/tree_stability/tree_stabilizer.hpp"

#include <string>



namespace agent {
namespace rmm {

/*!
 * @brief Tree stability managing class for RMM agent
 */
class RmmTreeStabilizer : public agent_framework::TreeStabilizer {
public:
    /* Destructor */
    virtual ~RmmTreeStabilizer();


    /*!
     * @brief Stabilize chassis and all its children.
     * @param chassis_uuid UUID of chassis to stabilize.
     * @return Chassis persistent UUID.
     */
    const std::string stabilize_chassis(const std::string& chassis_uuid) const;


    /*!
     * @brief Stabilize network interface and all its children.
     * @param network_interface_uuid UUID of network interface to stabilize.
     * @return Network interface persistent UUID.
     */
    const std::string stabilize_network_interface(const std::string& network_interface_uuid) const;


    /*!
     * @brief Stabilize fan and all its children.
     * @param fan_uuid UUID of fan to stabilize.
     * @return Fan persistent UUID.
     */
    const std::string stabilize_fan(const std::string& fan_uuid) const;


    /*!
     * @brief Stabilize port vlan and all its children.
     * @param port_vlan_uuid UUID of port vlan to stabilize.
     * @return Port vlan persistent UUID.
     */
    const std::string stabilize_port_vlan(const std::string& port_vlan_uuid) const;


    /*!
     * @brief Stabilize power zone and all its children.
     * @param power_zone_uuid UUID of power zone to stabilize.
     * @return Power zone persistent UUID.
     */
    const std::string stabilize_power_zone(const std::string& power_zone_uuid) const;


    /*!
     * @brief Stabilize thermal zone and all its children.
     * @param thermal_zone_uuid UUID of thermal zone to stabilize.
     * @return Thermal zone persistent UUID.
     */
    const std::string stabilize_thermal_zone(const std::string& thermal_zone_uuid) const;


    /*!
     * @brief Stabilize psu and all its children.
     * @param psu_uuid UUID of psu to stabilize.
     * @return Psu persistent UUID.
     */
    const std::string stabilize_psu(const std::string& psu_uuid) const;


    /*!
     * @brief Stabilize manager and all its children.
     * @param manager_uuid UUID of manager to stabilize.
     * @return Manager persistent UUID.
     */
    virtual const std::string stabilize(const std::string& manager_uuid) override;
};

}
}
