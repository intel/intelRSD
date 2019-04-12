/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 *
 * @brief Initial discovery implementation.
 * */

#pragma once



#include "agent-framework/discovery/discovery_manager.hpp"
#include "agent-framework/module/model/model_storage.hpp"
#include "agent-framework/eventing/utils.hpp"

#include "tree_stability/storage_stabilizer.hpp"
#include "storage_agent_context.hpp"



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
     * @param context Shared pointer to agent storage context.
     * */
    explicit DiscoveryManager(AgentContext::SPtr context) : m_context(context) {}


    /*!
     * @brief Gather information about storage and its submodules.
     * @return UUID of the discovered module manager.
     */
    virtual Uuid discover() override;

    /*!
     * @brief Helper function for rediscovering data from discoverers and automatic update in the Managers.
     * @tparam Discoverer Type of discoverer.
     * @tparam Model Type of model.
     * @param parent_uuid Parent UUID for the rediscovered resources.
     * @param post_update_action Function which is invoked if discovered resource has been updated.
     */
    template <typename Discoverer, typename Model>
    void rediscover(const Uuid& parent_uuid,
                    std::function<void(Uuid, Uuid)> post_update_action = [](const Uuid&, const Uuid&){}) {
        Discoverer discoverer{m_context};
        std::vector<Model> items = discoverer.discover(parent_uuid);
        for (auto& item : items) {
            Uuid old_uuid = item.get_uuid(); // If there is any relation, we can remove it by old UUID of the resource
            m_stabilizer.stabilize(item);
            auto result = agent_framework::module::get_manager<Model>().add_or_update_entry(item);
            if (result == agent_framework::module::TableInterface::UpdateStatus::Updated ||
                result == agent_framework::module::TableInterface::UpdateStatus::StatusChanged) {
                post_update_action(old_uuid, item.get_uuid());
                agent_framework::eventing::send_update(item);
            }
            else if (result == agent_framework::module::TableInterface::UpdateStatus::Added) {
                agent_framework::eventing::send_event(item.get_uuid(),
                                                      Model::get_component(),
                                                      agent_framework::model::enums::Notification::Add,
                                                      item.get_parent_uuid());
            }
        }
    }


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
     * @param[in] manager Reference to parent Manager object.
     * @return Discovered chassis object.
     * */
    virtual agent_framework::model::Chassis discover_chassis(const agent_framework::model::Manager& manager);


    /*!
     * @brief Discover system.
     * @param[in] manager Reference to parent Manager object.
     * @param[in] chassis Reference to chassis object.
     * @return Discovered system object.
     * */
    virtual agent_framework::model::System discover_system(const agent_framework::model::Manager& manager,
                                                           const agent_framework::model::Chassis& chassis);


    /*!
     * @brief Discover system's ethernet interfaces.
     * @param[in] system Reference to parent System object.
     * @return Discovered collection of system's ethernet interface objects.
     * */
    virtual std::vector<agent_framework::model::NetworkInterface> discover_ethernet_interfaces(
        const agent_framework::model::System& system);


    /*!
     * @brief Discover drives.
     * @param[in] chassis Parent Chassis object.
     * @return Discovered drives collection.
     * */
    virtual std::vector<agent_framework::model::Drive> discover_drives(const agent_framework::model::Chassis& chassis);


    /*!
     * @brief Discover fabrics.
     * @param[in] manager Reference to parent Manager object.
     * @return Discovered fabric object.
     * */
    virtual agent_framework::model::Fabric discover_fabric(const agent_framework::model::Manager& manager);


    /*!
     * @brief Discover zones.
     * @param[in] fabric Parent component.
     * @return Discovered zones collection.
     * */
    virtual std::vector<agent_framework::model::Zone> discover_zones(const agent_framework::model::Fabric& fabric);


    /*!
     * @brief Discover endpoints.
     * @param[in] fabric Parent component.
     * @return Discovered endpoints collection.
     * */
    virtual std::vector<agent_framework::model::Endpoint> discover_endpoints(const agent_framework::model::Fabric& fabric);


    /*!
     * @brief Discover Storage Service.
     * @param[in] manager Reference to parent Manager object.
     * @return Discovered storage service object.
     * */
    virtual agent_framework::model::StorageService discover_storage_service(
        const agent_framework::model::Manager& manager);


    /*!
     * @brief Discover storage pools.
     * @param[in] storage_service Reference to parent Storage Service component.
     * @return Discovered storage pools.
     * */
    virtual std::vector<agent_framework::model::StoragePool> discover_storage_pools(
        const agent_framework::model::StorageService& storage_service);


    /*!
     * @brief Discover volumes.
     * @param[in] storage_service Reference to parent Storage Service component.
     * @return Discovered volumes.
     * */
    virtual std::vector<agent_framework::model::Volume> discover_volumes(
        const agent_framework::model::StorageService& storage_service);

    /*!
     * @brief Restores Endpoints and Zones from the database.
     * @param[in] parent_uuid UUID of the parent component.
     * */
    virtual void restore(const Uuid& parent_uuid);


private:
    AgentContext::SPtr m_context{};
    StorageStabilizer m_stabilizer{};

    void update_volume_capacity_sources();

};

}
}
}

