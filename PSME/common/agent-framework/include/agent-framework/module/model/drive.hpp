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
 * @file drive.hpp
 * @brief Drive model
 * */

#pragma once



#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/common.hpp"



namespace agent_framework {
namespace model {

/*! Drive model class */
class Drive : public Resource {
public:
    using Locations = attribute::Array<attribute::Location>;
    using Identifiers = attribute::Array<attribute::Identifier>;


    explicit Drive(const std::string& parent_uuid = {},
                   enums::Component parent_type = enums::Component::StorageController);


    virtual ~Drive();


    Drive(const Drive&) = default;


    Drive& operator=(const Drive&) = default;


    Drive(Drive&&) = default;


    Drive& operator=(Drive&&) = default;


    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;


    /*!
     * @brief construct an object of class Drive from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed Drive object
     */
    static Drive from_json(const json::Json& json);


    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return Drive::collection_name;
    }


    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return Drive::component;
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
     * @brief Set latency tracking enabled
     * @param latency_tracking_enabled Drive's latency tracking status
     * */
    void set_latency_tracking_enabled(const OptionalField<bool>& latency_tracking_enabled) {
        m_latency_tracking_enabled = latency_tracking_enabled;
    }


    /*!
     * @brief Get latency tracking enabled
     * @return Latency tracking enabled
     * */
    const OptionalField<bool>& get_latency_tracking_enabled() const {
        return m_latency_tracking_enabled;
    }


    /*!
     * Gets FRU info
     * @return FRU info reference
     * */
    const attribute::FruInfo& get_fru_info() const {
        return m_fru_info;
    }


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
    const OptionalField<double>& get_capacity_gb() const {
        return m_capacity_gb;
    }


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
    const OptionalField<std::uint32_t>& get_rpm() const {
        return m_rpm;
    }


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
    const OptionalField<enums::DriveType>& get_type() const {
        return m_type;
    }


    /*!
     * Sets drive type
     * @param[in] type Drive type
     * */
    void set_type(const OptionalField<enums::DriveType>& type) {
        m_type = type;
    }


    /*!
     * Gets the interface
     * @return Drive's interface
     * */
    const OptionalField<enums::TransportProtocol>& get_interface() const {
        return m_interface;
    }


    /*!
     * Sets drive interface
     * @param[in] interface Drive interface
     * */
    void set_interface(const OptionalField<enums::TransportProtocol>& interface) {
        m_interface = interface;
    }


    /*!
     * Gets the indicator_led
     * @return Drive's indicator_led
     * */
    const OptionalField<enums::IndicatorLed>& get_indicator_led() const {
        return m_indicator_led;
    }


    /*!
     * Sets drive indicator_led
     * @param[in] indicator_led Drive indicator_led
     * */
    void set_indicator_led(const OptionalField<enums::IndicatorLed>& indicator_led) {
        m_indicator_led = indicator_led;
    }


    /*!
     * @brief Set asset tag
     *
     * @param asset_tag Drive's asset tag
     * */
    void set_asset_tag(const OptionalField<std::string>& asset_tag) {
        m_asset_tag = asset_tag;
    }


    /*!
     * @brief Get asset tag
     *
     * @return String with drive's asset tag
     * */
    const OptionalField<std::string>& get_asset_tag() const {
        return m_asset_tag;
    }


    /*!
     * Gets drive's capable speed in gigabytes per second
     * @return Drive's capable speed in gigabytes per second
     * */
    const OptionalField<double>& get_capable_speed_gbs() const {
        return m_capable_speed_gbs;
    }


    /*!
     * Sets drive capable speed in gigabytes per second
     * @param[in] capable_speed_gbs Drive's capable speed in gigabytes per second
     * */
    void set_capable_speed_gbs(const OptionalField<double>& capable_speed_gbs) {
        m_capable_speed_gbs = capable_speed_gbs;
    }


    /*!
     * Gets drive's negotiated speed in gigabytes per second
     * @return Drive's negotiated speed in gigabytes per second
     * */
    const OptionalField<double>& get_negotiated_speed_gbs() const {
        return m_negotiated_speed_gbs;
    }


    /*!
     * Sets drive negotiated speed in gigabytes per second
     * @param[in] negotiated_speed_gbs Drive's negotiated speed in gigabytes per second
     * */
    void set_negotiated_speed_gbs(const OptionalField<double>& negotiated_speed_gbs) {
        m_negotiated_speed_gbs = negotiated_speed_gbs;
    }


    /*!
     * @brief Get locations array
     * @return locations array
     * */
    const Locations& get_locations() const {
        return m_locations;
    }


    /*!
     * @brief adder for locations array
     *
     * @param location a Location to add
     */
    void add_location(const attribute::Location& location) {
        m_locations.add_entry(location);
    }


    /*!
     * @brief setter for locations array
     *
     * @param locations of type Locations
     */
    void set_locations(const Locations& locations) {
        m_locations = locations;
    }


