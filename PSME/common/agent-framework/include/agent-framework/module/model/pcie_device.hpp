/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file pcie_device.hpp
 * @brief Definition of PCIe Device class
 * */
#pragma once
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/pnc.hpp"

namespace agent_framework {
namespace model {

/*! @brief PcieDevice class */
class PcieDevice : public Resource {
public:
    // Please note: in GAMI, PCIeDevices are under a Manager,
    // but in REST they are under Chassis.
    // parent_type is for the REST application, so it's Chassis.
    explicit PcieDevice(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::Chassis);
    ~PcieDevice();

    /*! Enable copy */
    PcieDevice(const PcieDevice&) = default;
    PcieDevice& operator=(const PcieDevice&) = default;
    /*! Enable move */
    PcieDevice(PcieDevice&&) = default;
    PcieDevice& operator=(PcieDevice&&) = default;

    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return PcieDevice::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     * */
    static enums::Component get_component() {
        return PcieDevice::component;
    }

    /*!
     * @brief Returns JSON representation of the object.
     * @return Json representation.
     * */
    json::Json to_json() const;

    /*!
     * @brief Constructs object from JSON
     * @param[in] json the json::Json deserialized to object
     * @return the newly constructed object
     */
    static PcieDevice from_json(const json::Json& json);

    /*!
     * @brief Set FruInfo structure
     * @param[in] fru_info FruInfo
     * */
    void set_fru_info(const attribute::FruInfo& fru_info) {
        m_fru_info = fru_info;
    }

    /*!
     * @brief Get FruInfo
     * @return FruInfo
     * */
    const attribute::FruInfo& get_fru_info() const {
        return m_fru_info;
    }

    /*!
     * @brief Set device identifier
     * @param[in] device_id Switch identifier
     * */
    void set_device_id(const OptionalField<std::string>& device_id) {
        m_device_id = device_id;
    }

    /*!
     * @brief Get device identifier
     * @return Device identifier
     * */
    const OptionalField<std::string>& get_device_id() const {
        return m_device_id;
    }

    /*!
     * @brief Set device class
     * @param[in] device_class Device class
     * */
    void set_device_class(const OptionalField<enums::PcieDeviceClass>& device_class) {
        m_device_class = device_class;
    }

    /*!
     * @brief Get device class
     * @return device class
     * */
    const OptionalField<enums::PcieDeviceClass>& get_device_class() const {
        return m_device_class;
    }

    /*!
     * @brief Get speed gbps
     * @return Speed gbps
     * */
    const OptionalField<std::uint32_t>& get_speed_gbps() const {
        return m_speed_gbps;
    }

    /*!
     * @brief Set speed gbps
     * @param[in] speed_gbps Speed gbps
     * */
    void set_speed_gbps(const OptionalField<std::uint32_t>& speed_gbps) {
        m_speed_gbps = speed_gbps;
    }

    /*!
     * @brief Get width
     * @return Width
     * */
    const OptionalField<std::uint32_t>& get_width() const {
        return m_width;
    }

    /*!
     * @brief Set width
     * @param[in] width Width
     * */
    void set_width(const OptionalField<std::uint32_t>& width) {
        m_width = width;
    }

    /*!
     * @brief Get max speed gbps
     * @return Max speed gbps
     * */
    const OptionalField<std::uint32_t>& get_max_speed_gbps() const {
        return m_max_speed_gbps;
    }

    /*!
     * @brief Set max speed gbps
     * @param[in] max_speed_gbps Max speed gbps
     * */
    void set_max_speed_gbps(const OptionalField<std::uint32_t>& max_speed_gbps) {
        m_max_speed_gbps = max_speed_gbps;
    }

    /*!
     * @brief Get max width
     * @return Max width
     * */
    const OptionalField<std::uint32_t>& get_max_width() const {
        return m_max_width;
    }

    /*!
     * @brief Set max width
     * @param[in] max_width Max width
     * */
    void set_max_width(const OptionalField<std::uint32_t>& max_width) {
        m_max_width = max_width;
    }

    /*!
     * @brief Get firmware version
     * @return Firmware version
     * */
    const OptionalField<std::string>& get_firmware_version() const {
        return m_firmware_version;
    }

    /*!
     * @brief Set firmware version
     * @param[in] firmware_version Firmware version
     * */
    void set_firmware_version(const OptionalField<std::string>& firmware_version) {
        m_firmware_version = firmware_version;
    }

    /*!
     * @brief Get chassis
     * @return Chassis
     * */
    const OptionalField<std::string>& get_chassis() const {
        return m_chassis;
    }

    /*!
     * @brief Set chassis
     * @param[in] chassis Chassis
     * */
    void set_chassis(const OptionalField<std::string>& chassis) {
        m_chassis = chassis;
    }

    /*!
     * @brief Get asset tag
     * @return Asset tag
     * */
    const OptionalField<std::string>& get_asset_tag() const {
        return m_asset_tag;
    }

    /*!
     * @brief Set chassis
     * @param[in] asset_tag Asset tag
     * */
    void set_asset_tag(const OptionalField<std::string>& asset_tag) {
        m_asset_tag = asset_tag;
    }

    /*!
     * @brief Get sku
     * @return Sku
     * */
    const OptionalField<std::string>& get_sku() const {
        return m_sku;
    }

    /*!
     * @brief Set sku
     * @param[in] sku Sku
     * */
    void set_sku(const OptionalField<std::string>& sku) {
        m_sku = sku;
    }

private:
    OptionalField<std::string> m_device_id{};
    OptionalField<enums::PcieDeviceClass> m_device_class{};
    OptionalField<std::uint32_t> m_speed_gbps{};
    OptionalField<std::uint32_t> m_width{};
    OptionalField<std::uint32_t> m_max_speed_gbps{};
    OptionalField<std::uint32_t> m_max_width{};
    OptionalField<std::string> m_firmware_version{};
    OptionalField<std::string> m_chassis{};
    OptionalField<std::string> m_asset_tag{};
    OptionalField<std::string> m_sku{};
    attribute::FruInfo m_fru_info{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;
};

}
}
