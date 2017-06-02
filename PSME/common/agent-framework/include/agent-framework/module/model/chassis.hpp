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
 * @file chassis.hpp
 * @brief Chassis model interface
 * */

#pragma once
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/chassis.hpp"

#include <string>

namespace agent_framework {
namespace model {

/*! Chassis class */
class Chassis : public Resource {
public:

    explicit Chassis(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::None);
    ~Chassis();

    Chassis(const Chassis&) = default;
    Chassis& operator=(const Chassis&) = default;
    Chassis(Chassis&&) = default;
    Chassis& operator=(Chassis&&) = default;

    /*!
     * @brief Create an object from Json
     * @param json object
     * @return Chassis object
     */
    static Chassis from_json(const Json::Value& json);

    /*!
     * @brief Convert an object to json
     * @return Json object
     */
    Json::Value to_json() const;

    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return Chassis::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return Chassis::component;
    }

    /*!
     * @brief Set type
     * @param[in] type type
     * */
    void set_type(const enums::ChassisType& type) {
        m_type = type;
    }
    /*!
     * @brief Get type of chassis.
     * @return Chassis type.
     * */
    enums::ChassisType get_type() const {
        return m_type;
    }

    /*!
     * @brief Set size
     * @param[in] size size
     * */
    void set_size(const std::uint32_t& size) {
        m_size = size;
    }

    /*!
     * @brief Get size
     * @return Size
     * */
    const std::uint32_t& get_size() const {
        return m_size;
    }

    /*!
     * @brief Set location offset
     * @param[in] location location
     * */
    void set_location_offset(const std::uint32_t& location) {
        m_location_offset = location;
    }

    /*!
     * @brief Get location offset
     * @return Location offset
     * */
    const std::uint32_t& get_location_offset() const {
        return m_location_offset;
    }

    /*!
     * @brief Set parent id from optional string
     * @param[in] parent_id Parent id
     * */
    void set_parent_id(const std::string& parent_id) {
        m_parent_id = parent_id;
    }

    /*!
     * @brief Get Parent id as optional string
     * @return Parent id
     * */
    const std::string get_parent_id() const {
        return m_parent_id;
    }

    /*!
     * @brief Set platform type
     * @param platform Platform type
     */
    void set_platform(const enums::PlatformType& platform) {
        m_platform = platform;
    }

    /*!
     * @brief Get platform type
     * @return Platform type
     */
    const enums::PlatformType get_platform() const {
        return m_platform;
    }

    /*!
     * @brief Get thermal zone
     * @return Thermal zone
     * */
    const OptionalField<std::string>& get_thermal_zone() const {
        return m_thermal_zone;
    }

    /*!
     * @brief Set Thermal zone structure
     * @param[in] thermal_zone system thermal zone
     * */
    void set_thermal_zone(const OptionalField<std::string>& thermal_zone) {
        m_thermal_zone = thermal_zone;
    }

    /*!
     * @brief Get power zone
     * @return Power zone
     * */
    const OptionalField<std::string>& get_power_zone() const {
        return m_power_zone;
    }

    /*!
     * @brief Set Power zone structure
     * @param[in] power_zone system power zone
     * */
    void set_power_zone(const OptionalField<std::string>& power_zone) {
        m_power_zone = power_zone;
    }

    /*!
     * @brief Set FRUInfo structure
     * @param[in] fru_info system FRUInfo
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
     * @brief Set IPMB type
     * @param[in] type IPMB type
     * */
    void set_ipmb_type(const std::uint32_t type) {
        m_ipmb_type = type;
    }

    /*!
     * @brief Get IPMB type
     * @return IPMB type
     * */
    const std::uint32_t& get_ipmb_type() const {
        return m_ipmb_type;
    }

    /*!
    * @brief Get network interface name
    * @return network interface name
    * */
    const OptionalField<std::string>& get_network_interface() const {
        return m_network_interface;
    }

    /*!
     * @brief Set network interface name
     * @param[in] interface Network interface name
     * */
    void set_network_interface(const OptionalField<std::string>& interface) {
        m_network_interface = interface;
    }

    /*!
    * @brief Get SKU
    * @return SKU
    * */
    const OptionalField<std::string>& get_sku() const {
        return m_sku;
    }

    /*!
     * @brief Set SKU
     * @param[in] sku SKU
     * */
    void set_sku(const OptionalField<std::string>& sku) {
        m_sku = sku;
    }

    /*!
    * @brief Get Asset Tag
    * @return Asset Tag
    * */
    const OptionalField<std::string>& get_asset_tag() const {
        return m_asset_tag;
    }

    /*!
     * @brief Set Asset Tag
     * @param[in] asset_tag Asset Tag
     * */
    void set_asset_tag(const OptionalField<std::string>& asset_tag) {
        m_asset_tag = asset_tag;
    }

    /*!
    * @brief Get Indicator LED
    * @return Indicator LED
    * */
    OptionalField<enums::IndicatorLed> get_indicator_led() const {
        return m_indicator_led;
    }

    /*!
     * @brief Set Indicator LED
     * @param[in] indicator_led Indicator LED
     * */
    void set_indicator_led(const OptionalField<enums::IndicatorLed>& indicator_led) {
        m_indicator_led = indicator_led;
    }

private:

    enums::ChassisType m_type{enums::ChassisType::Module};
    std::uint32_t m_size{};
    std::uint32_t m_location_offset{};
    enums::PlatformType m_platform{enums::PlatformType::BDCR};
    OptionalField<std::string> m_power_zone{};
    OptionalField<std::string> m_thermal_zone{};
    attribute::FruInfo m_fru_info{};
    std::uint32_t m_ipmb_type{};
    OptionalField<std::string> m_network_interface{};
    OptionalField<std::string> m_sku{};
    OptionalField<std::string> m_asset_tag{};
    OptionalField<enums::IndicatorLed> m_indicator_led{};
    std::string m_parent_id{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}
