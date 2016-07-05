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
 * @file logical_drive.hpp
 * @brief LogicalDrive
 * */
#ifndef AGENT_FRAMEWORK_MODULE_LOGICAL_DRIVE_HPP
#define AGENT_FRAMEWORK_MODULE_LOGICAL_DRIVE_HPP

#include "agent-framework/module/collection.hpp"
#include "agent-framework/module/oem_data.hpp"
#include "agent-framework/module/block_device.hpp"
#include "agent-framework/module/hard_drive.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"

#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace agent_framework {
namespace generic {

/*! LogicalDrive */
class LogicalDrive : public generic::BlockDevice {
public:

    /*!
     * Const strings for LVM names
     * */
    class LvmTypes {
        public:
            static constexpr const char VOLUME_GROUP[] = "LVG";
            static constexpr const char LOGICAL_VOLUME[] = "LV";
            static constexpr const char PHYSICAL_VOLUME[] = "PV";
    };

    /*! LogicalDrive pointers */
    using LogicalDriveSharedPtr = std::shared_ptr<LogicalDrive>;
    using LogicalDriveWeakPtr = std::weak_ptr<LogicalDrive>;
    using HardDriveSharedPtr = HardDrive::HardDriveSharedPtr;

    /*! LogicalDrive constructor */
    LogicalDrive(const std::string& device_type) : BlockDevice(device_type) {}

    /*! Creates LogicalDrive unique object */
    static LogicalDriveSharedPtr make_logical_drive() {
        return LogicalDriveSharedPtr{ new LogicalDrive("LogicalDrive") };
    }

    /*!
     * Get Logical Drive size in GB
     *
     * @return Logical Drive size in GB
     * */
    double get_capacity_gb() const { return m_capacityGB; }


    /*!
     * @brief Set LogicalDrive size in GB
     * @param[in] logical_drive_capacity_gb LogicalVolume's size in GB
     * */
    void set_capacity_gb(const double logical_drive_capacity_gb) {
        m_capacityGB = logical_drive_capacity_gb;
    }

    /*!
     * Get the name
     *
     * @return LogicalDrive name
     * */
    const std::string& get_name() const { return m_name; }

     /*!
     * @brief Set logical_drive name
     * @param[in] logical_drive_name LogicalDrive name
     * */
    void set_name(const std::string& logical_drive_name) {
        m_name = logical_drive_name;
    }

    /*!
     * Get the type of logical drive
     *
     * @return LogicalDrive type
     * */
    const std::string& get_type() const { return m_type; }

     /*!
     * @brief Set logical_drive type
     * @param[in] logical_drive_type LogicalDrive type
     * */
    void set_type(const std::string& logical_drive_type) {
        m_type = logical_drive_type;
    }

    /*!
     * Get the logical drive mode
     *
     * @return LogicalDrive mode
     * */
    const std::string& get_mode() const { return m_mode; }
     /*!
     * @brief Set logical_drive mode
     * @param[in] logical_drive_mode LogicalDrive mode
     * */
    void set_mode(const std::string& logical_drive_mode) {
        m_mode = logical_drive_mode;
    }

    /*!
     * Get the logical drive master drive uuid
     *
     * @return LogicalDrive master drive uuid
     * */
    const std::string& get_master() const { return m_master; }

     /*!
     * @brief Set logical_drive master drive uuid
     * @param[in] logical_drive_master LogicalDrive master drive uuid
     * */
    void set_master(const std::string& logical_drive_master) {
        m_master = logical_drive_master;
    }

    /*!
     * Get information about image populated on the logical drive
     *
     * @return LogicalDrive image info
     * */
    const OptionalField<std::string>& get_image() const { return m_image; }

     /*!
     * @brief Set logical_drive image info
     * @param[in] logical_drive_image LogicalDrive image info
     * */
     void set_image(const OptionalField<std::string>& logical_drive_image) {
         m_image = logical_drive_image;
     }

    /*!
     * Get information if the logical drive is bootable
     *
     * @return LogicalDrive bootable option
     * */
    bool is_bootable() const { return m_bootable; }

