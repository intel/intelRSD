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
 * @brief System model interface
 * */
#ifndef AGENT_FRAMEWORK_MODULE_MODEL_SYSTEM_HPP
#define AGENT_FRAMEWORK_MODULE_MODEL_SYSTEM_HPP

#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"
#include "agent-framework/module-ref/model/resource.hpp"
#include "agent-framework/module-ref/enum/compute.hpp"
#include "agent-framework/module-ref/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! System */
class System : public Resource {
public:

    using BootOverrideSupported = attribute::Array<enums::BootOverrideSupported>;
    using PciDevices = attribute::Array<attribute::PciDevice>;
    using UsbDevices = attribute::Array<attribute::UsbDevice>;

    explicit System(const std::string& parent_uuid = {});
    ~System();

    System(const System &) = default;
    System & operator=(const System &) = default;
    System(System &&) = default;
    System & operator=(System &&) = default;

    /*!
     * @brief construct an object of class System from JSON
     *
     * @param json the Json::Value deserialized to object
     *
     * @return the newly constructed System object
     */
    static System from_json(const Json::Value& json);

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
    static enums::CollectionName get_collection_name() {
        return System::collection_name;
    }

    /**
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return System::component;
    }

    /*!
     * @brief Set bios version
     * @param[in] bios_version blade bios version
     * */
    void set_bios_version(const OptionalField<std::string>& bios_version) {
        m_bios_version = bios_version;
    }

    /*!
     * @brief Get bios version
     * @return Bios version
     * */
    const OptionalField<std::string>& get_bios_version() const {
        return m_bios_version;
    }

    /*!
     * @brief Set boot override
     * @param[in] boot_override blade boot override
     * */
    void set_boot_override(const enums::BootOverride& boot_override) {
        m_boot_override = boot_override;
    }

    /*!
     * @brief Get boot override
     * @return Boot override
     * */
    const enums::BootOverride& get_boot_override() const {
        return m_boot_override;
    }

    /*!
     * @brief Set boot override target
     * @param[in] boot_override_target blade boot override target
     * */
    void set_boot_override_target(
            const enums::BootOverrideTarget& boot_override_target) {
        m_boot_override_target = boot_override_target;
    }

    /*!
     * @brief Get boot override target
     * @return Boot override target
     * */
    const enums::BootOverrideTarget& get_boot_override_target() const {
        return m_boot_override_target;
    }
    /*!
     * @brief Set boot override supported
     * @param[in] boot_override_supported blade boot override supported
     * */
    void set_boot_override_supported(
            const BootOverrideSupported& boot_override_supported) {
        m_boot_override_supported = boot_override_supported;
    }

    /*!
     * @brief Get boot override supported
     * @return Boot override supported
     * */
    const BootOverrideSupported& get_boot_override_supported() const {
        return m_boot_override_supported;
    }

    /*!
     * @brief Get uefi target
     * @return Uefi target
     * */
    const OptionalField<std::string>& get_uefi_target() const {
        return m_uefi_target;
    }

    /*!
     * @brief Set uefi target
     * @param[in] uefi_target blade uefi target
     * */
    void set_uefi_target(const OptionalField<std::string>& uefi_target) {
        m_uefi_target = uefi_target;
    }

    /*!
     * @brief Set power state from request
     * @param power_state blade power state
     * */
    void set_power_state(const enums::PowerState& power_state) {
        m_power_state = power_state;
    }

    /*!
     * @brief Get power state
     * @return power state
     * */
    const enums::PowerState& get_power_state() const {
        return m_power_state;
    }

    /*!
     * @brief Set FRUInfo structure
     * @param[in] fru_info blade FRUInfo
     * */
    void set_fru_info(const attribute::FruInfo& fru_info) {
        m_fru_info = fru_info;
    }

    /*!
     * @brief Get FruInfo
     * @return FruInfo
     * */
    const attribute::FruInfo get_fru_info() const {
        return m_fru_info;
    }

    /*!
     * @brief Get stock keeping unit
     * @return stock keeping unit
     * */
    const OptionalField<std::string>& get_sku() const {
        return m_sku;
    }