    /*!
     * Gets the status_indicator
     * @return Drive's status_indicator
     * */
    const OptionalField<enums::StatusIndicator>& get_status_indicator() const {
        return m_status_indicator;
    }


    /*!
     * Sets drive status_indicator
     * @param[in] status_indicator Drive status_indicator
     * */
    void set_status_indicator(const OptionalField<enums::StatusIndicator>& status_indicator) {
        m_status_indicator = status_indicator;
    }


    /*!
     * @brief Set revision
     *
     * @param revision Drive's revision
     * */
    void set_revision(const OptionalField<std::string>& revision) {
        m_revision = revision;
    }


    /*!
     * @brief Get revision
     *
     * @return String with drive's revision
     * */
    const OptionalField<std::string>& get_revision() const {
        return m_revision;
    }


    /*!
     * @brief Set the drive's failure predicted
     *
     * @param failure_predicted Drive's the drive's failure predicted
     * */
    void set_failure_predicted(const OptionalField<bool>& failure_predicted) {
        m_failure_predicted = failure_predicted;
    }


    /*!
     * @brief Get failure predicted
     *
     * @return Is the drive's failure predicted?
     * */
    const OptionalField<bool>& get_failure_predicted() const {
        return m_failure_predicted;
    }


    /*!
     * @brief Set sku
     *
     * @param sku Drive's sku
     * */
    void set_sku(const OptionalField<std::string>& sku) {
        m_sku = sku;
    }


    /*!
     * @brief Get sku
     *
     * @return String with drive's sku
     * */
    const OptionalField<std::string>& get_sku() const {
        return m_sku;
    }


    /*!
     * @brief Get identifiers array
     * @return identifiers array
     * */
    const Identifiers& get_identifiers() const {
        return m_identifiers;
    }


    /*!
     * @brief adder for identifiers array
     *
     * @param identifier a Identifier to add
     */
    void add_identifier(const attribute::Identifier& identifier) {
        m_identifiers.add_entry(identifier);
    }


    /*!
     * @brief setter for identifiers array
     *
     * @param identifiers of type Identifiers
     */
    void set_identifiers(const Identifiers& identifiers) {
        m_identifiers = identifiers;
    }


    /*!
     * Gets the hotspare_type
     * @return Drive's hotspare_type
     * */
    const OptionalField<enums::HotspareType>& get_hotspare_type() const {
        return m_hotspare_type;
    }


    /*!
     * Sets drive hotspare_type
     * @param[in] hotspare_type Drive hotspare_type
     * */
    void set_hotspare_type(const OptionalField<enums::HotspareType>& hotspare_type) {
        m_hotspare_type = hotspare_type;
    }


    /*!
     * Gets the encryption_ability
     * @return Drive's encryption_ability
     * */
    const OptionalField<enums::EncryptionAbility>& get_encryption_ability() const {
        return m_encryption_ability;
    }


    /*!
     * Sets drive encryption_ability
     * @param[in] encryption_ability Drive encryption_ability
     * */
    void set_encryption_ability(const OptionalField<enums::EncryptionAbility>& encryption_ability) {
        m_encryption_ability = encryption_ability;
    }


    /*!
     * Gets the encryption_status
     * @return Drive's encryption_status
     * */
    const OptionalField<enums::EncryptionStatus>& get_encryption_status() const {
        return m_encryption_status;
    }


    /*!
     * Sets drive encryption_status
     * @param[in] encryption_status Drive encryption_status
     * */
    void set_encryption_status(const OptionalField<enums::EncryptionStatus>& encryption_status) {
        m_encryption_status = encryption_status;
    }


    /*!
     * @brief Set block_size_bytes
     *
     * @param block_size_bytes Drive's block_size_bytes
     * */
    void set_block_size_bytes(const OptionalField<int32_t>& block_size_bytes) {
        m_block_size_bytes = block_size_bytes;
    }


    /*!
     * @brief Get block_size_bytes
     *
     * @return the drive's block_size_bytes
     * */
    const OptionalField<int32_t>& get_block_size_bytes() const {
        return m_block_size_bytes;
    }


    /*!
     * @brief Set predicted_media_life_left
     *
     * @param predicted_media_life_left Drive's predicted_media_life_left
     * */
    void set_predicted_media_life_left(const OptionalField<int32_t>& predicted_media_life_left) {
        m_predicted_media_life_left = predicted_media_life_left;
    }


    /*!
     * @brief Get predicted_media_life_left
     *
     * @return the drive's predicted_media_life_left
     * */
    const OptionalField<int32_t>& get_predicted_media_life_left() const {
        return m_predicted_media_life_left;
    }


    /*!
     * @brief Set whether the drive has been erased
     * @param erased Drive's erasure status
     * */
    void set_erased(bool erased) {
        m_erased = erased;
    }