     /*!
     * @brief Set if the logical_drive is bootable
     * @param[in] logical_drive_is_bootable LogicalDrive bootable option
     * */
    void set_bootable(const bool logical_drive_is_bootable) {
        m_bootable = logical_drive_is_bootable;
    }

    /*!
     * Get information if the logical drive is write/delete protected
     *
     * @return LogicalDrive protected flag
     * */
    bool is_protected() const { return m_protected; }

     /*!
     * @brief Set the logical_drive write/delete protection flag
     * @param[in] logical_drive_is_protected LogicalDrive protected flag
     * */
    void set_protected(const bool logical_drive_is_protected) {
        m_protected = logical_drive_is_protected;
    }

    /*!
     * Get information if the logical drive is snapshot
     *
     * @return LogicalDrive snapshot option
     * */
    bool is_snapshot() const { return m_snapshot; }

     /*!
     * @brief Set if the logical_drive is snapshot
     * @param[in] snapshot_flag LogicalDrive snapshot option
     * */
    void set_snapshot(const bool snapshot_flag) {
        m_snapshot = snapshot_flag;
    }

    /*!
     * Get OEM specific data
     *
     * @return LogicalDrive OEM specific data
     * */
    const generic::OEMData& get_oem() const { return m_oem; }

     /*!
     * @brief Set the logical_drive OEM specific data
     * @param[in] logical_drive_oem LogicalDriveOEM data
     * */
    void set_oem(const generic::OEMData& logical_drive_oem) {
        m_oem = logical_drive_oem;
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
     * @brief Find logical drive with given UUID
     * @param[in] uuid Logical drive UUID
     * @return Logical drive with given UUID
     * */
    LogicalDriveWeakPtr find_logical_drive(const string& uuid) const;

    /*!
     * @brief Delete logical drive with given UUID
     * @param[in] uuid Logical drive UUID
     * @return true if deleted successfuly, false otherwise
     * */
    bool delete_logical_drive(const std::string& uuid);

    /*!
     * @brief Add new hard drive to HardDrives collection
     *
     * @param[in] hard_drive Pointer to new physical drive.
     * */
    void add_hard_drive(const HardDriveSharedPtr& hard_drive) {
        m_hard_drives.push_back(hard_drive);
    }

    /*!
     * @brief Delete hard drive from HardDrives collection
     *
     * @param[in] hard_drive Pointer to deleting physical drive.
     * */
    void remove_hard_drive(const HardDriveSharedPtr& hard_drive) {
        m_hard_drives.erase(std::remove(m_hard_drives.begin(), m_hard_drives.end(), hard_drive), m_hard_drives.end());
    }

    /*!
     * @brief Return vector of hard drives.
     *
     * @return Reference to vector of hard drives.
     * */
    const std::vector<HardDriveSharedPtr>& get_hard_drives() const {
        return m_hard_drives;
    }

    /*!
     * @brief Return collection object.
     *
     * @return Reference to collections.
     * */
    const generic::Collection& get_collections() const {
        return m_collections;
    }

    /*!
     * @brief Add new collection
     *
     * @param[in] entry  New collection entry.
     * */
    void add_collection(const generic::Collection::CollectionEntry& entry) {
        m_collections.add_collection_entry(entry);
    }

    /*!
     * @brief Set volume group
     *
     * @param[in] volume_group Pointer to volume_group
     * */
    void set_volume_group(LogicalDriveSharedPtr& volume_group) {
        m_volume_group = volume_group;
    }

    /*!
     * @brief Return volume group
     *
     * @return Pointer to volume group
     * */
    const LogicalDriveSharedPtr& get_volume_group() const {
        return m_volume_group;
    }

    ~LogicalDrive();

private:
    std::string m_name{};
    double m_capacityGB{};
    std::string m_type{};
    std::string m_mode{};
    std::string m_master{};
    OptionalField<std::string> m_image{};
    bool m_bootable{};
    bool m_protected{};
    bool m_snapshot{};
    generic::OEMData m_oem{};
    std::vector<LogicalDriveSharedPtr> m_logical_drives{};
    std::vector<HardDriveSharedPtr> m_hard_drives{};
    generic::Collection m_collections{};
    LogicalDriveSharedPtr m_volume_group{};
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_LOGICAL_DRIVE_HPP */
