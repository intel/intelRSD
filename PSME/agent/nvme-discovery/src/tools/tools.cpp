/*!
 * @brief Implementation of Tools class.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file tools.cpp
 */

#include "tools/tools.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/storage_components.hpp"

#include <stdexcept>
#include <algorithm>



using namespace agent::nvme::tools;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace std;

namespace {

static inline bool is_in_role(const Endpoint& endpoint, const enums::EntityRole& role) {
    for (const auto& ce : endpoint.get_connected_entities()) {
        if (role == ce.get_entity_role()) {
            return true;
        }
    }
    return false;
}

}


bool agent::nvme::tools::is_target(const Endpoint& endpoint) {
    return ::is_in_role(endpoint, enums::EntityRole::Target);
}


bool agent::nvme::tools::is_initiator(const Endpoint& endpoint) {
    return ::is_in_role(endpoint, enums::EntityRole::Initiator);
}


void agent::nvme::tools::convert_to_subnqn(std::string& nqn) {
    // NQN format: nqn.2014-08.org.nvmexpress:uuid:ffffffff-ffff-ffff-ffff-ffffffffffff
    transform(nqn.begin(), nqn.end(), nqn.begin(), ::tolower);
    size_t found = nqn.find("uuid");
    if (std::string::npos != found) {
        // passing "uuid:" in nqn
        static constexpr short UUID_INFIX_LENGTH = 5;
        found += UUID_INFIX_LENGTH;
        nqn = nqn.substr(found);
    }
    else {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent", "Wrong NQN format");
    }
}


void agent::nvme::tools::update_storage_pool_consumed_capacity(const Uuid& storage_pool_uuid) {
    auto volume_uuids = get_m2m_manager<StoragePool, Volume>().get_children(storage_pool_uuid);
    int64_t consumed_capacity_bytes{};
    for (const auto& volume_uuid : volume_uuids) {
        auto volume = get_manager<Volume>().get_entry(volume_uuid);
        if (volume.get_capacity().get_allocated_bytes().has_value()) {
            // check for overlap
            if (consumed_capacity_bytes + volume.get_capacity().get_allocated_bytes() < 0) {
                consumed_capacity_bytes = std::numeric_limits<int64_t>::max();
                log_error("nvme-discovery-agent", "Total capacity consumed by volumes overlaps max value");
                break;
            }
            consumed_capacity_bytes += volume.get_capacity().get_allocated_bytes();
        }
    }
    auto storage_pool_ref = get_manager<StoragePool>().get_entry_reference(storage_pool_uuid);
    auto cap = storage_pool_ref->get_capacity();
    cap.set_consumed_bytes(consumed_capacity_bytes);
    storage_pool_ref->set_capacity(cap);
}