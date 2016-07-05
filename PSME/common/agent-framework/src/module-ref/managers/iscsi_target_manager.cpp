/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 *
 * @section DESCRIPTION
*/

#include "agent-framework/module-ref/managers/iscsi_target_manager.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include <algorithm>

using std::lock_guard;
using std::recursive_mutex;
using std::numeric_limits;
using namespace agent_framework::module::managers;

uint32_t IscsiTargetManager::create_target_id() const {
    lock_guard<recursive_mutex> lock{m_mutex};
    for (uint32_t tid = 0;
        tid < numeric_limits<uint32_t>::max(); ++tid) {
        /* find the target id in the map */
        auto iter = m_target_id_map.find(tid);
        if (m_target_id_map.end() == iter) {
            /* return new target id */
            return tid;
        }
    }

    THROW(agent_framework::exceptions::IscsiError,
        "storage-agent", "No free target ID available.");
}

uint32_t IscsiTargetManager::get_target_id(const string& uuid) const {
    lock_guard<recursive_mutex> lock{m_mutex};
    auto iter = std::find_if(m_target_id_map.begin(), m_target_id_map.end(),
        [&](const std::pair<uint32_t, std::string>& elem) {
            return elem.second == uuid;
        });
    if (m_target_id_map.end() == iter) {
        THROW(agent_framework::exceptions::InvalidUuid,
            "storage-agent", "Target ID not found for UUID = '" + uuid + "'.");
    }
    return iter->first;
}

void IscsiTargetManager::remove_target_id(uint32_t target_id) {
    lock_guard<recursive_mutex> lock{m_mutex};
    m_target_id_map.erase(target_id);
}

void IscsiTargetManager::add_target_id(uint32_t target_id, const string& uuid) {
    lock_guard<recursive_mutex> lock{m_mutex};
    m_target_id_map[target_id] = uuid;
}

IscsiTargetManager::~IscsiTargetManager() { }
