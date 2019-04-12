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
 * @file volume_group.hpp
 * @brief Lvm volume group interface
 * */

#pragma once



#include "physical_volume.hpp"
#include "logical_volume.hpp"

#include <string>
#include <vector>



namespace storage {
namespace lvm {
namespace model {

/*! @brief Group contains logical and physical volumes */
class VolumeGroup {
public:
    using PhysicalVolumes = std::vector<PhysicalVolume>;
    using LogicalVolumes = std::vector<LogicalVolume>;


    /*!
     * @brief Get volume group name
     * @return Volume group name
     * */
    const std::string& get_name() const {
        return m_name;
    }


    /*!
     * @brief Set volume group name
     * @param[in] name Volume group name
     * */
    void set_name(const std::string& name) {
        m_name = name;
    }


    /*!
     * @brief Get volume group free capacity in B
     * @return Volume group free capacity in B
     * */
    std::uint64_t get_free_b() const {
        return m_free_b;
    }


    /*!
     * @brief Set volume group free capacity in B
     * @param[in] free_b Volume group free capacity in B
     * */
    void set_free_b(std::uint64_t free_b) {
        m_free_b = free_b;
    }


    /*!
     * @brief Get volume group capacity in bytes
     * @return Volume group capacity in bytes
     * */
    std::uint64_t get_capacity_b() const {
        return m_capacity_b;
    }


    /*!
     * @brief Set volume group capacity in bytes
     * @param[in] capacity_b Volume group capacity in bytes
     * */
    void set_capacity_b(std::uint64_t capacity_b) {
        m_capacity_b = capacity_b;
    }


    /*!
     * @brief Get information about volume group protection
     * @return True if volume group is read only
     * */
    bool get_protection_status() const {
        return m_is_protected;
    }


    /*!
     * @brief Set information about volume group protection
     * @param[in] protection_status should be set to true if volume group is read only
     * */
    void set_protection_status(bool protection_status) {
        m_is_protected = protection_status;
    }


    /*!
     * @brief Get volume group health
     * @return Volume group health
     * */
    const std::string& get_health() const {
        return m_health;
    }


    /*!
     * @brief Set volume group health
     * @param[in] health Volume group health
     * */
    void set_health(const std::string& health) {
        m_health = health;
    }


    /*!
     * @brief Get volume group status
     * @return Volume group status
     * */
    const std::string& get_status() const {
        return m_status;
    }


    /*!
     * @brief Set volume group status
     * @param[in] status Volume group status
     * */
    void set_status(const std::string& status) {
        m_status = status;
    }


    /*!
     * @brief Get physical volumes of this Volume Group
     * @return Collection of physical volumes
     */
    const PhysicalVolumes& get_physical_volumes() const;


    /*!
     * @brief Add physical volume to this Volume Group
     * @param[in] volume PhysicalVolume to add
     */
    void add_physical_volume(const PhysicalVolume& volume);


    /*!
     * @brief Get logical volumes of this Volume Group
     * @return Collection of logical volumes
     */
    const LogicalVolumes& get_logical_volumes() const;


    /*!
     * @brief Add logical volume to this Volume Group
     * @param[in] volume LogicalVolume to add
     */
    void add_logical_volume(const LogicalVolume& volume);


    /*!
     * Get string description of the volume group.
     * @return Volume group as string.
     */
    std::string to_string() const;


private:
    std::string m_name{};
    std::uint64_t m_capacity_b{};
    std::uint64_t m_free_b{};
    std::string m_status{};
    std::string m_health{};
    bool m_is_protected{};

    /*! Physical Volumes of this Volume Group */
    PhysicalVolumes m_physical_volumes{};

    /*! Logical Volumes of this Volume Group */
    LogicalVolumes m_logical_volumes{};
};

}
}
}
