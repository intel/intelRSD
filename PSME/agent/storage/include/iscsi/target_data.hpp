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
 * @file target_data.hpp
 * @brief Target data interface
*/

#ifndef ISCSI_TGT_TARGET_DATA_HPP
#define	ISCSI_TGT_TARGET_DATA_HPP

#include <vector>
#include <memory>
#include <string>

namespace agent {
namespace storage {
namespace iscsi {
namespace tgt {

/*! Class represents tgt lun model data */
class LunData {
public:
    /*!
     * @brief Set lun id
     * @param lun Lun id
     */
    void set_lun(const std::uint64_t lun);

    /*!
     * @brief Get lun id
     * @return Lun id
     */
    std::uint64_t get_lun() const;

    /*!
     * @brief Set lun device path
     * @param device_path Lun device path
     */
    void set_device_path(const std::string& device_path);

    /*!
     * @brief Get lun device path
     * @return Lun device path
     */
    const std::string& get_device_path() const;

private:
    std::uint64_t m_lun{};
    std::string m_device_path{};
};
using LunDataSPtr = std::shared_ptr<LunData>;
using LunDataSVec = std::vector<LunDataSPtr>;

/*! Class represents tgt target model data */
class TargetData {
public:
    /*!
     * @brief Set target id
     * @param target_id Target id
     */
    void set_target_id(const std::int32_t target_id);

    /*!
     * @brief Get target id
     * @return Target id
     */
    std::int32_t get_target_id() const;

    /*!
     * @brief Set target iqn
     * @param target_iqn Target iqn
     */
    void set_target_iqn(const std::string& target_iqn);

    /*!
     * @brief Get target iqn
     * @return Target iqn
     */
    const std::string& get_target_iqn() const;

    /*!
     * @brief Set target initiator
     * @param initiator Target initiator
     */
    void set_target_initiator(const std::string& initiator);

    /*!
     * @brief Get target initiator
     * @return Target initiator
     */
    const std::string& get_target_initiator() const;


    /*!
     * @brief Add lun data object to list
     * @param lun LunData object
     */
    void add_lun_data(const LunDataSPtr& lun);

    /*!
     * @brief Get luns list
     * @return Luns data list
     */
    const LunDataSVec& get_luns() const;
private:
    std::int32_t m_target_id{};
    std::string m_target_iqn{};
    std::string m_target_initiator{};
    LunDataSVec m_luns{};
};
using TargetDataSPtr = std::shared_ptr<TargetData>;
using TargetDataSVec = std::vector<TargetDataSPtr>;

}
}
}
}

#endif	/* ISCSI_TGT_TARGET_DATA_HPP */
