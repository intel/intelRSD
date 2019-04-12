/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file logical_volume.hpp
 * @brief Logical volume model interface
 * */

#pragma once



#include <string>



namespace storage {
namespace lvm {
namespace model {

/*! @brief Logical volume model */
class LogicalVolume {
public:
    /*!
     * @brief Get logical volume name
     * @return Logical volume name
     * */
    const std::string& get_name() const {
        return m_name;
    }


    /*!
     * @brief Set logical volume name
     * @param[in] name Logical volume name
     * */
    void set_name(const std::string& name) {
        m_name = name;
    }


    /*!
     * @brief Get logical volume master name
     * @return Logical volume master name
     * */
    const std::string& get_master_name() const {
        return m_master_name;
    }


    /*!
     * @brief Set logical volume master name
     * @param[in] master_name Logical volume master name
     * */
    void set_master_name(const std::string& master_name) {
        m_master_name = master_name;
    }


    /*!
     * @brief Get logical volume capacity in bytes
     * @return Logical volume capacity in bytes
     * */
    std::uint64_t get_capacity_b() const {
        return m_capacity_b;
    }


    /*!
     * @brief Set logical volume capacity in bytes
     * @param[in] capacity_b Logical volume capacity in bytes
     * */
    void set_capacity_b(std::uint64_t capacity_b) {
        m_capacity_b = capacity_b;
    }


    /*!
     * @brief Get logical volume's volume_group
     * @return Logical volume's volume_group
     * */
    const std::string& get_volume_group() const {
        return m_volume_group;
    }


    /*!
     * @brief Set logical volume's volume_group
     * @param[in] volume_group Logical volume's volume_group
     * */
    void set_volume_group(const std::string& volume_group) {
        m_volume_group = volume_group;
    }


    /*!
     * @brief Get information about logical volume protection
     * @return True if logical volume is read only
     * */
    bool get_protection_status() const {
        return m_is_protected;
    }


    /*!
     * @brief Set information about logical volume protection
     * @param[in] protection_status should be set to true if logical volume is read only
     * */
    void set_protection_status(bool protection_status) {
        m_is_protected = protection_status;
    }


    /*!
     * @brief Get logical volume health
     * @return Logical volume health
     * */
    const std::string& get_health() const {
        return m_health;
    }


    /*!
     * @brief Set logical volume health
     * @param[in] health Logical volume health
     * */
    void set_health(const std::string& health) {
        m_health = health;
    }


    /*!
     * @brief Get logical volume status
     * @return Logical volume status
     * */
    const std::string& get_status() const {
        return m_status;
    }


    /*!
     * @brief Set logical volume status
     * @param[in] status Logical volume status
     * */
    void set_status(const std::string& status) {
        m_status = status;
    }


    /*!
     * @brief Get logical volume snapshot status
     * @return Logical volume snapshot status
     * */
    bool is_snapshot() const {
        return m_is_snapshot;
    }


    /*!
     * @brief Set logical volume snapshot status
     * @param[in] snapshot_status Logical volume snapshot status
     * */
    void set_snapshot(bool snapshot_status) {
        m_is_snapshot = snapshot_status;
    }


    /*!
     * @brief Get logical volume bootable flag
     * @return Logical volume bootable flag
     * */
    bool is_bootable() const {
        return m_is_bootable;
    }


    /*!
     * @brief Set logical volume bootable Status
     * @param[in] bootable Logical volume bootable status
     * */
    void set_bootable(bool bootable) {
        m_is_bootable = bootable;
    }


    /*!
     * Get string description of the logical volume.
     * @return Logical volume as string.
     */
    std::string to_string() const;


private:
    std::string m_name{};
    std::string m_master_name{};
    std::uint64_t m_capacity_b{};
    std::string m_volume_group{};
    bool m_is_protected{};
    bool m_is_snapshot{};
    bool m_is_bootable{};
    std::string m_status{};
    std::string m_health{};
};

}
}
}
