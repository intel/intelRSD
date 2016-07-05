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
 * @file physical_drive.hpp
 * @brief Physical drive
 * */
#ifndef AGENT_FRAMEWORK_MODULE_MODEL_DRIVE_HPP
#define AGENT_FRAMEWORK_MODULE_MODEL_DRIVE_HPP

#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"
#include "agent-framework/module-ref/model/block_device.hpp"
#include "agent-framework/module-ref/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! Physical Drive */
class Drive : public BlockDevice {
public:
    explicit Drive(const std::string& parent_uuid = {});
    ~Drive();

    Drive(const Drive&) = default;
    Drive& operator=(const Drive&) = default;
    Drive(Drive&&) = default;
    Drive& operator=(Drive&&) = default;

    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to Json::Value
     */
    Json::Value to_json() const;

    /*!
     * @brief construct an object of class Drive from JSON
     *
     * @param json the Json::Value deserialized to object
     *
     * @return the newly constructed Drive object
     */
    static Drive from_json(const Json::Value& json);

    /**
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return Drive::collection_name;
    }

    /**
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() { return Drive::component; }

    /*!
     * @brief Set physical id
     *
     * @param physical_id Drive physical location
     * */
    void set_physical_id(const OptionalField<std::string>& physical_id) {
        m_physical_id = physical_id;
    }

    /*!
     * @brief Get physical id
     *
     * @return String with drive physical location
     * */
    const OptionalField<std::string>& get_physical_id() const {
        return m_physical_id;
    }

    /*!
     * @brief Set firmware version
     * @param firmware_version firmware version of the drive
     * */
    void set_firmware_version(const OptionalField<std::string>& firmware_version) {
        m_firmware_version = firmware_version;
    }

    /*!
     * @brief Get firmware version
     * @return String with firmware version of the drive
     * */
    const OptionalField<std::string>& get_firmware_version() const {
        return m_firmware_version;
    }

    /*!
     * Gets FRU info
     * @return FRU info reference
     * */
    const attribute::FruInfo& get_fru_info() const { return m_fru_info; }

    /*!
     * Sets FRU info
     * @param[in] fru_info FRU info
     * */
    void set_fru_info(const attribute::FruInfo& fru_info) {
        m_fru_info = fru_info;
    }

    /*!
     * Gets drive's capacity in GB
     * @return Drive's capacity in GB
     * */
    const OptionalField<double>& get_capacity_gb() const { return m_capacity_gb; }

    /*!
     * Sets drive capacity in GB
     * @param[in] capacity_gb Drive capacity in GB
     * */
    void set_capacity_gb(const OptionalField<double>& capacity_gb) {
        m_capacity_gb = capacity_gb;
    }

    /*!
     * Gets drive's rpm (only for HDD drives)
     * @return Drive's rotation per minute
     * */
    const OptionalField<std::uint32_t>& get_rpm() const { return m_rpm; }

    /*!
     * Sets drive's rotation per minute
     * @param[in] rpm Drive's rotation per minute
     * */
    void set_rpm(const OptionalField<std::uint32_t>& rpm) {
        m_rpm = rpm;
    }

    /*!
     * Gets the type
     * @return Drive's type
     * */
    OptionalField<enums::DriveType> get_type() const { return m_type; }

    /*!
     * Sets drive type
     * @param[in] type Drive type
     * */
    void set_type(const OptionalField<enums::DriveType> type) {
        m_type = type;
    }

    /*!
     * Gets the interface
     * @return Drive's interface
     * */
    OptionalField<enums::DriveInterface> get_interface() const { return m_interface; }

    /*!
     * Sets drive interface
     * @param[in] interface Drive interface
     * */
    void set_interface(const OptionalField<enums::DriveInterface> interface) {
        m_interface = interface;
    }

private:

    OptionalField<enums::DriveInterface> m_interface{enums::DriveInterface::SATA};
    OptionalField<enums::DriveType> m_type{enums::DriveType::SSD};
    OptionalField<double> m_capacity_gb{};
    attribute::FruInfo m_fru_info{};
    OptionalField<std::string> m_physical_id{};
    OptionalField<std::string> m_firmware_version{};
    OptionalField<std::uint32_t> m_rpm{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_MODEL_DRIVE_HPP */
