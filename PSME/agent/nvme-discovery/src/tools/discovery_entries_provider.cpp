/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
#include "tools/tools.hpp"
#include <algorithm>


using namespace ::agent_framework::model;
using namespace ::agent_framework::module;

namespace {

Uuid get_initiator_uuid(const std::string& host_subnqn) {
    Uuid initiator_uuid{};
    const auto nqn = literals::Endpoint::NQN_FORMAT + host_subnqn;
    const auto initiator_endpoints = get_manager<Endpoint>().get_entries([&](const Endpoint & e) {
        return attribute::Identifier::get_nqn(e) == nqn;
    });
    if (initiator_endpoints.empty()) {
        log_debug("nvme-discovery", "Initiator " << host_subnqn << " not found");
    }
    else if (initiator_endpoints.size() > 1) {
        log_warning("nvme-discovery", "More than one initiator with " << host_subnqn << " found");
    }
    else {
        initiator_uuid = initiator_endpoints.front().get_uuid();
    }
    return initiator_uuid;
}

Uuid get_initiator_zone_uuid(const std::string& initiator_uuid) {
    Uuid initiator_zone_uuid{};
    const auto& initiator_zone_uuids{get_m2m_manager<Zone, Endpoint>().get_parents(initiator_uuid)};
    const auto count = initiator_zone_uuids.size();
    if (1 == count) {
        initiator_zone_uuid = initiator_zone_uuids.front();
    }
    else if (count > 1) {
        log_error("nvme-discovery", "Initiator " << initiator_uuid << " is associated with more than one zone");
    }
    return initiator_zone_uuid;
}

}

namespace agent {
namespace nvmf {

nvmf_discovery::DiscoveryEntriesProvider::DiscoveryEntries
DiscoveryEntriesProvider::get_discovery_entries(const std::string& host_subnqn, size_t) {
    nvmf_discovery::DiscoveryEntriesProvider::DiscoveryEntries entries{};

    const auto initiator_uuid = get_initiator_uuid(host_subnqn);
    if (initiator_uuid.empty()) {
        return entries;
    }

    const auto zone_uuid = get_initiator_zone_uuid(initiator_uuid);
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
        for (const auto& transport : target.get_ip_transport_details()) {
            try {
                DiscoveryEntriesProvider::DiscoveryEntry entry{};
                entry.address_family = ::nvme::AddressFamily::Ipv4;
                entry.subsystem_type = ::nvme::SubsystemType::Nvm;
                auto tgt_nqn = attribute::Identifier::get_nqn(target);
                nvme::tools::convert_to_subnqn(tgt_nqn);
                entry.subsystem_nqn = tgt_nqn;
                entry.transport_address = transport.get_ipv4_address().get_address().value();
                entry.transport_service_id = std::to_string(transport.get_port().value());
                entry.transport_type = ::nvme::TransportType::Rdma;
                entries.push_back(std::move(entry));
            }
            catch (const std::exception& e) {
                log_error("nvme-discovery", e.what());
            }
        }
    }

    return entries;
}

uint64_t DiscoveryEntriesProvider::get_discovery_entries_count(const std::string& host_subnqn) {
    uint64_t entries_count{0};
    const auto initiator_uuid = get_initiator_uuid(host_subnqn);
    if (initiator_uuid.empty()) {
        return entries_count;
    }

    const auto zone_uuid = get_initiator_zone_uuid(initiator_uuid);
    if (zone_uuid.empty()) {
        return entries_count;
    }

    auto initiator_targets_uuids_size{get_m2m_manager<Zone, Endpoint>().get_children(zone_uuid).size()};

    if (initiator_targets_uuids_size > 0) {
        entries_count = initiator_targets_uuids_size - 1;
    }

    return entries_count;
}

uint64_t DiscoveryEntriesProvider::get_generation_counter(const std::string&) {
    return 0;
}

}
}
