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
 * @file switch.hpp
 * @brief Definition of Fabric Switch class
 * */
#pragma once
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/pnc.hpp"

namespace agent_framework {
namespace model {

/*! @brief Switch class */
class Switch : public Resource {
public:

    using AllowedActions = attribute::Array<enums::ResetType>;

    explicit Switch(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::Fabric);
    ~Switch();

    /*! Enable copy */
    Switch(const Switch&) = default;
    Switch& operator=(const Switch&) = default;
    /*! Enable move */
    Switch(Switch&&) = default;
    Switch& operator=(Switch&&) = default;

    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return Switch::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     * */
    static enums::Component get_component() {
        return Switch::component;
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
    static Switch from_json(const json::Json& json);

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
     * @brief Set switch identifier
     * @param[in] switch_id Switch identifier
     * */
    void set_switch_id(const OptionalField<std::string>& switch_id) {
        m_switch_id = switch_id;
    }

    /*!
     * @brief Get switch identifier
     * @return Switch identifier
     * */
    const OptionalField<std::string>& get_switch_id() const {
        return m_switch_id;
    }

    /*!
     * @brief Set sku
     * @param[in] sku sku
     * */
    void set_sku(const OptionalField<std::string>& sku) {
        m_sku = sku;
    }

    /*!
     * @brief Get sku
     * @return sku
     * */
    const OptionalField<std::string>& get_sku() const {
        return m_sku;
    }

    /*!
     * @brief Set asset tag
     * @param[in] asset_tag asset tag
     * */
    void set_asset_tag(const OptionalField<std::string>& asset_tag) {
        m_asset_tag = asset_tag;
    }

    /*!
     * @brief Get asset tag
     * @return asset tag
     * */
    const OptionalField<std::string>& get_asset_tag() const {
        return m_asset_tag;
    }

    /*!
     * @brief Set chassis
     * @param[in] chassis chassis
     * */
    void set_chassis(const OptionalField<std::string>& chassis) {
        m_chassis = chassis;
    }

    /*!
     * @brief Get chassis
     * @return chassis
     * */
    const OptionalField<std::string>& get_chassis() const {
        return m_chassis;
    }

    /*!
     * @brief Set memory path
     * @param[in] memory_path Memory Path
     */
    void set_memory_path(const std::string& memory_path) {
        m_memory_path = memory_path;
    }

    /*!
     * @brief Get memory path
     * @return memory path
     * */
    const std::string& get_memory_path() const {
        return m_memory_path;
    }

    /*!
     * @brief Set bridge device path
     * @param[in] bridge_path Bridge device path
     */
    void set_bridge_path(const std::string& bridge_path) {
        m_bridge_path = bridge_path;
    }

    /*!
     * @brief Get bridge device path
     * @return Bridge device path
     * */
    const std::string& get_bridge_path() const {
        return m_bridge_path;
    }

    /*!
     * @brief Set secondary bus number
     * @param[in] sec_bus_num Bridge secondary bus number
     */
    void set_sec_bus_num(const uint8_t sec_bus_num) {
        m_sec_bus_num = sec_bus_num;
    }

    /*!
     * @brief Get secondary bus number
     * @return Bridge secondary bus number
     * */
    uint8_t get_sec_bus_num() const {
        return m_sec_bus_num;
    }

    /*!
     * @brief Get indicator LED
     * @return indicator LED
     * */
    const OptionalField<enums::IndicatorLed>& get_indicator_led() const {
        return m_indicator_led;
    }

    /*!
     * @brief Set indicator LED
     * @param[in] indicator_led indicator LED
     * */
    void set_indicator_led(const OptionalField<enums::IndicatorLed>& indicator_led) {
        m_indicator_led = indicator_led;
    }

    /*!
     * @brief Get power state
     * @return power state
     * */
    const OptionalField<enums::PcieSwitchPowerState>& get_power_state() const {
        return m_power_state;
    }

    /*!
     * @brief Set power state
     * @param[in] power_state power state
     * */
    void set_power_state(const OptionalField<enums::PcieSwitchPowerState>& power_state) {
        m_power_state = power_state;
    }

    /*!
     * @brief Get protocol
     * @return protocol
     * */
    const OptionalField<enums::TransportProtocol>& get_protocol() const {
        return m_protocol;
    }

    /*!
     * @brief Set protocol
     * @param[in] protocol protocol
     * */
    void set_protocol(const OptionalField<enums::TransportProtocol>& protocol) {
        m_protocol = protocol;
    }

    /*!
     * @brief Returns list of all allowed actions on the switch
     * @return List of allowed actions
     */
    const AllowedActions& get_allowed_actions() const {
        return m_allowed_actions;
    }

    /*!
     * @brief Sets list of all allowed actions on the switch
     * @param allowed_actions List of allowed actions
     */
    void set_allowed_actions(const AllowedActions& allowed_actions) {
        m_allowed_actions = allowed_actions;
    }

    /*!
     * @brief Adds allowed action
     * @param allowed_action Allowed action
     */
    void add_allowed_action(const enums::ResetType& allowed_action) {
        m_allowed_actions.add_entry(allowed_action);
    }

private:
    OptionalField<std::string> m_switch_id{};
    OptionalField<std::string> m_chassis{};
    OptionalField<std::string> m_sku{};
    OptionalField<std::string> m_asset_tag{};
    attribute::FruInfo m_fru_info{};
    OptionalField<enums::IndicatorLed> m_indicator_led{};
    OptionalField<enums::PcieSwitchPowerState> m_power_state{};
    OptionalField<enums::TransportProtocol> m_protocol{};
    AllowedActions m_allowed_actions{};

    // path the the memory controller device for this switch
    std::string m_memory_path{};
    // name of the pci bridge device for this switch
    std::string m_bridge_path{};
    // secondary bus number for the switch
    std::uint8_t m_sec_bus_num{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;
};

}
}
