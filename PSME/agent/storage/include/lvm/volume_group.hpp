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
 *
 * @file volume_group.hpp
 * @brief Lvm volume group interface
 * */

#pragma once
#include <string>
#include <vector>

namespace agent {
namespace storage {
namespace lvm {

class PhysicalVolume;
class LogicalVolume;

/*! @brief Group contains logical and physical volumes */
class VolumeGroup {
public:
    using PhysicalVolumeVec = std::vector<PhysicalVolume>;
    using LogicalVolumeVec = std::vector<LogicalVolume>;

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
     * @brief Get volume group capacity in GB
     * @return Volume group capacity in GB
     * */
    double get_capacity_gb() const {
        return m_capacity_gb;
    }

    /*!
     * @brief Set volume group capacity in GB
     * @param[in] capacity_gb Volume group capacity in GB
     * */
    void set_capacity_gb(double capacity_gb) {
        m_capacity_gb = capacity_gb;
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
    const PhysicalVolumeVec& get_physical_volumes() const;

    /*!
     * @brief Add physical volume to this Volume Group
     * @param[in] volume PhysicalVolume to add
     */
    void add_physical_volume(const PhysicalVolume& volume);

    /*!
     * @brief Get logical volumes of this Volume Group
     * @return Collection of logical volumes
     */
    const LogicalVolumeVec& get_logical_volumes() const;

    /*!
     * @brief Add logical volume to this Volume Group
     * @param[in] volume LogicalVolume to add
     */
    void add_logical_volume(const LogicalVolume& volume);

private:
    std::string m_name{};
    double m_capacity_gb{};
    bool m_is_protected{};
    std::string m_status{};
    std::string m_health{};

    /*! Physical Volumes of this Volume Group */
    PhysicalVolumeVec m_physical_volumes{};
    /*! Logical Volumes of this Volume Group */
    LogicalVolumeVec m_logical_volumes{};
};

}
}
}

