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
 * @file logical_drive.hpp
 * @brief LogicalDrive model interface
 * */

#pragma once



#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/block_device.hpp"
#include "agent-framework/module/enum/storage.hpp"
#include "agent-framework/module/enum/common.hpp"



namespace agent_framework {
namespace model {

/*! LogicalDrive */
class LogicalDrive : public BlockDevice {
public:

    explicit LogicalDrive(const std::string& parent_uuid = {},
                          enums::Component parent_type = enums::Component::StorageService);


    virtual ~LogicalDrive();


    LogicalDrive(const LogicalDrive&) = default;


    LogicalDrive& operator=(const LogicalDrive&) = default;


    LogicalDrive(LogicalDrive&&) = default;


    LogicalDrive& operator=(LogicalDrive&&) = default;


    /*!
     * @brief construct an object of class LogicalDrive from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed LogicalDrive object
     */
    static LogicalDrive from_json(const json::Json& json);


    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;


    /*!
     * @brief Get collection name
     *
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return LogicalDrive::collection_name;
    }


    /*!
     * @brief Get component name
     *
     * @return component name
     */
    static enums::Component get_component() {
        return LogicalDrive::component;
    }


    /*!
     * Get the type of logical drive
     *
     * @return LogicalDrive type
     * */
    const OptionalField<enums::LogicalDriveType> get_type() const {
        return m_type;
    }


    /*!
     * @brief Set logical_drive type
     *
     * @param[in] logical_drive_type LogicalDrive type
     * */
    void set_type(const OptionalField<enums::LogicalDriveType> logical_drive_type) {
        m_type = logical_drive_type;
    }


    /*!
     * Get Logical Drive size in GB
     *
     * @return Logical Drive size in GB
     * */
    const OptionalField<double>& get_capacity_gb() const {
        return m_capacityGB;
    }


    /*!
     * @brief Set LogicalDrive size in GB
     *
     * @param[in] logical_drive_capacity_gb LogicalVolume's size in GB
     * */
    void set_capacity_gb(const OptionalField<double>& logical_drive_capacity_gb) {
        m_capacityGB = logical_drive_capacity_gb;
    }


    /*!
     * Get the logical drive mode
     *
     * @return LogicalDrive mode
     * */
    const OptionalField<enums::LogicalDriveMode> get_mode() const {
        return m_mode;
    }


    /*!
     * @brief Set logical_drive mode
     *
     * @param[in] logical_drive_mode LogicalDrive mode
     * */
    void set_mode(const OptionalField<enums::LogicalDriveMode> logical_drive_mode) {
        m_mode = logical_drive_mode;
    }


    /*!
     * Get the logical drive master drive uuid
     *
     * @return LogicalDrive master drive uuid
     * */
    const OptionalField<std::string>& get_master() const {
        return m_master;
    }


    /*!
     * @brief Set logical_drive master drive uuid
     *
     * @param[in] logical_drive_master LogicalDrive master drive uuid
     * */
    void set_master(const OptionalField<std::string>& logical_drive_master) {
        m_master = logical_drive_master;
    }


    /*!
     * Get information about image populated on the logical drive
     *
     * @return LogicalDrive image info
     * */
    const OptionalField<std::string>& get_image() const {
        return m_image;
    }


    /*!
     * @brief Set logical_drive image info
     *
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
    const OptionalField<bool>& get_bootable() const {
        return m_bootable;
    }


    /*!
     * @brief Set if the logical_drive is bootable
     *
     * @param[in] logical_drive_is_bootable LogicalDrive bootable option
     * */
    void set_bootable(const OptionalField<bool>& logical_drive_is_bootable) {
        m_bootable = logical_drive_is_bootable;
    }


    /*!
     * Get information if the logical drive is write/delete protected
     *
     * @return LogicalDrive protected flag
     * */
    const OptionalField<bool>& get_protected() const {
        return m_protected;
    }


    /*!
     * @brief Set the logical_drive write/delete protection flag
     *
     * @param[in] logical_drive_is_protected LogicalDrive protected flag
     * */
    void set_protected(const OptionalField<bool>& logical_drive_is_protected) {
        m_protected = logical_drive_is_protected;
    }


    /*!
     * Get information if the logical drive is snapshot
     *
     * @return LogicalDrive snapshot option
     * */
    const OptionalField<bool>& is_snapshot() const {
        return m_is_snapshot;
    }


    /*!
     * @brief Set if the logical_drive is snapshot
     *
     * @param[in] snapshot LogicalDrive snapshot option
     * */
    void set_is_snapshot(const OptionalField<bool>& snapshot) {
        m_is_snapshot = snapshot;
    }


private:

    OptionalField<enums::LogicalDriveType> m_type{enums::LogicalDriveType::LVM};
    OptionalField<double> m_capacityGB{};
    OptionalField<enums::LogicalDriveMode> m_mode{};
    OptionalField<std::string> m_master{};
    OptionalField<bool> m_is_snapshot{};
    OptionalField<std::string> m_image{};
    OptionalField<bool> m_bootable{};
    OptionalField<bool> m_protected{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;
};

}
}

