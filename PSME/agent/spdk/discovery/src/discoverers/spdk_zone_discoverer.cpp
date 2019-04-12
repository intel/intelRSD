/*!
 * @brief Zone discoverer implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file spdk_zone_discoverer.cpp
 */

#include "agent-framework/module/model/endpoint.hpp"
#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/discovery/builders/zone_builder.hpp"
#include "agent-framework/database/database_entities.hpp"
#include "agent-framework/database/database_keys.hpp"

#include "discovery/discoverers/spdk_zone_discoverer.hpp"
#include "spdk/spdk_api.hpp"



using namespace agent::spdk::discovery;
using namespace agent_framework::model;
using namespace agent_framework::module;

namespace {

std::vector<Endpoint> find_endpoints_by_nqn(const std::string& nqn) {
    return get_manager<Endpoint>().get_entries([nqn](const Endpoint& endpoint) -> bool {
        return attribute::Identifier::get_nqn(endpoint) == nqn;
    });
}


OptionalField<Endpoint> find_initiator(const std::vector<Uuid> endpoints) {
    for (const auto& endpoint_uuid : endpoints) {
        try {
            auto endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
            if (Endpoint::is_initiator(endpoint)) {
                return endpoint;
            }
        }
        catch (const agent_framework::exceptions::InvalidUuid&) {
            log_warning("spdk-discovery", "Could not find endpoint '" << endpoint_uuid << "' from DB!");
        }
    }
    return {};
}

std::set<Uuid> convert_hw_zone_to_set(const std::vector<std::string>& nqns, const Uuid& initiator) {
    std::set<Uuid> hw_zone{};
    hw_zone.insert(initiator);

    for (const auto& target_nqn : nqns) {
        auto targets = find_endpoints_by_nqn(target_nqn);
        if (!targets.empty()) {
            hw_zone.insert(targets.front().get_uuid());
        }
        else {
            log_critical("spdk-discovery", "Could not find Target Endpoint with NQN("
                << target_nqn << ") during reading zone from HW.");
        }
    }
    return hw_zone;
}

}