    /*!
     * @brief Get erased
     * @return true if the drive has been erased
     * */
    bool get_erased() const {
        return m_erased;
    }


    /*!
     * @brief Sets erase flag
     * @param is_being_erased New value
     * */
    void set_is_being_erased(bool is_being_erased) {
        m_is_being_erased = is_being_erased;
    }


    /*!
     * @brief Gets erase flag
     * @return true if the drive is currently being erased
     * */
    bool get_is_being_erased() const {
        return m_is_being_erased;
    }


    /*!
     * @brief Sets warning flag
     * @param is_in_warning_state New value
     * */
    void set_is_in_warning_state(bool is_in_warning_state) {
        m_is_in_warning_state = is_in_warning_state;
    }


    /*!
     * @brief Gets warning flag
     * @return true if the drive is in the warning state
     * */
    bool get_is_in_warning_state() const {
        return m_is_in_warning_state;
    }


    /*!
     * @brief Sets critical discovery flag
     * @param is_in_critical_discovery_state New value
     * */
    void set_is_in_critical_discovery_state(bool is_in_critical_discovery_state) {
        m_is_in_critical_discovery_state = is_in_critical_discovery_state;
    }


    /*!
     * @brief Gets critical discovery flag
     * @return true if the drive is in the critical discovery state
     * */
    bool get_is_in_critical_discovery_state() const {
        return m_is_in_critical_discovery_state;
    }


    /*!
     * @brief Sets is being discovered flag
     * @param is_being_discovered New value of the is being discovered flag
     * */
    void set_is_being_discovered(bool is_being_discovered) {
        m_is_being_discovered = is_being_discovered;
    }


    /*!
     * @brief Gets is being discovered flag
     * @return true if the drive is currently being discovered
     * */
    bool get_is_being_discovered() const {
        return m_is_being_discovered;
    }


    /*!
     * @brief Sets last smart health
     * @param smart_health New value of the smart health
     * */
    void set_last_smart_health(enums::Health smart_health) {
        m_last_smart_health = smart_health;
    }


    /*!
     * @brief Gets last smart health
     * @return Last smart health
     * */
    enums::Health get_last_smart_health() const {
        return m_last_smart_health;
    }


    /*!
     * @brief Get dsp port uuids
     * @return Dsp port uuids
     */
    const std::vector<std::string>& get_dsp_port_uuids() const {
        return m_dsp_port_uuids;
    }


    /*!
     * @brief Set dsp port uuids
     * @param[in] dsp_port_uuids Dsp port uuids
     */
    void set_dsp_port_uuids(const std::vector<std::string>& dsp_port_uuids) {
        m_dsp_port_uuids = dsp_port_uuids;
    }


    /*!
     * @brief Add dsp port uuid to the array
     * @param[in] dsp_port_uuid Dsp port uuid
     */
    void add_dsp_port_uuid(const std::string& dsp_port_uuid) {
        m_dsp_port_uuids.push_back(dsp_port_uuid);
    }


private:

    OptionalField<enums::TransportProtocol> m_interface{};
    OptionalField<enums::DriveType> m_type{};
    OptionalField<double> m_capacity_gb{};
    attribute::FruInfo m_fru_info{};
    OptionalField<std::string> m_firmware_version{};
    OptionalField<bool> m_latency_tracking_enabled{};
    OptionalField<std::uint32_t> m_rpm{};
    OptionalField<enums::IndicatorLed> m_indicator_led{};
    OptionalField<std::string> m_asset_tag{};
    OptionalField<double> m_capable_speed_gbs{};
    OptionalField<double> m_negotiated_speed_gbs{};
    Locations m_locations{};
    OptionalField<enums::StatusIndicator> m_status_indicator{};
    OptionalField<std::string> m_revision{};
    OptionalField<bool> m_failure_predicted{};
    OptionalField<std::string> m_sku{};
    Identifiers m_identifiers{};
    OptionalField<enums::HotspareType> m_hotspare_type{};
    OptionalField<enums::EncryptionAbility> m_encryption_ability{};
    OptionalField<enums::EncryptionStatus> m_encryption_status{};
    OptionalField<int32_t> m_block_size_bytes{};
    OptionalField<int32_t> m_predicted_media_life_left{};
    bool m_erased{false};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

    // agent-specific data related to erase operation
    bool m_is_being_erased{false};
    bool m_is_in_warning_state{false};
    // agent-specific data used to mark if for some reason drive is inaccessible to the management host
    bool m_is_in_critical_discovery_state{false};
    // agent-specific status override for drives under in-band discovery
    bool m_is_being_discovered{false};
    // required to locate drive's port on a pnc agent in case of the oob discovery, may be more than one
    std::vector<std::string> m_dsp_port_uuids{};
    // stores health previously read from smart
    enums::Health m_last_smart_health{enums::Health::OK};

};

}
}
