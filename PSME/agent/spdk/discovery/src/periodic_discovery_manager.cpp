/*!
 * @brief Drive discoverer interface.
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
 * @file periodic_discovery_manager.cpp
 */

#include "discovery/periodic_discovery_manager.hpp"

#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/eventing/utils.hpp"



using namespace agent_framework::module;
using namespace agent_framework::model;

namespace {

template <typename M>
M get_entry_or_throw(const Uuid& parent_uuid) {
    if (get_manager<M>().get_keys(parent_uuid).empty()) {
        throw std::runtime_error(std::string{"No "} + M::get_component().to_string() + " entry available!");
    }
    return get_manager<M>().get_entries(parent_uuid).front();
}

template <typename M>
M get_entry_or_throw() {
    if (get_manager<M>().get_keys().empty()) {
        throw std::runtime_error(std::string{"No "} + M::get_component().to_string() + " entry available!");
    }
    return get_manager<M>().get_entries().front();
}

template <typename M>
std::set<Uuid> map_to_uuid_set(const std::vector<M>& collection) {
    std::set<Uuid> uuids{};
    std::transform(collection.begin(), collection.end(), std::inserter(uuids, uuids.begin()), [](const M& model) {
        return model.get_uuid();
    });
    return uuids;
}

template <typename M>
void handle_rediscovery(const std::vector<M>& resources, bool should_remove = false) {
    auto new_resources_uuid_set = ::map_to_uuid_set(resources);
    for (const auto& resource : resources) {
        auto rediscovery_status = get_manager<M>().add_or_update_entry(resource);
        switch (rediscovery_status) {
            case agent_framework::module::TableInterface::UpdateStatus::Added:
                log_info("spdk-discovery", "Resource " << M::get_component().to_string() << ": "
                                                       << resource.get_uuid() << " has been added after rediscovery.");
                agent_framework::eventing::send_event(resource.get_uuid(),
                                                      M::get_component(),
                                                      agent_framework::model::enums::Notification::Add,
                                                      resource.get_parent_uuid());
                break;
            case agent_framework::module::TableInterface::UpdateStatus::Updated:
            case agent_framework::module::TableInterface::UpdateStatus::StatusChanged:
                agent_framework::eventing::send_update(resource);
                log_info("spdk-discovery", "Resource " << M::get_component().to_string() << ": "
                                                       << resource.get_uuid()
                                                       << " has been updated after rediscovery.");
                break;
            case agent_framework::module::TableInterface::UpdateStatus::NoUpdate:
            default:
                log_debug("spdk-discovery", "Resource " << M::get_component().to_string() << ": "
                                                        << resource.get_uuid() << " has not been changed.");
                break;
        }
    }

    // Calculating if something was missing in current model
    auto current_uuids = get_manager<M>().get_keys();
    auto current_uuid_set = std::set<Uuid>(current_uuids.begin(), current_uuids.end());
    std::set<Uuid> removed_resources{};
    std::set_difference(current_uuid_set.begin(), current_uuid_set.end(),
                        new_resources_uuid_set.begin(), new_resources_uuid_set.end(),
                        std::inserter(removed_resources, removed_resources.begin()));

    for (const auto& uuid : removed_resources) {
        auto resource = get_manager<M>().get_entry(uuid);
        if (should_remove) {
            get_manager<M>().remove_entry(uuid);
            agent_framework::eventing::send_event(uuid,
                                                  M::get_component(),
                                                  agent_framework::model::enums::Notification::Remove,
                                                  resource.get_parent_uuid());

            log_info("spdk-discovery", "Resource " << M::get_component().to_string() << ": "
                                                   << uuid << " has been removed after rediscovery.");
        }
        else if (resource.get_status().get_state() != enums::State::Absent) {
            auto resource_ref = get_manager<M>().get_entry_reference(uuid);
            resource_ref->set_status({enums::State::Absent, enums::Health::Critical});
            agent_framework::eventing::send_update(resource_ref.get_raw_ref());
            log_info("spdk-discovery", "Resource " << M::get_component().to_string() << ": "
                                                   << uuid << " has been marked as absent after rediscovery.");
        }
    }
}

}

namespace agent {
namespace spdk {
namespace discovery {

Uuid PeriodicDiscoveryManager::discover() {
    log_debug("spdk-discovery", "Starting rediscovery procedure...");
    auto manager = ::get_entry_or_throw<Manager>();
    auto chassis = ::get_entry_or_throw<Chassis>(manager.get_uuid());
    auto fabric = ::get_entry_or_throw<Fabric>(manager.get_uuid());
    auto system = ::get_entry_or_throw<System>(manager.get_uuid());
    auto storage_service = ::get_entry_or_throw<StorageService>(manager.get_uuid());

    // TODO: check for any issue like missing links
    // TODO: Connect/disconnect related endpoint to changed NIC
    auto interfaces = discover_ethernet_interfaces(system);
    ::handle_rediscovery(interfaces, true);

    {
        // Set up mutex to prevent interruption from GAMI commands
        std::lock_guard<std::mutex> lock{m_context->spdk_mutex};

        // Drive inserted: nothing to do
        // Drive unplugged: nothing to do (marked as absent)
        auto drives = discover_drives(chassis);
        ::handle_rediscovery(drives);

        // Name of store changed: nothing to do as resource will be updated
        // UUID of store changed: the same procedure as add and remove store
        // Store added: nothing to do (volumes will be assigned later)
        // Store removed: nothing to do as volumes will also be marked as absent
        auto pools = discover_storage_pools(storage_service);
        ::handle_rediscovery(pools);

        // Volume created: nothing to do for volume here (storage pool exists here)
        // Volume removed: nothing to do as volume will be marked as absent
        // Size of volume changed: nothing to do as storage pool has been updated already
        // Name of volume changed: nothing to do as one volume will be added and one marked as absent
        auto volumes = discover_volumes(storage_service);
        ::handle_rediscovery(volumes);

        // When drive has gone or namespace has detached from nvmf subsystem: endpoint will change status
        // NVMf created: new endpoint will be added
        // NVMf removed: endpoint will be marked as absent
        auto endpoints = discover_endpoints(fabric);
        ::handle_rediscovery(endpoints);

        // ======
        // TODO: check for any issue like missing links or etc
        // TODO: manipulate hosts and namespaces when zone is active
        // auto zones = discover_zones(fabric);
        // ::handle_rediscovery(zones);
        // ======
    }

    log_debug("spdk-discovery", "Rediscovery procedure finished.");
    return manager.get_uuid();
}

}
}
}