std::vector<Zone> SpdkZoneDiscoverer::discover(const Uuid& parent_uuid) {
    std::vector<Zone> zones{};
    std::vector<::spdk::model::NvmfSubsystem> nvmf_subsystems{};

    try {
        nvmf_subsystems = m_context->spdk_api->get_nvmf_subsystems();
    }
    catch (const std::exception& exception) {
        log_error("spdk-discovery", "Failed to call get_nvmf_subsystem method on SPDK: " << exception.what());
    }

    // Map of Host's NQN to all targets' NQNs
    std::unordered_map<std::string, std::vector<std::string>> hw_zones{};

    // Zone on HW exists if there is NVMf subsystem and related host's NQN for it.
    for (const auto& nvmf_subsystem : nvmf_subsystems) {
        log_debug("spdk-discovery", "\n" << nvmf_subsystem.to_string());
        if (nvmf_subsystem.get_subtype() == ::spdk::model::NvmfSubsystem::DISCOVERY) {
            continue;
        }

        if (nvmf_subsystem.get_hosts().empty()) {
            // Just target endpoint without initiator. Take next...
            log_debug("spdk-discovery", "NVMf subsystem (" << nvmf_subsystem.get_nqn()
                << ") has no hosts defined (target endpoint)");
            continue;
        }

        // We assume that there is only one initiator
        if (nvmf_subsystem.get_hosts().size() > 1) {
            std::stringstream ss{};
            ss << "Zone for target(" << nvmf_subsystem.get_nqn();
            ss << ") contains more that 1 host:" << std::endl;
            for (const auto& host : nvmf_subsystem.get_hosts()) {
                ss << "\t" << host.to_string();
            }
            log_warning("spdk-discovery", ss.str());
        }

        for (const auto& host : nvmf_subsystem.get_hosts()) {
            const auto& host_nqn = host.get_nqn();
            if (hw_zones.count(host_nqn)) {
                // Append next target endpoint
                hw_zones[host_nqn].push_back(nvmf_subsystem.get_nqn());
            }
            else {
                hw_zones.insert({host_nqn, {nvmf_subsystem.get_nqn()}});
            }
        }
    }

    for (const auto& entity : agent_framework::database::ZoneEntity::get_aggregates()) {
        auto zone = agent_framework::discovery::ZoneBuilder::build_default(parent_uuid);
        zone.set_uuid(entity.get_uuid());

        auto db_endpoint_uuids = entity.get_multiple_values(agent_framework::database::ZONE_ENDPOINTS_PROPERTY);

        // We assume that there is only one initiator in DB
        auto initiator = ::find_initiator(db_endpoint_uuids);
        if (initiator.has_value()) {
            auto initiator_nqn = attribute::Identifier::get_nqn(initiator.value());
            if (hw_zones.count(initiator_nqn)) {
                auto hw_endpoints_set = ::convert_hw_zone_to_set(hw_zones[initiator_nqn], initiator->get_uuid());
                auto db_endpoints_set = std::set<Uuid>(db_endpoint_uuids.begin(), db_endpoint_uuids.end());

                if (hw_endpoints_set == db_endpoints_set) {
                    log_debug("spdk-discovery",
                        "Database endpoints in zone(" << zone.get_uuid() << ") matches hardware endpoints.");

                    for (const auto& endpoint_uuid : hw_endpoints_set) {
                        get_m2m_manager<Zone, Endpoint>().add_entry(zone.get_uuid(), endpoint_uuid);
                        log_debug("spdk-discovery", "Assigned endpoint(" << endpoint_uuid << ") to zone(" << zone.get_uuid() << ")");
                    }
                }
                else {
                    log_warning("spdk-discovery", "Inconsistent state in DB (" << db_endpoints_set.size()
                        << " endpoints) and HW (" << hw_endpoints_set.size() << " endpoints)! Reading zone from HW...");

                    for (const auto& endpoint_uuid : hw_endpoints_set) {
                        get_m2m_manager<Zone, Endpoint>().add_entry(zone.get_uuid(), endpoint_uuid);
                        log_debug("spdk-discovery", "Assigned endpoint(" << endpoint_uuid << ") to zone(" << zone.get_uuid() << ")");
                    }

                    // TODO: Save new HW zone in DB

                    // TODO: handle multiple hosts:
                    // TODO: - remove entry in hw_zones after processing, if something is still in collection do something with it
                }
            }
            else {
                if (db_endpoint_uuids.size() == 1) {
                    // Only initiator
                    get_m2m_manager<Zone, Endpoint>().add_entry(zone.get_uuid(), initiator->get_uuid());
                    log_debug("spdk-discovery", "Assigned initiator endpoint(" << initiator->get_uuid() << ") to zone(" << zone.get_uuid() << ")");
                }
                else {
                    // Incorrect zone with initiators/targets endpoints which are not in HW
                    zone.set_status({enums::State::Disabled, enums::Health::Warning});
                    std::stringstream ss{};
                    ss << "Zone " << zone.get_uuid() << " contains incorrect endpoints:" << std::endl;
                    for (const auto& uuid : db_endpoint_uuids) {
                        ss << "  " << uuid << std::endl;

                        // TODO: what if endpoints not exists? Try to create dummy endpoints (?)
                        if (get_manager<Endpoint>().entry_exists(uuid)) {
                            get_m2m_manager<Zone, Endpoint>().add_entry(zone.get_uuid(), uuid);
                        }
                    }
                    log_error("spdk-discovery", ss.str());
                }
            }
        }
        else if (!db_endpoint_uuids.empty()) {
            for (const auto& target : db_endpoint_uuids) {
                if (!get_manager<Endpoint>().entry_exists(target)) {
                    log_warning("spdk-discovery",
                        "Cannot assign target '" << target << "' to zone. Entry not exists in model.");
                    zone.set_status({enums::State::Disabled, enums::Health::Warning});
                }
                else {
                    get_m2m_manager<Zone, Endpoint>().add_entry(zone.get_uuid(), target);
                }
                log_debug("spdk-discovery", "Assigned target endpoint(" << target << ") to zone(" << zone.get_uuid() << ")");
            }
        }

        zones.push_back(zone);
        if (Zone::is_healthy(zone)) {
            log_debug("spdk-discovery", "Fully discovered zone: " << zone.get_uuid());
        }
    }

    return zones;
}