    /*!
     * @brief Set stock keeping unit
     * @param[in] sku stock keeping unit
     * */
    void set_sku(const OptionalField<std::string>& sku) {
        m_sku = sku;
    }

    /*!
     * @brief Get asset tag
     * @return asset tag
     * */
    const OptionalField<std::string>& get_asset_tag() const {
        return m_asset_tag;
    }

    /*!
     * @brief Set asset tag
     * @param[in] asset_tag asset tag
     * */
    void set_asset_tag(const OptionalField<std::string>& asset_tag) {
        m_asset_tag = asset_tag;
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
     * @brief return an object chassis of class std::string
     *
     * @return the chassis value
     */
    const OptionalField<std::string>& get_chassis() const {
        return m_chassis;
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
     * @brief return an object pci_devices of class PciDevices
     *
     * @return the pci_devices value
     */
    const PciDevices& get_pci_devices() const {
        return m_pci_devices;
    }

    /*!
     * @brief setter for pci_devices attribute
     *
     * @param pci_devices of type PciDevices
     */
    void set_pci_devices(const PciDevices& pci_devices) {
        m_pci_devices = pci_devices;
    }

    /*!
     * @brief adds pci_device
     *
     * @param pci_device of type PciDevice
     */
    void add_pci_device(const attribute::PciDevice& pci_device) {
        m_pci_devices.add_entry(pci_device);
    }

    /*!
     * @brief adds boot_override_supported
     *
     * @param boot_override_supported of type BootOverrideSupported
     */
    void add_boot_override_supported(const enums::BootOverrideSupported& boot_override_supported) {
        m_boot_override_supported.add_entry(boot_override_supported);
    }

    /*!
     * @brief return an object usb_devices of class UsbDevices
     *
     * @return the usb_devices value
     */
    const UsbDevices& get_usb_devices() const {
        return m_usb_devices;
    }

    /*!
     * @brief setter for usb_devices attribute
     *
     * @param usb_devices of type UsbDevices
     */
    void set_usb_devices(const UsbDevices& usb_devices) {
        m_usb_devices = usb_devices;
    }

    /*!
     * @brief adds usb_device
     *
     * @param usb_device of type UsbDevice
     */
    void add_usb_device(const attribute::UsbDevice& usb_device) {
        m_usb_devices.add_entry(usb_device);
    }

    /*!
     * @brief returns connection data
     * @return connection data
     */
    const attribute::ConnectionData& get_connection_data() const {
        return m_connection_data;
    }

    /*!
     * @brief sets connection data
     * @param connection_data connection data
     */
    void set_connection_data(const attribute::ConnectionData& connection_data) {
        m_connection_data = connection_data;
    }

    /*!
    * @brief Set System GUID
    * @param[in] guid System GUID
    * */
    void set_guid(const std::string& guid) {
        m_guid = guid;
    }

    /*!
     * @brief Get System GUID
     * @return System GUID
     */
    const std::string& get_guid() const {
        return m_guid;
    }

    static bool is_chassis() { return true; }

private:
    OptionalField<std::string> m_bios_version{};
    enums::BootOverride m_boot_override{enums::BootOverride::Disabled};
    enums::BootOverrideTarget m_boot_override_target{
        enums::BootOverrideTarget::None};
    BootOverrideSupported m_boot_override_supported{};
    OptionalField<std::string> m_uefi_target{};
    enums::PowerState m_power_state{enums::PowerState::Off};
    PciDevices m_pci_devices{};
    UsbDevices m_usb_devices{};
    attribute::FruInfo m_fru_info{};
    OptionalField<std::string> m_sku{};
    OptionalField<std::string> m_asset_tag{};
    OptionalField<enums::IndicatorLed> m_indicator_led{};
    OptionalField<std::string> m_chassis{};
    attribute::ConnectionData m_connection_data{};
    std::string m_guid{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};
}
}

#endif /* AGENT_FRAMEWORK_MODULE_MODEL_SYSTEM_HPP */
