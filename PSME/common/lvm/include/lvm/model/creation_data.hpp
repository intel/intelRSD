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
 * @file creation_data.hpp
 * @brief LVM creation data
 * */

#pragma once



#include <string>



namespace storage {
namespace lvm {
namespace model {

/*! @brief Class needed to copy/create new LVM */
class CreationData {
public:

    /*!
     * @brief Set lvm size
     * @param size Lvm size
     */
    void set_size(const std::uint64_t size) {
        m_size = size;
    }


    /*!
     * @brief Get lvm size
     * @return Lvm size
     */
    std::uint64_t get_size() const {
        return m_size;
    }


    /*!
     * @brief Set volume group name
     * @param group Volume group name
     */
    void set_volume_group(const std::string& group) {
        m_volume_group = group;
    }


    /*!
     * @brief Get volume group name
     * @return Volume group name
     */
    const std::string& get_volume_group() const {
        return m_volume_group;
    }


    /*!
     * @brief Set logical volume name
     * @param logical_volume Logical volume name
     */
    void set_logical_volume(const std::string& logical_volume) {
        m_logical_volume = logical_volume;
    }


    /*!
     * @brief Get logical volume name
     * @return Logical volume name
     */
    const std::string& get_logical_volume() const {
        return m_logical_volume;
    }


    /*!
     * @brief Set create name
     * @param name Create name
     */
    void set_create_name(const std::string& name) {
        m_create_name = name;
    }


    /*!
     * @brief Get create name
     * @return Create name
     */
    const std::string& get_create_name() const {
        return m_create_name;
    }


    /*!
     * @brief Set bootable flag
     * @param bootable Bootable flag
     */
    void set_bootable(bool bootable) {
        m_bootable = bootable;
    }


    /*!
     * @brief Get bootable flag
     * @return Bootable flag
     */
    bool get_bootable() const {
        return m_bootable;
    }


private:
    std::string m_create_name{};
    std::string m_volume_group{};
    std::string m_logical_volume{};
    std::uint64_t m_size{};
    bool m_bootable{};
};

}
}
}
