/*!
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file discovery_entries_provider.cpp
 */

#include "tools/discovery_entries_provider.hpp"
#include "agent-framework/module/common_components.hpp"
#include <algorithm>
#include <tuple>



using namespace ::agent_framework::model;
using namespace ::agent_framework::module;

namespace {

Uuid get_host_zone_uuid(const Uuid& host_system_uuid) {
    // Make uppercase and lowercase version of UUID
    std::string host_system_uuid_lowercase = host_system_uuid;
    for (auto& character: host_system_uuid_lowercase) {
        character = std::tolower(character, std::locale());
    }
    std::string host_system_uuid_uppercase = host_system_uuid;
    for (auto& character: host_system_uuid_uppercase) {
        character = std::toupper(character, std::locale());
    }

    // Get initiator endpoint UUIDs for uppercase and lowercase host system UUID
    auto initiator_endpoint_uuids_uppercase = get_manager<Endpoint>().get_keys(
        [&host_system_uuid_uppercase](const Endpoint& e) {
            return attribute::Identifier::get_uuid(e) == host_system_uuid_uppercase;
        });
    auto initiator_endpoint_uuids_lowercase = get_manager<Endpoint>().get_keys(
        [&host_system_uuid_lowercase](const Endpoint& e) {
            return attribute::Identifier::get_uuid(e) == host_system_uuid_lowercase;
        });

    std::set<Uuid> initiator_endpoint_uuids;
    initiator_endpoint_uuids.insert(initiator_endpoint_uuids_uppercase.begin(),
                                    initiator_endpoint_uuids_uppercase.end());
    initiator_endpoint_uuids.insert(initiator_endpoint_uuids_lowercase.begin(),
                                    initiator_endpoint_uuids_lowercase.end());

    Uuid initiator_zone_uuid{};

    if (initiator_endpoint_uuids.size() == 1) {
        auto initiator_zone_uuids{get_m2m_manager<Zone, Endpoint>().get_parents(*initiator_endpoint_uuids.begin())};
        if (initiator_zone_uuids.size() == 1) {
            initiator_zone_uuid = *initiator_zone_uuids.begin();
        }
        else if (initiator_zone_uuids.size() > 1) {
            log_error("fpga-discovery", "Initiator endpoint with system UUID " << host_system_uuid
                                                                               << " is associated with more than one zone");
        }
    }
    else if (initiator_endpoint_uuids.size() > 1) {
        log_error("fpga-discovery",
                  "Host with system UUID " << host_system_uuid << " is associated with more than one endpoint");
    }
    return initiator_zone_uuid;
}

}

namespace agent {
namespace fpga_discovery {

DiscoveryEntriesProvider::DiscoveryEntries
DiscoveryEntriesProvider::get_discovery_entries(const Uuid& host_system_uuid) {
    DiscoveryEntries entries{};

    log_info("fpga-discovery", "getting entries for host (initiator) with system uuid: " << host_system_uuid);
    if (host_system_uuid.empty()) {
        return entries;
    }

    const auto zone_uuid = get_host_zone_uuid(host_system_uuid);
    log_info("fpga-discovery", "initiator zone uuid: " << zone_uuid);
    if (zone_uuid.empty()) {
        return entries;
    }

    auto initiator_targets_uuids{get_m2m_manager<Zone, Endpoint>().get_children(zone_uuid)};
    // remove initiator uuid from zone children to leave just target endpoints
    // a zone should contain only one initiator endpoint
    initiator_targets_uuids.erase(
        std::remove_if(std::begin(initiator_targets_uuids), std::end(initiator_targets_uuids), [](const Uuid& uuid) {
            return Endpoint::is_initiator(uuid);
        }),
        std::end(initiator_targets_uuids));
    const auto& initiator_targets = get_manager<Endpoint>().get_entries([&initiator_targets_uuids](const Endpoint& e) {
        return std::find(std::begin(initiator_targets_uuids), std::end(initiator_targets_uuids), e.get_uuid())
               != std::end(initiator_targets_uuids);
    });
    for (const auto& target : initiator_targets) {
        log_info("fpga-discovery", "target uuid: " << target.get_uuid());
        for (const auto& transport : target.get_ip_transport_details()) {
            try {
                DiscoveryEntry entry{};
                entry.ip = transport.get_ipv4_address().get_address();
                entry.port = static_cast<uint16_t>(transport.get_port());
                entry.transport = (transport.get_transport_protocol() == enums::TransportProtocol::RoCEv2 ? "RDMA"
                                                                                                          : "TCP");
                entries.insert(std::move(entry));
            }
            catch (const std::exception& e) {
                log_error("fpga-discovery", e.what());
            }
        }
    }

    return entries;
}


bool operator<(const DiscoveryEntriesProvider::DiscoveryEntry& l_entry,
               const DiscoveryEntriesProvider::DiscoveryEntry& r_entry) {
    return std::tie(l_entry.ip, l_entry.port, l_entry.transport) <
           std::tie(r_entry.ip, r_entry.port, r_entry.transport);
}

}
}
