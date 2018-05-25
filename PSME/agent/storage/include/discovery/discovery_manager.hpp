/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * @file discovery_manager.hpp
 *
 * @brief Initial discovery implementation.
 * */

#pragma once



#include "agent-framework/discovery/discovery_manager.hpp"
#include "agent-framework/module/model/model_storage.hpp"

#include "tree_stability/storage_stabilizer.hpp"
#include "discovery/storage_discoverer.hpp"



namespace agent {
namespace storage {
namespace discovery {

/*!
 * @brief Implementation of initial discovery
 */
class DiscoveryManager : public agent_framework::discovery::DiscoveryManager {
public:

    /*!
     * @brief Constructor for discovery manager.
     * @param discoverer Shared pointer to platform specific discoverer for Storage Service.
     * */
    DiscoveryManager(StorageDiscoverer::SPtr discoverer) : m_discoverer(discoverer) {}


    /*!
     * @brief Gather information about storage and its submodules.
     * @return UUID of the discovered module manager.
     */
    virtual Uuid discover() override;


    /*! @brief Default destructor. */
    virtual ~DiscoveryManager();


protected:

    /*!
     * @brief Discover Storage Service Manager.
     * @return Discovered manager object.
     * */
    virtual agent_framework::model::Manager discover_manager();


    /*!
     * @brief Discover chassis.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Discovered chassis object.
     * */
    virtual agent_framework::model::Chassis discover_chassis(const Uuid& parent_uuid);


    /*!
     * @brief Discover system.
     * @param[in] parent_uuid UUID of the parent component.
     * @param[in] chassis_uuid UUID of the chassis component.
     * @return Discovered system object.
     * */
    virtual agent_framework::model::System discover_system(const Uuid& parent_uuid, const Uuid& chassis_uuid);


    /*!
     * @brief Discover system's ethernet interfaces.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Discovered collection of system's ethernet interface objects.
     * */
    virtual std::vector<agent_framework::model::NetworkInterface> discover_ethernet_interfaces(const Uuid& parent_uuid);


    /*!
     * @brief Discover drives.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Discovered drives collection.
     * */
    virtual std::vector<agent_framework::model::Drive> discover_drives(const Uuid& parent_uuid);


    /*!
     * @brief Discover fabrics.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Discovered fabric object.
     * */
    virtual agent_framework::model::Fabric discover_fabric(const Uuid& parent_uuid);


    /*!
     * @brief Discover zones.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Discovered zones collection.
     * */
    virtual std::vector<agent_framework::model::Zone> discover_zones(const Uuid& parent_uuid);


    /*!
     * @brief Discover endpoints.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Discovered endpoints collection.
     * */
    virtual std::vector<agent_framework::model::Endpoint> discover_endpoints(const Uuid& parent_uuid);


    /*!
     * @brief Discover Storage Service.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Discovered storage service object.
     * */
    virtual agent_framework::model::StorageService discover_storage_service(const Uuid& parent_uuid);


    /*!
     * @brief Discover storage pools.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Discovered storage pools.
     * */
    virtual std::vector<agent_framework::model::StoragePool> discover_storage_pools(const Uuid& parent_uuid);


    /*!
     * @brief Discover volumes.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Discovered volumes.
     * */
    virtual std::vector<agent_framework::model::Volume> discover_volumes(const Uuid& parent_uuid);


private:
    StorageDiscoverer::SPtr m_discoverer{};
    StorageStabilizer m_stabilizer{};

    void update_volume_relations(std::vector<agent_framework::model::Volume>& volumes_to_update,
                                 agent_framework::model::Volume& volume_to_update,
                                 const Uuid& old_volume_uuid);
    void restore_iscsi_targets(const Uuid& uuid) const;
    void discovery_iscsi_targets(const Uuid& uuid) const; // @TODO: move to discoverer
    void update_drive_relations(agent_framework::model::Drive& drive_to_update);
    void add_weak_pool_volume_collection();
    void update_volume_capacity_sources();

};

}
}
}

