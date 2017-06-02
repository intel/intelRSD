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
 * @file physical_volume.hpp
 * @brief Physical volume class declaration
 * */

#pragma once
#include <string>

namespace agent {
namespace storage {
namespace lvm {

/*! @brief Physical representation of logical volume */
class PhysicalVolume {
public:
    /*!
     * @brief Get physical volume name
     * @return Physical volume name
     * */
    const std::string& get_name() const {
        return m_name;
    }

    /*!
     * @brief Set physical volume name
     * @param[in] name Physical volume name
     * */
    void set_name(const std::string& name) {
        m_name = name;
    }

    /*!
     * @brief Get physical volume capacity in GB
     * @return Physical volume capacity in GB
     * */
    double get_capacity_gb() const {
        return m_capacity_gb;
    }

    /*!
     * @brief Set physical volume capacity in GB
     * @param[in] capacity_gb Physical volume capacity in GB
     * */
    void set_capacity_gb(double capacity_gb) {
        m_capacity_gb = capacity_gb;
    }

    /*!
     * @brief Get physical volume's volume_group
     * @return Physical volume's volume_group
     * */
    const std::string& get_volume_group() const {
        return m_volume_group;
    }

    /*!
     * @brief Set physical volume's volume_group
     * @param[in] volume_group Physical volume's volume_group
     * */
    void set_volume_group(const std::string& volume_group) {
        m_volume_group = volume_group;
    }

    /*!
     * @brief Get information about physical volume protection
     * @return True if physical volume is read only
     * */
    bool get_protection_status() const {
        return m_is_protected;
    }

    /*!
     * @brief Get physical volume health
     * @return Physical volume health
     * */
    const std::string& get_health() const {
        return m_health;
    }

    /*!
     * @brief Set physical volume health
     * @param[in] health Physical volume health
     * */
    void set_health(const std::string& health) {
        m_health = health;
    }

    /*!
     * @brief Get physical volume status
     * @return Physical volume status
     * */
    const std::string& get_status() const {
        return m_status;
    }

    /*!
     * @brief Set physical volume status
     * @param[in] status Physical volume status
     * */
    void set_status(const std::string& status) {
        m_status = status;
    }

private:
    std::string m_name{};
    double m_capacity_gb{};
    std::string m_volume_group{};
    bool m_is_protected{false};
    std::string m_status{};
    std::string m_health{};
};

}
}
}

