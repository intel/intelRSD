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


using namespace ::agent_framework::model;
using namespace ::agent_framework::module;

namespace {

Uuid get_initiator_zone_uuid(const Uuid& initiator_uuid) {
    // Make uppercase and lowercase version of UUID
    std::string initiator_uuid_lowercase = initiator_uuid;
    for (auto& character: initiator_uuid_lowercase) {
        character = std::tolower(character, std::locale());
    }

    std::string initiator_uuid_uppercase = initiator_uuid;
    for (auto& character: initiator_uuid_uppercase) {
        character = std::toupper(character, std::locale());
    }

    // Get zone UUIDs for uppercase and lowercase initiator UUID
    const auto& initiator_lowercase_zone_uuids{get_m2m_manager<Zone, Endpoint>().get_parents(initiator_uuid_lowercase)};
    const auto& initiator_uppercase_zone_uuids{get_m2m_manager<Zone, Endpoint>().get_parents(initiator_uuid_uppercase)};

    // Put zone UUIDs from both versions in one set to remove duplicates
    std::set<Uuid> initiator_zone_uuids;
    initiator_zone_uuids.insert(initiator_lowercase_zone_uuids.begin(), initiator_lowercase_zone_uuids.end());
    initiator_zone_uuids.insert(initiator_uppercase_zone_uuids.begin(), initiator_uppercase_zone_uuids.end());

    // Get initiator zone UUID
    Uuid initiator_zone_uuid{};
    const auto count = initiator_zone_uuids.size();
    if (1 == count) {
        initiator_zone_uuid = *initiator_zone_uuids.begin();
    }
    else if (count > 1) {
        log_error("fpga-discovery", "Initiator " << initiator_uuid << " is associated with more than one zone");
    }
    return initiator_zone_uuid;
}

}

namespace agent {
namespace fpga_discovery {



DiscoveryEntriesProvider::DiscoveryEntries
DiscoveryEntriesProvider::get_discovery_entries(const Uuid& initiator_uuid) {
    DiscoveryEntries entries{};

    log_info("fpga-discovery", "getting entries for initiator with uuid: " << initiator_uuid);
    if (initiator_uuid.empty()) {
        return entries;
    }

    const auto zone_uuid = get_initiator_zone_uuid(initiator_uuid);
    log_info("fpga-discovery", "initiator zone uuid: " << zone_uuid);
    if (zone_uuid.empty()) {
        return entries;
    }

    auto initiator_targets_uuids{get_m2m_manager<Zone, Endpoint>().get_children(zone_uuid)};
    // remove initiator uuid from zone children to leave just target endpoints
    // a zone should contain only one initiator endpoint
    initiator_targets_uuids.erase(
            std::remove(std::begin(initiator_targets_uuids), std::end(initiator_targets_uuids), initiator_uuid),
            std::end(initiator_targets_uuids));
    const auto& initiator_targets = get_manager<Endpoint>().get_entries([&initiator_targets_uuids](const Endpoint & e) {
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
                entry.transport = (transport.get_transport_protocol() == enums::TransportProtocol::RoCEv2 ? "RDMA" : "TCP");
                entries.push_back(std::move(entry));
            }
            catch (const std::exception& e) {
                log_error("fpga-discovery", e.what());
            }
        }
    }

    return entries;
}



}
}
