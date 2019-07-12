/*!
 * @brief Discovery manager for compute agent class interface.
 *
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 */

#pragma once



#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/discovery/discovery_manager.hpp"

#include "discoverers/generic_discoverer.hpp"
#include "ipmi/utils/sdv/mdr_region_accessor.hpp"
#include "tree_stability/compute_stabilizer.hpp"
#include "status/bmc.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace agent {
namespace compute {
namespace discovery {

class DiscoveryManager : public agent_framework::discovery::DiscoveryManager {
public:
    DiscoveryManager(agent::compute::Bmc& bmc,
                     ipmi::sdv::MdrRegionAccessorFactory::Ptr mdr_accessor_factory,
                     DiscovererFactory::Ptr discoverer_factory = std::make_shared<DiscovererFactory>());


    DiscoveryManager(const DiscoveryManager&) = default;


    DiscoveryManager(DiscoveryManager&&) = default;


    DiscoveryManager& operator=(const DiscoveryManager&) = default;


    DiscoveryManager& operator=(DiscoveryManager&&) = default;


    virtual ~DiscoveryManager();


    /*!
     * @brief Perform full discovery of all resources managed by a particular management controller.
     * @return Stabilized UUID of discovered manager
     */
    virtual Uuid discover() override;


protected:

    /*!
     * @brief Discover sled manager.
     * @return Discovered manager object.
     */
    agent_framework::model::Manager discover_manager();


    /*!
     * @brief Discover sled chassis.
     * @param parent_uuid Parent UUID.
     * @return Discovered chassis object.
     */
    agent_framework::model::Chassis discover_chassis(const Uuid& parent_uuid);


    /*!
     * @brief Discover sled's thermal zone.
     * @param parent_uuid Parent UUID.
     * @return Discovered thermal zone object.
     */
    agent_framework::model::ThermalZone discover_thermal_zone(const Uuid& parent_uuid);


    /*!
     * @brief Discover sled's power zone.
     * @param parent_uuid Parent UUID.
     * @return Discovered power zone object.
     */
    agent_framework::model::PowerZone discover_power_zone(const Uuid& parent_uuid);


    /*!
     * @brief Discover system.
     * @param parent_uuid Parent UUID.
     * @param chassis_uuid System chassis UUID.
     * @return Discovered system object.
     */
    agent_framework::model::System discover_system(const Uuid& parent_uuid, const Uuid& chassis_uuid);


    /*!
     * @brief Discover all system PCIe devices.
     * @param parent_uuid Parent UUID.
     * @param chassis_uuid sled chassis UUID.
     * @return Vector of discovered PcieDevice objects.
     */
    std::vector<agent_framework::model::PcieDevice>
    discover_pcie_devices(const Uuid& parent_uuid, const Uuid& chassis_uuid);


    /*!
     * @brief Discover all system PCIe functions
     * @param devices vector of PcieDevice objects, parents of the Pcie functions
     * @return Vector of discovered PcieFunction objects.
     */
    std::vector<agent_framework::model::PcieFunction>
    discover_pcie_functions(const std::vector<agent_framework::model::PcieDevice>& devices);


    /*!
     * @brief Discover all system processors.
     * @param parent_uuid Parent UUID.
     * @return Vector of discovered processor objects.
     */
    std::vector<agent_framework::model::Processor> discover_processors(const Uuid& parent_uuid);


    /*!
     * @brief Discover all system memory.
     * @param parent_uuid Parent UUID.
     * @return Vector of discovered memory objects.
     */
    std::vector<agent_framework::model::Memory> discover_memory(const Uuid& parent_uuid);


    /*!
     * @brief Discover all system memory domains.
     * @param parent_uuid Parent UUID.
     * @return Vector of discovered memory domain objects.
     */
    std::vector<agent_framework::model::MemoryDomain> discover_memory_domains(const Uuid& parent_uuid);


    /*!
     * @brief Discover all system memory chunks.
     * @param parent_uuid Parent UUID.
     * @return Vector of discovered memory chunks objects.
     */
    std::vector<agent_framework::model::MemoryChunks> discover_memory_chunks(const Uuid& parent_uuid);


    /*!
     * @brief Discover storage subsystem.
     * @param parent_uuid Parent UUID.
     * @return Discovered storage subsystem object.
     */
    agent_framework::model::StorageSubsystem discover_storage_subsystem(const Uuid& parent_uuid);


    /*!
     * @brief Discover all system drives.
     * @param parent_uuid Parent UUID.
     * @return Vector of discovered system drives.
     */
    std::vector<agent_framework::model::Drive> discover_drives(const Uuid& parent_uuid);


    /*!
     * @brief Discover all sled network interfaces.
     * @param parent_uuid Parent UUID.
     * @return Vector of discovered system network interfaces.
     */
    std::vector<agent_framework::model::NetworkInterface> discover_network_interfaces(const Uuid& parent_uuid);


