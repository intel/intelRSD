/*!
 * @brief Initial discovery implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file discovery_manager.hpp
 * */

#pragma once



#include "agent-framework/discovery/discovery_manager.hpp"
#include "agent-framework/module/model/model_storage.hpp"

#include "spdk_agent_context.hpp"
#include "spdk_stabilizer.hpp"



namespace agent {
namespace spdk {
namespace discovery {

/*!
 * @brief Implementation of initial discovery
 */
class DiscoveryManager : public agent_framework::discovery::DiscoveryManager {
public:

    /*!
     * @brief Constructor for discovery manager.
     * @param context Shared pointer to agent storage context.
     * */
    explicit DiscoveryManager(AgentContext::SPtr context) : m_context(context) {}


    /*!
     * @brief Gather information about storage and its submodules.
     * @return UUID of the discovered module manager.
     */
    virtual Uuid discover() override;


    /*! @brief Default destructor. */
    virtual ~DiscoveryManager();

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
     * @brief Discover Storage Service.
     * @param[in] manager Reference to parent Manager object.
     * @return Discovered storage service object.
     * */
    virtual agent_framework::model::StorageService discover_storage_service(
        const agent_framework::model::Manager& manager);


    /*!
     * @brief Discover Volumes
     * @param[in] storage_service Reference to parent Storage Service object.
     * @return vector of discovered Volume objects.
     * */
    virtual std::vector<agent_framework::model::Volume> discover_volumes(
        const agent_framework::model::StorageService& storage_service);


    /*!
     * @brief Discover system's ethernet interfaces.
     * @param[in] system Reference to parent System object.
     * @return Discovered collection of system's ethernet interface objects.
     * */
    virtual std::vector<agent_framework::model::NetworkInterface> discover_ethernet_interfaces(
        const agent_framework::model::System& system);


    /*!
     * @brief Discover Drives.
     * @param chassis Reference to parent Chassis object.
     * @return vector of discovered drives.
     * */
    virtual std::vector<agent_framework::model::Drive> discover_drives(const agent_framework::model::Chassis& chassis);


    /*!
     * @brief Discover storage pools.
     * @param[in] storage_service Reference to parent Storage Service component.
     * @return Discovered storage pools.
     * */
    virtual std::vector<agent_framework::model::StoragePool> discover_storage_pools(
        const agent_framework::model::StorageService& storage_service);


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

protected:

    AgentContext::SPtr m_context{};
    SpdkStabilizer m_stabilizer{};

};

}
}
}

