/*!
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
 *
 * @file module_manager.hpp
 * @brief Module Manager
 * */

#ifndef AGENT_FRAMEWORK_MODULE_TARGET_MANAGER_HPP
#define AGENT_FRAMEWORK_MODULE_TARGET_MANAGER_HPP

#include "target.hpp"
#include <vector>
#include <memory>

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace generic {


/*! Target Manager */
class TargetManager {
    using TargetVec = std::vector<Target::TargetSharedPtr>;
public:

    /*!
     * @brief Get ISCSI targets
     * @return Targets list
     */
    const TargetVec& get_targets() const;

    /*!
     * @brief Add new target to list
     * @param target Target object
     */
    void add_target(const Target::TargetSharedPtr& target);

    /*!
     * @brief Remove target from list
     * @param target Target object
     */
    void remove_target(const Target::TargetSharedPtr& target);

    /*!
     * @brief Find target by uuid
     * @param uuid Target uuid
     * @return On success Target object, otherwise nullptr
     */
    Target::TargetSharedPtr find_by_uuid(const std::string& uuid) const;

    /*!
     * @brief Find target by id
     * @param target_id Target id
     * @return On success Target object, otherwise nullptr
     */
    Target::TargetSharedPtr find_by_id(const std::int32_t target_id) const;

    /*!
     * @brief Find target by iqn
     * @param iqn Target iqn
     * @return On success Target object, otherwise nullptr
     */
    Target::TargetSharedPtr find_by_iqn(const std::string& iqn) const;

    /*!
     * @brief Return first free target id
     * @return New target id
     */
    std::int32_t get_new_target_id() const;

    /*!
     * @brief Return first free lun id
     * @param target Target object
     * @return New lun id
     */
    std::uint64_t get_new_lun_id(const Target::TargetSharedPtr& target) const;

private:
    TargetVec m_targets{};
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_TARGET_MANAGER_HPP */

