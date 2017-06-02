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
 * @file physical_drive.hpp
 * @brief Physical drive
 * */
#pragma once
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/block_device.hpp"
#include "agent-framework/module/enum/storage.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! Physical Drive */
class PhysicalDrive : public BlockDevice {
public:
    explicit PhysicalDrive(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::StorageServices);
    ~PhysicalDrive();

    PhysicalDrive(const PhysicalDrive&) = default;
    PhysicalDrive& operator=(const PhysicalDrive&) = default;
    PhysicalDrive(PhysicalDrive&&) = default;
    PhysicalDrive& operator=(PhysicalDrive&&) = default;

    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to Json::Value
     */
    Json::Value to_json() const;

    /*!
     * @brief construct an object of class PhysicalDrive from JSON
     *
     * @param json the Json::Value deserialized to object
     *
     * @return the newly constructed PhysicalDrive object
     */
    static PhysicalDrive from_json(const Json::Value& json);

    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return PhysicalDrive::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return PhysicalDrive::component;
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
    OptionalField<double> get_capacity_gb() const {
        return m_capacity_gb;
    }

    /*!
     * Sets drive capacity in GB
     * @param[in] capacity_gb Drive capacity in GB
     * */
    void set_capacity_gb(const OptionalField<double> capacity_gb) {
        m_capacity_gb = capacity_gb;
    }

    /*!
     * Gets the type
     * @return Drive's type
     * */
    OptionalField<enums::PhysicalDriveType> get_type() const { return m_type; }

    /*!
     * Sets drive type
     * @param[in] type Drive type
     * */
    void set_type(const OptionalField<enums::PhysicalDriveType> type) {
        m_type = type;
    }

    /*!
     * Sets drive interface
     * @param[in] interface Drive interface
     * */
    void set_interface(const OptionalField<enums::PhysicalDriveInterface> interface) {
        m_interface = interface;
    }

    /*!
     * Gets the interface
     * @return Drive's interface
     * */
    OptionalField<enums::PhysicalDriveInterface> get_interface() const { return m_interface; }

    /*!
     * @brief setter for rpm attribute
     *
     * @param rpm of type optional uint32_t
     */
    void set_rpm(const OptionalField<uint32_t> rpm) {
        m_rpm = rpm;
    }

    /*!
     * @brief return rpm of optional type uint32_t
     *
     * @return the rpm value
     */
    OptionalField<uint32_t> get_rpm() const {
        return m_rpm;
    }

private:

    OptionalField<enums::PhysicalDriveInterface> m_interface{enums::PhysicalDriveInterface::SATA};
    OptionalField<enums::PhysicalDriveType> m_type{enums::PhysicalDriveType::SSD};
    OptionalField<double> m_capacity_gb{};
    OptionalField<uint32_t> m_rpm{};
    attribute::FruInfo m_fru_info{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}

