/*!
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
 * @file nvmf-discovery/discovery_entries_getter.hpp
 */

#pragma once

#include "nvme/definitions.hpp"

#include <memory>
#include <string>
#include <vector>

namespace nvmf_discovery {

/*! Discovery entries provider abstract class */
class DiscoveryEntriesProvider {
public:
    using Ptr = std::shared_ptr<DiscoveryEntriesProvider>;

    /*! Discovery Entry */
    struct DiscoveryEntry {
        nvme::TransportType transport_type{nvme::TransportType::Rdma};
        nvme::AddressFamily address_family{nvme::AddressFamily::Ipv4};
        nvme::SubsystemType subsystem_type{nvme::SubsystemType::Nvm};
        nvme::TReqSecureChannel treq_secure_channel{nvme::TReqSecureChannel::NotSpecified};
        uint16_t port_id{0};
        uint16_t controller_id{nvme::CONTROLLER_ID_DYNAMIC};
        uint16_t admin_max_sq_size{nvme::MIN_ADMIN_MAX_SQ_SIZE};
        std::string transport_service_id{};
        std::string subsystem_nqn{};
        std::string transport_address{};
    };

    using DiscoveryEntries = std::vector<DiscoveryEntry>;

    virtual ~DiscoveryEntriesProvider();

    /*!
     * Returns discovery entries for given host
     * @param host_subnqn Host identifier
     * @param offset Starting index of discovery entry to begin
     * @return Discovery entries for given host_nqn
     */
    virtual DiscoveryEntries get_discovery_entries(const std::string& host_subnqn, size_t offset = 0) = 0;

    /*!
     * Returns number of discovery entries for given host
     * @param host_subnqn Host identifier
     * @return Number of discovery entries for given host
     */
    virtual uint64_t get_discovery_entries_count(const std::string& host_subnqn) = 0;

    /*!
     * Returns generation counter indicating the version of the discovery information for given host
     * @param host_subnqn Host identifier
     * @return Generation counter indicating the version of the discovery information for given host
     */
    virtual uint64_t get_generation_counter(const std::string& host_subnqn) = 0;
};

}