    /*!
     * @brief Discover network device.
     * @param parent_uuid Parent UUID.
     * @return Discovered network device object.
     */
    agent_framework::model::NetworkDevice discover_network_device(const Uuid& parent_uuid);


    /*!
     * @brief Discover network device function.
     * @param parent_uuid Parent UUID.
     * @return Discovered network device function object.
     */
    agent_framework::model::NetworkDeviceFunction discover_network_device_function(const Uuid& parent_uuid);


    /*!
     * @brief Discover all system trusted modules.
     * @param parent_uuid Parent UUID.
     * @return Vector of system trusted modules.
     */
    std::vector<agent_framework::model::TrustedModule> discover_trusted_modules(const Uuid& parent_uuid);


    /*!
     * @brief Discover system SEL service.
     * @param parent_uuid Parent UUID.
     * @return Discovered SEL log service object.
     */
    agent_framework::model::LogService discover_sel_service(const Uuid& parent_uuid);


    std::string connection_data_to_string() {
        return m_bmc.get_id();
    }


    template<typename T, typename... Args>
    T update_collection(agent_framework::model::attribute::EventData::Vector& events,
                           T&& collection, const Args&... args) {
        using Model = typename T::value_type;
        auto& table = agent_framework::module::get_manager<Model>();
        const auto before_change_epoch = table.get_current_epoch();
        for (auto& elem : collection) {
            m_stabilizer.stabilize(elem, args...);
            add_or_update(events, table, elem);
        }
        remove_untouched(before_change_epoch, events, table);
        return collection;
    }

    template<typename T>
    void add_or_update_with_removal(agent_framework::model::attribute::EventData::Vector& events, T& resource) {
        auto& table = agent_framework::module::get_manager<T>();
        const auto before_change_epoch = table.get_current_epoch();
        add_or_update(events, table, resource);
        remove_untouched(before_change_epoch, events, table);
    }

private:

    template<typename U>
    void remove_untouched(const std::uint64_t before_change_epoch,
            agent_framework::model::attribute::EventData::Vector& events, U& table) {
        using Model = typename U::value_type;
        const auto& bmc_id = m_bmc.get_id();
        auto not_touched = table.get_keys(
            [&before_change_epoch, &bmc_id](const Model& entry) {
                return (entry.get_agent_id() == bmc_id
                        && !entry.was_touched_after(before_change_epoch));
            }
        );
        for (const auto& to_remove_uuid : not_touched) {
            agent_framework::model::attribute::EventData event{};
            table.remove_entry(to_remove_uuid,
                    [&event](const Model& elem) {
                        event.set_parent(elem.get_parent_uuid());
                        event.set_component(elem.get_uuid());
                        event.set_type(elem.get_component());
                        event.set_notification(agent_framework::model::enums::Notification::Remove);
                    });
            if (!event.get_component().empty()) {
                events.push_back(std::move(event));
            }
        }
    }

    template<typename U, typename T = typename U::value_type>
    void add_or_update(agent_framework::model::attribute::EventData::Vector& events,
                       U& table, T& resource) {
        resource.set_agent_id(m_bmc.get_id());
        const auto status = table.add_or_update_entry(resource);

        /* add all metrics and definitions for the resource */
        if (auto telemetry_service = m_bmc.telemetry()) {
            telemetry_service->build_metrics_for_resource(m_stabilizer, resource);
        }

        auto add_event = [&events](const T& res, agent_framework::model::enums::Notification notification_type) {
            agent_framework::model::attribute::EventData event{};
            event.set_parent(res.get_parent_uuid());
            event.set_component(res.get_uuid());
            event.set_type(res.get_component());
            event.set_notification(notification_type);
            events.push_back(std::move(event));
        };

        if ((resource.get_component() == agent_framework::model::enums::Component::Manager ||
            resource.get_component() == agent_framework::model::enums::Component::System) &&
            (agent_framework::module::TableInterface::UpdateStatus::Added == status)) {
            add_event(resource, agent_framework::model::enums::Notification::Add);
            log_info("discovery-manager", "Added " << T::get_component().to_string());
            log_debug("discovery-manager", "Added " << T::get_component().to_string() << ", UUID " << resource.get_uuid());
        }
        else if (agent_framework::module::TableInterface::UpdateStatus::StatusChanged == status ||
                 agent_framework::module::TableInterface::UpdateStatus::Updated == status) {
            add_event(resource, agent_framework::model::enums::Notification::Update);
            log_info("discovery-manager", "Updated " << T::get_component().to_string());
            log_debug("discovery-manager", "Updated " << T::get_component().to_string() << ", UUID " << resource.get_uuid());
        }
    }

    agent::compute::Bmc& m_bmc;
    GenericDiscoverer::Ptr m_discoverer;
    agent::compute::ComputeStabilizer m_stabilizer{};
};

}
}
}
