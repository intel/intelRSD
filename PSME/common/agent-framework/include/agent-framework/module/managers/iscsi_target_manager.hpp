/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file iscsi_target_manager.hpp
 * @brief Iscsi Target manager
 * */

#pragma once
#include "agent-framework/module/managers/generic_manager.hpp"
#include "agent-framework/module/model/iscsi_target.hpp"
#include "agent-framework/module/model/attributes/iscsi_data.hpp"

/*! Psme namespace */
namespace agent_framework {
namespace module {
namespace managers {

/*! Extended ISCSI target manager */
class IscsiTargetManager final :
                public GenericManager<agent_framework::model::IscsiTarget> {
public:
    using GenericManager<agent_framework::model::IscsiTarget>::GenericManager;
    using IscsiData = agent_framework::model::attribute::IscsiData;

    /*!
     * @brief Set iscsi data object
     * @param[in] iscsi_data IscsiData object
     */
    void set_iscsi_data(const IscsiData& iscsi_data) {
        m_iscsi_data = iscsi_data;
    }

    /*!
     * @brief Get iscsi data object
     * @return IscsiData object
     */
    const IscsiData& get_iscsi_data() const {
        return m_iscsi_data;
    }

    /*!
     * @brief Create new target id
     * @return New target id
     */
    uint32_t create_target_id() const;

    /*!
     * @brief Add target id
     * @param[in] target_id Target ID
     * @param[in] uuid ISCSI target UUID
     */
    void add_target_id(uint32_t target_id, const std::string& uuid);

    /*!
     * @brief Get target id using ISCSI target UUID
     * @param[in] uuid ISCSI target UUID
     * @return Target id or exception if not found
     */
    uint32_t get_target_id(const std::string& uuid) const;

    /*!
     * @brief Remove target id
     * @param[in] target_id Target ID
     */
    void remove_target_id(uint32_t target_id);

    /*! Default destructor */
    ~IscsiTargetManager();

private:
    IscsiData m_iscsi_data{};
    std::map<uint32_t, std::string> m_target_id_map{};
};

}
}
}


