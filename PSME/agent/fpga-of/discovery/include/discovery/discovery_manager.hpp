/*!
 * @brief Initial discovery implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file discovery_manager.hpp
 * */

#pragma once



#include "agent-framework/discovery/discovery_manager.hpp"
#include "agent-framework/module/model/model_fpgaof.hpp"

#include "fpgaof_agent_context.hpp"
#include "fpgaof_stabilizer.hpp"



namespace agent {
namespace fpgaof {
namespace discovery {

/*!
 * @brief Implementation of initial discovery
 */
class DiscoveryManager : public agent_framework::discovery::DiscoveryManager {
public:

    /*!
     * @brief Constructor for discovery manager.
     * @param context Shared pointer to agent FPGA-oF context.
     * */
    DiscoveryManager(AgentContext::SPtr context) : m_context(context) {}


    /*!
     * @brief Gather information about FPGA and its submodules.
     * @return UUID of the discovered module manager.
     */
    virtual Uuid discover() override;


    /*! @brief Default destructor. */
    virtual ~DiscoveryManager();


protected:

    AgentContext::SPtr m_context{};
    FpgaofStabilizer m_stabilizer{};

    /*!
     * @brief Discover manager.
     * @return Discovered manager object.
     * */
    virtual agent_framework::model::Manager discover_manager();


    /*!
     * @brief Discover chassis.
     * @param manager Reference to parent Manager object.
     * @return Discovered chassis object.
     * */
    virtual agent_framework::model::Chassis discover_chassis(const agent_framework::model::Manager& manager);


    /*!
     * @brief Discover fabrics.
     * @param manager Reference to parent Manager object.
     * @return Discovered fabric object.
     * */
    virtual agent_framework::model::Fabric discover_fabric(const agent_framework::model::Manager& manager);


    /*!
     * @brief Discover system's ethernet interfaces.
     * @param[in] system Reference to parent System object.
     * @return Discovered collection of system's ethernet interface objects.
     * */
    virtual std::vector<agent_framework::model::NetworkInterface> discover_ethernet_interfaces(
        const agent_framework::model::System& system);


    /*!
     * @brief Discover processors.
     * @param[in] system_uuid UUID of the parent System
     * @return vector of discovered Processors.
     * */
    virtual std::vector<agent_framework::model::Processor> discover_processors(const Uuid& system_uuid);

    /*!
     * @brief Discover endpoints.
     * @param[in] fabric Reference to parent Fabric component.
     * @return vector of discovered Endpoints.
     * */
    virtual std::vector<agent_framework::model::Endpoint> discover_endpoints(
        const agent_framework::model::Fabric& fabric);


    /*!
     * @brief Discover System.
     * @param[in] manager Reference to parent Manager object.
     * @param chassis Reference to Chassis object.
     * @return Discovered system object.
     * */
    virtual agent_framework::model::System discover_system(
        const agent_framework::model::Manager& manager, const agent_framework::model::Chassis& chassis);


    /*!
     * @brief Discover Zones.
     * @param[in] fabric Reference to parent Fabric object
     * @return Discovered collection of Zones.
     * */
    virtual std::vector<agent_framework::model::Zone> discover_zones(const agent_framework::model::Fabric& fabric);

};

}
}
}

