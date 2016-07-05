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
 * @file target.hpp
 * @brief Target interface
 * */
#ifndef AGENT_FRAMEWORK_MODULE_TARGET_HPP
#define AGENT_FRAMEWORK_MODULE_TARGET_HPP

#include "agent-framework/module/oem_data.hpp"
#include "agent-framework/module/status.hpp"
#include "agent-framework/module/resources.hpp"
#include "agent-framework/module/hard_drive.hpp"
#include "agent-framework/module/logical_drive.hpp"
#include <memory>
#include <vector>

namespace agent_framework {
namespace generic {

/*! Target */
class Target : public Resources {
public:
    class Lun {
    public:
        /*!
         * @brief Set lun id
         * @param lun_id Lun id
         */
        void set_lun_id(const std::uint64_t lun_id) {
            m_lun_id = lun_id;
        }

        /*!
         * @brief Get lun id
         * @return Lun id
         */
        std::uint64_t get_lun_id() const {
            return m_lun_id;
        }

        /*!
         * @brief Set lun device path
         * @param device_path Lun device path
         */
        void set_device_path(const std::string& device_path) {
            m_device_path = device_path;
        }

        /*!
         * @brief Get lun device path
         * @return Lun device path
         */
        const std::string& get_device_path() const {
            return m_device_path;
        }

        /*!
         * @brief Set logical drive uuid
         * @param uuid Logical drive uuid
         */
        void set_logical_drive_uuid(const std::string& uuid) {
            m_logical_drive_uuid = uuid;
        }

        /*!
         * @brief Get logical drive uuid
         * @return Logical drive uuid
         */
        const std::string& get_logical_drive_uuid() const {
            return m_logical_drive_uuid;
        }

    private:
        std::uint64_t m_lun_id{};
        std::string m_logical_drive_uuid{};
        std::string m_device_path{};
    };
    using LunVec = std::vector<Lun>;

    ~Target();

    /*! Enable copy */
    Target(const Target&) = default;
    Target& operator=(const Target&) = default;

    /*! Target pointers */
    using TargetSharedPtr = std::shared_ptr<Target>;
    using TargetWeakPtr = std::weak_ptr<Target>;

    using LogicalDriveSharedPtr = LogicalDrive::LogicalDriveSharedPtr;
    using HardDriveSharedPtr = HardDrive::HardDriveSharedPtr;

    /*! Default constructor */
    Target(){}

    /*! Creates Target shared object */
    static TargetSharedPtr make_target() {
        return std::make_shared<Target>();
    }

    /*!
     * @brief Get target lun
     * @return Const target lun
     */
    const LunVec& get_target_lun() const {
        return m_target_lun;
    }

    /*!
     * @brief Get target lun
     * @return Target lun
     */
    LunVec& get_target_lun() {
        return  const_cast<LunVec&>(
                static_cast<const Target*>(this)-> get_target_lun());
    }

    /*!
     * @brief Set target id
     * @param target_id Target id
     */
    void set_target_id(const std::int32_t target_id) {
        m_target_id = target_id;
    }

    /*!
     * @brief Get target id
     * @return Target id
     */
    std::int32_t get_target_id() const {
        return m_target_id;
    }

    /*!
     * @brief Set target lun
     * @param target_lun Target lun
     */
    void set_target_lun(const LunVec& target_lun) {
        m_target_lun = target_lun;
    }

    /*!
     * @brief Get target iqn
     * @return Target iqn
     */
    const std::string& get_target_iqn() const {
        return m_target_iqn;
    }

    /*!
     * @brief Add target lun
     * @param lun Target lun
     */
    void add_target_lun(const Lun& lun) {
        m_target_lun.emplace_back(lun);
    }

    /*!
     * @brief Set target iqn
     * @param target_iqn Target iqn
     */
    void set_target_iqn(const std::string& target_iqn) {
        m_target_iqn = target_iqn;
    }

    /*!
     * @brief Get target address
     * @return Target address
     */
    const std::string& get_target_address() const {
        return m_target_address;
    }

    /*!
     * @brief Set target address
     * @param target_address Target address
     */
    void set_target_address(const std::string& target_address) {
        m_target_address = target_address;
    }

    /*!
     * @brief Get target port
     * @return Target port
     */
    std::uint32_t get_target_port() const {
        return m_target_port;
    }

    /*!
     * @brief Set target port
     * @param port Target port
     */
    void set_target_port(const std::uint32_t port) {
        m_target_port = port;
    }

    /*!
     * @brief Get initiator iqn
     * @return Initiator iqn
     */
    const std::string& get_initiator_iqn() const {
        return m_initiator_iqn;
    }

    /*!
     * @brief Set target initiator iqn
     * @param initiator_iqn Target initiator iqn
     */
    void set_initiator_iqn(const std::string& initiator_iqn) {
        m_initiator_iqn = initiator_iqn;
    }

    /*!
     * @brief Set target oem data
     * @param oem_data target oem data
     */
    void set_oem_data(const OEMData& oem_data) {
        m_oem_data = oem_data;
    }

    /*!
     * @brief Get target oem data
     * @return target oem data
     */
    const OEMData& get_oem_data() const {
        return m_oem_data;
    }

    /*!
     * @brief Add new logical drive to Logical Drives collection
     *
     * @param[in] logical_drive  Pointer to new logical drive.
     * */
    void add_logical_drive(LogicalDriveSharedPtr logical_drive) {
        m_logical_drives.push_back(logical_drive);
    }

    /*!
     * @brief Return vector of logical drives.
     *
     * @return Reference to vector of logical drives.
     * */
    const std::vector<LogicalDriveSharedPtr>& get_logical_drives() const {
        return m_logical_drives;
    }

    /*!
     * @brief Add new hard drive to HardDrives collection
     *
     * @param[in] hard_drive Pointer to new physical drive.
     * */
    void add_hard_drive(const HardDriveSharedPtr& hard_drive) {
        m_hard_drives.push_back(hard_drive);
    }

    /*!
     * @brief Return vector of hard drives.
     *
     * @return Reference to vector of hard drives.
     * */
    const std::vector<HardDriveSharedPtr>& get_hard_drives() const {
        return m_hard_drives;
    }

private:
    Status m_status{};
    LunVec m_target_lun{};
    std::int32_t m_target_id{};
    std::string m_target_iqn{};
    std::string m_target_address{};
    std::uint32_t m_target_port{};
    std::string m_initiator_iqn{};
    OEMData m_oem_data{};
    std::vector<LogicalDriveSharedPtr> m_logical_drives{};
    std::vector<HardDriveSharedPtr> m_hard_drives{};

};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_TARGET_HPP */
