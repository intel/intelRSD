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
 * @file switch.hpp
 * @brief Switch model interface
 * */
#pragma once
#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"
#include "agent-framework/module-ref/model/resource.hpp"
#include "agent-framework/module-ref/enum/network.hpp"
#include "agent-framework/module-ref/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! Switch */
class Switch : public Resource {
public:
    explicit Switch(const std::string& parent_uuid = {});
    ~Switch();

    Switch(const Switch&) = default;
    Switch& operator=(const Switch&) = default;
    Switch(Switch&&) = default;
    Switch& operator=(Switch&&) = default;

    /*!
     * @brief construct an object of class Switch from JSON
     *
     * @param json the Json::Value deserialized to object
     *
     * @return the newly constructed Switch object
     */
    static Switch from_json(const Json::Value& json);

    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to Json::Value
     */
    Json::Value to_json() const;

    /**
     * @brief Get collection name
     * @return collection name
     */
    static const enums::CollectionName& get_collection_name() {
        return Switch::collection_name;
    }

    /**
     * @brief Get component name
     * @return component name
     */
    static const enums::Component& get_component() {
        return Switch::component;
    }

    /*!
     * @brief Set switch identifier
     * @param[in] switch_identifier switch identifier string
     * */
    void set_switch_identifier(const OptionalField<std::string>& switch_identifier) {
        m_switch_identifier = switch_identifier;
    }

    /*!
     * @brief Get switch identifier
     * @return switch identifier
     * */
    const OptionalField<std::string>& get_switch_identifier() const {
        return m_switch_identifier;
    }

    /*!
     * @brief Set mac address
     * @param[in] mac_address switch mac address
     * */
    void set_mac_address(const OptionalField<std::string>& mac_address) {
        m_mac_address = mac_address;
    }

    /*!
     * @brief Get mac address
     * @return mac address string
     * */
    const OptionalField<std::string>& get_mac_address() const {
        return m_mac_address;
    }

    /*!
     * @brief setter for technology attribute
     *
     * @param technology of type enums::SwitchTechnology
     */
    void set_technology(const OptionalField<enums::SwitchTechnology>& technology) {
        m_technology = technology;
    }

    /*!
     * @brief Get switch technology
     * @return switch technology
     * */
    const OptionalField<enums::SwitchTechnology>& get_technology() const {
        return m_technology;
    }

    /*!
     * @brief setter for fru_info attribute
     *
     * @param fru_info of type attribute::FruInfo
     */
    void set_fru_info(const attribute::FruInfo& fru_info) {
        m_fru_info = fru_info;
    }

    /*!
     * @brief Get switch FRU info
     * @return FRU info
     * */
    const attribute::FruInfo& get_fru_info() const {
        return m_fru_info;
    }

    /*!
     * @brief setter for location attribute
     *
     * @param location of type std::uint32_t
     */
    void set_location(const OptionalField<std::uint32_t>& location) {
        m_location = location;
    }

    /*!
     * @brief return location of type std::uint32_t
     *
     * @return the location value
     */
    const OptionalField<std::uint32_t>& get_location() const {
        return m_location;
    }

    /*!
     * @brief setter for chassis attribute
     *
     * @param chassis of type std::string
     */
    void set_chassis(const OptionalField<std::string>& chassis) {
        m_chassis = chassis;
    }

    /*!
     * @brief return an object chassis of class std::string
     *
     * @return the chassis value
     */
    const OptionalField<std::string>& get_chassis() const {
        return m_chassis;
    }



    /*!
     * @brief Setter for firmware name
     *
     * @param firmware_name firmware name to set or NIL to remove
     */
    void set_firmware_name(const OptionalField<std::string>& firmware_name) {
        m_firmware_name = firmware_name;
    }
    /*!
     * @brief Getter for firmware name
     *
     * @return firmware name as a string or NIL if not set
     */
    const OptionalField<std::string>& get_firmware_name() const {
        return m_firmware_name;
    }


    /*!
     * @brief Setter for firmware version
     *
     * @param firmware_version firmware version to set or NIL to remove
     */
    void set_firmware_version(const OptionalField<std::string>& firmware_version) {
        m_firmware_version = firmware_version;
    }
    /*!
     * @brief Getter for firmware version
     *
     * @return firmware version as a string or NIL if not set
     */
    const OptionalField<std::string>& get_firmware_version() const {
        return m_firmware_version;
    }


    /*!
     * @brief setter for switch role in the network
     *
     * @param role to be set
     */
    void set_role(const OptionalField<enums::SwitchRole>& role) {
        m_role = role;
    }
    /*!
     * @brief Get switch role
     * @return switch role
     * */
    const OptionalField<enums::SwitchRole>& get_role() const {
        return m_role;
    }


    /*!
     * @brief Setter for manufacturing date
     *
     * @param manufacturing_date manufacturing date to set or NIL to remove
     */
    void set_manufacturing_date(const OptionalField<std::string>& manufacturing_date) {
        m_manufacturing_date = manufacturing_date;
    }
    /*!
     * @brief Getter for manufacturing date
     *
     * @return manufacturing date as a string or NIL if not set
     */
    const OptionalField<std::string>& get_manufacturing_date() const {
        return m_manufacturing_date;
    }


    /*!
     * @brief Setter for max ACL number
     *
     * @param[in] max_acl_number max ACL number to set or NIL to remove
     */
    void set_max_acl_number(const OptionalField<std::uint32_t>
        max_acl_number) {
        m_max_acl_number = max_acl_number;
    }
    /*!
     * @brief Getter for max ACL number
     *
     * @return max ACL number or NIL if not set
     */
    const OptionalField<std::uint32_t> get_max_acl_number() const {
        return m_max_acl_number;
    }

private:
    OptionalField<std::string> m_switch_identifier{};
    OptionalField<enums::SwitchTechnology> m_technology{};
    OptionalField<std::string> m_mac_address{};
    OptionalField<std::string> m_firmware_name{}; // new in gami 0.82
    OptionalField<std::string> m_firmware_version{}; // new in gami 0.82
    OptionalField<enums::SwitchRole> m_role{}; // new in gami 0.82
    attribute::FruInfo m_fru_info{};
    OptionalField<std::string> m_manufacturing_date{}; // new in gami 0.82
    OptionalField<std::uint32_t> m_location{};
    OptionalField<std::string> m_chassis{};
    OptionalField<std::uint32_t> m_max_acl_number{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

} // namespace model
} // namespace agent_framework

