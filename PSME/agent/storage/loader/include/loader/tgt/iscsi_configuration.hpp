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
 * @file iscsi_configuration.hpp
 */

#pragma once



#include "tgt/iscsi_data.hpp"
#include "tgt/target_data.hpp"

#include <string>
#include <map>
#include <mutex>

namespace agent {
namespace storage {
namespace loader {

class IscsiConfiguration {
public:

    using TargetsMap = std::map<tgt::TargetData::Id, tgt::TargetData>;

    /*!
     * @brief Set iSCSI data object
     * @param[in] iscsi_data IscsiData object
     */
    void set_iscsi_data(const tgt::IscsiData& iscsi_data);

    /*!
     * @brief Get iSCSI data object
     * @return IscsiData object
     */
    const tgt::IscsiData& get_iscsi_data() const {
        return m_iscsi_data;
    }

    /*!
     * @brief Store new target data entry into current configuration.
     * @param[in] id Target identifier.
     * @param[in] target Target data.
     */
    void add_iscsi_target(const TargetsMap::key_type& id, const TargetsMap::mapped_type& target);

    /*!
     * @brief Get all stored iSCSI target data.
     * @return All stored targets.
     */
    const TargetsMap& get_iscsi_targets() const {
        return m_targets;
    }

    /*!
     * @brief Retrieve next free target ID.
     * @return Target ID to create in the TGT daemon.
     */
    tgt::TargetData::Id create_target_id() const;

    /*!
     * @brief Remove existing target from configuration.
     * @param target_id Identifier of target to remove.
     */
    void remove_target_id(TargetsMap::key_type target_id);

    /*!
     * Set TGT socket path.
     * @param tgt_socket Path to TGT socket.
     */
    void set_tgt_socket(const std::string& tgt_socket);

    /*!
     * Get TGT socket path.
     * @return TGT socket path.
     */
    const std::string& get_tgt_socket() const {
        return m_tgt_socket;
    }


private:
    mutable std::recursive_mutex m_mutex{};

    std::string m_tgt_socket{};
    tgt::IscsiData m_iscsi_data{};
    TargetsMap m_targets{};
};

}
}
}

