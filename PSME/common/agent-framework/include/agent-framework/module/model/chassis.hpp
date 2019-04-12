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

    using AllowedResetActions = attribute::Array<enums::ResetType>;


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
    static Chassis from_json(const json::Json& json);


    /*!
     * @brief Convert an object to json
     * @return Json object
     */
    json::Json to_json() const;


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
     * @brief Set location id from optional string
     * @param[in] location_id Parent id
     * */
    void set_location_id(const OptionalField<std::string>& location_id) {
        m_location_id = location_id;
    }


    /*!
     * @brief Get Location id as optional string
     * @return Location id
     * */
    const OptionalField<std::string>& get_location_id() const {
        return m_location_id;
    }


    /*!
     * @brief Set parent id from optional string
     * @param[in] parent_id Parent id
     * */
    void set_parent_id(const OptionalField<std::string>& parent_id) {
        m_parent_id = parent_id;
    }


    /*!
     * @brief Get Parent id as optional string
     * @return Parent id
     * */
    const OptionalField<std::string>& get_parent_id() const {
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
     * @brief Set GEO tag of the chassis
     * @param[in] geo_tag Chassis GEO tag
     * */
    void set_geo_tag(const OptionalField<std::string>& geo_tag) {
        m_geo_tag = geo_tag;
    }


    /*!
     * @brief Get GEO tag of the chassis
     * @return Chassis GEO tag
     * */
    const OptionalField<std::string>& get_geo_tag() const {
        return m_geo_tag;
    }


    /*!
     * @brief Set disaggregated power cooling support of the chassis
     * @param[in] disaggregated_power_cooling_support Chassis supports disaggregated power cooling
     * */
    void set_disaggregated_power_cooling_support(const OptionalField<bool>& disaggregated_power_cooling_support) {
        m_disaggregated_power_cooling_support = disaggregated_power_cooling_support;
    }


    /*!
     * @brief Get disaggregated power cooling support of the chassis
     * @return Disaggregated power cooling support
     * */
    const OptionalField<bool>& get_disaggregated_power_cooling_support() const {
        return m_disaggregated_power_cooling_support;
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
    * @brief Get Parent chassis uuid
    * @return Parent chassis uuid
    * */
    const std::string& get_parent_chassis() const {
        return m_parent_chassis;
    }


    /*!
     * @brief Set Parent chassis uuid
     * @param[in] parent_chassis Parent Chassis Uuid
     * */
    void set_parent_chassis(const std::string& parent_chassis) {
        m_parent_chassis = parent_chassis;
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


    /*!
     * @brief Gets slot id value
     * @return Slot id value
     */
    std::uint8_t get_slot_id() const {
        return m_slot_id;
    }


    /*!
     * @brief Sets slot id value
     * @param slot_id new slot id value
     */
    void set_slot_id(std::uint8_t slot_id) {
        m_slot_id = slot_id;
    }


    /*!
     * @brief Gets information whether chassis is managed by parent resource
     * @return Flag indicating whether chassis is managed by parent resource
     */
    bool get_is_managed() const {
        return m_is_managed;
    }


    /*!
     * @brief Sets flag indicating whether chassis is managed by parent resource
     * @param is_managed Flag indicating whether chassis is managed by parent resource
     */
    void set_is_managed(bool is_managed) {
        m_is_managed = is_managed;
    }


    /*!
     * @brief Returns list of all allowed reset actions on the chassis
     * @return List of allowed reset actions
     */
    const AllowedResetActions& get_allowed_reset_actions() const {
        return m_allowed_reset_actions;
    }


    /*!
     * @brief Sets list of all allowed reset actions on the chassis
     * @param allowed_reset_actions List of the allowed reset actions
     */
    void set_allowed_reset_actions(const AllowedResetActions& allowed_reset_actions) {
        m_allowed_reset_actions = allowed_reset_actions;
    }


private:

    enums::ChassisType m_type{enums::ChassisType::Module};
    bool m_is_managed{true};
    OptionalField<std::string> m_parent_id{};
    std::uint32_t m_location_offset{};
    OptionalField<std::string> m_location_id{};
    enums::PlatformType m_platform{enums::PlatformType::UNKNOWN};
    OptionalField<std::string> m_power_zone{};
    OptionalField<std::string> m_thermal_zone{};
    OptionalField<bool> m_disaggregated_power_cooling_support{};
    attribute::FruInfo m_fru_info{};
    std::uint32_t m_ipmb_type{};
    OptionalField<std::string> m_network_interface{};
    OptionalField<std::string> m_sku{};
    OptionalField<std::string> m_asset_tag{};
    OptionalField<std::string> m_geo_tag{};
    OptionalField<enums::IndicatorLed> m_indicator_led{};
    AllowedResetActions m_allowed_reset_actions{};

    // used by rmm agent to store slot within the parent chassis
    std::string m_parent_chassis{};
    std::uint8_t m_slot_id{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}
