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
 * @file lvm_create_data.hpp
 * @brief Lvm create data information
 * */

#ifndef PSME_AGENT_STORAGE_LVM_LVM_CREATE_DATA_HPP
#define PSME_AGENT_STORAGE_LVM_LVM_CREATE_DATA_HPP

#include <string>

namespace agent {
namespace storage {
namespace lvm {

/*! @brief Class needed to copy/create new lvm */
class LvmCreateData {
public:
    /*!
     * @brief Set logical drive uuid
     * @param uuid Logical drive uuid
     */
    void set_uuid(const std::string& uuid) {
        m_uuid = uuid;
    }
    /*!
     * @brief Get logical drive uuid
     * @return Logical drive uuid
     */
    const std::string& get_uuid() const {
        return m_uuid;
    }
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
     * @param logical Logical volume name
     */
    void set_logical_volume(const std::string& logical) {
        m_logical_volume = logical;
    }

    /*!
     * @brief Get logical volume name
     * @return  Logical volume name
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
    void set_bootable(const bool bootable) {
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
    std::uint64_t m_size{};
    std::string m_uuid{};
    std::string m_volume_group{};
    std::string m_logical_volume{};
    std::string m_create_name{};
    bool m_bootable{};
};

}
}
}
#endif	/* PSME_AGENT_STORAGE_LVM_LVM_CREATE_DATA_HPP */

