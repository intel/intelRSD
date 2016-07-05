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

#include "agent-framework/module/target_manager.hpp"
#include "agent-framework/module/target.hpp"
#include <algorithm>

using namespace agent_framework::generic;

const TargetManager::TargetVec& TargetManager::get_targets() const {
    return m_targets;
}

void TargetManager::add_target(const Target::TargetSharedPtr& target) {
    m_targets.emplace_back(target);
}

void TargetManager::remove_target(const Target::TargetSharedPtr& target) {
    auto elem = std::find(m_targets.begin(), m_targets.end(), target);
    if (elem != m_targets.end()) {
        m_targets.erase(elem);
    }
}

Target::TargetSharedPtr
TargetManager::find_by_uuid(const std::string& uuid) const {
    for (const auto& target : m_targets) {
        if (target->get_uuid() == uuid) {
            return target;
        }
    }
    return {};
}

Target::TargetSharedPtr
TargetManager::find_by_id(const std::int32_t target_id) const {
    for (const auto& target : m_targets) {
        if (target->get_target_id() == target_id) {
            return target;
        }
    }
    return {};
}
Target::TargetSharedPtr
TargetManager::find_by_iqn(const std::string& iqn) const {
    for (const auto& target : m_targets) {
        if (target->get_target_iqn() == iqn) {
            return target;
        }
    }
    return {};
}

std::int32_t TargetManager::get_new_target_id() const {
    for (std::int32_t i = 0;
            i < std::numeric_limits<std::int32_t>::max(); ++i) {
        if (!find_by_id(i)) {
            return i;
        }
    }
    return 0;
}

std::uint64_t TargetManager::get_new_lun_id(
                            const Target::TargetSharedPtr& target) const {
    const auto& luns = target->get_target_lun();
    for (std::uint64_t i = 0;
            i < std::numeric_limits<std::uint64_t>::max(); ++i) {
        if (luns.cend() == std::find_if(luns.cbegin(),
                                        luns.cend(),
                                        [i](const Target::Lun& lun) {
                                            return lun.get_lun_id() == i;
                                        })) {
            return i;
        }
    }
    return 0;

}
