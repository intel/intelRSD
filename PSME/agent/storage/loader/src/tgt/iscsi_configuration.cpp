/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * */

#include "agent-framework/exceptions/exception.hpp"

#include "loader/tgt/iscsi_configuration.hpp"

using namespace agent::storage::loader;

tgt::TargetData::Id IscsiConfiguration::create_target_id() const {
    std::lock_guard<std::recursive_mutex> lock{m_mutex};

    /* TID should start from 1 */
    for (tgt::TargetData::Id tid = 1; tid < std::numeric_limits<tgt::TargetData::Id>::max(); ++tid) {
        /* Find the target ID in the map */
        auto it = m_targets.find(tid);
        if (m_targets.end() == it) {
            /* Return new target id */
            return tid;
        }
    }

    THROW(agent_framework::exceptions::IscsiError, "storage-agent", "No free target ID available.");
}

void IscsiConfiguration::remove_target_id(TargetsMap::key_type target_id) {
    std::lock_guard<std::recursive_mutex> lock{m_mutex};
    m_targets.erase(target_id);
}

void IscsiConfiguration::set_iscsi_data(const tgt::IscsiData& iscsi_data) {
    std::lock_guard<std::recursive_mutex> lock{m_mutex};
    m_iscsi_data = std::move(iscsi_data);
}

void IscsiConfiguration::add_iscsi_target(const TargetsMap::key_type& id, const TargetsMap::mapped_type& target) {
    std::lock_guard<std::recursive_mutex> lock{m_mutex};
    m_targets[id] = std::move(target);
}

void IscsiConfiguration::set_tgt_socket(const std::string& tgt_socket) {
    std::lock_guard<std::recursive_mutex> lock{m_mutex};
    m_tgt_socket = std::move(tgt_socket);
}
