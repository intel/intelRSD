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
 * @file discovery_entries_provider.hpp
 */

#pragma once



#include <memory>
#include <vector>
#include "agent-framework/module/utils/uuid.hpp"
#include "json-wrapper/json-wrapper.hpp"



namespace agent {
namespace fpga_discovery {

class DiscoveryEntriesProvider {
public:
    using Ptr = std::shared_ptr<DiscoveryEntriesProvider>;

    /*! Discovery Entry */
    struct DiscoveryEntry {
        std::string ip{};
        uint16_t port{0};
        std::string transport{};


        json::Json to_json() const {
            json::Json value{};
            value["ip"] = ip;
            value["port"] = port;
            value["transport"] = transport;
            return value;
        }
    };
    using DiscoveryEntries = std::vector<DiscoveryEntry>;


    DiscoveryEntriesProvider() = default;


    virtual ~DiscoveryEntriesProvider() = default;


    /*!
     * Returns discovery entries for given host
     * @param host_uuid Host identifier
     * @return Discovery entries for given host_uuid
     */
    DiscoveryEntries get_discovery_entries(const Uuid& initiator_uuid);


    /*!
     * Returns number of discovery entries for given host
     * @param host_uuid Host identifier
     * @return Number of discovery entries for given host
     */
    uint64_t get_discovery_entries_count(const std::string& host_uuid);

};
}
}

