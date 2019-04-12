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
 * @brief System model interface
 * @file system.hpp
 * */
#pragma once



#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/compute.hpp"
#include "agent-framework/module/enum/common.hpp"



namespace agent_framework {
namespace model {

/*! System */
class System : public Resource {
public:
    using Reference = agent_framework::generic::ObjReference<System, std::recursive_mutex>;
    using BootOverrideSupported = attribute::Array<enums::BootOverrideTarget>;
    using PciDevices = attribute::Array<attribute::PciDevice>;
    using UsbDevices = attribute::Array<attribute::UsbDevice>;
    using CableIds = attribute::Array<std::string>;
    using PerformanceConfigurations = attribute::Array<attribute::PerformanceConfiguration>;


    explicit System(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::None);


    ~System();


    System(const System&) = default;


    System& operator=(const System&) = default;


    System(System&&) = default;


    System& operator=(System&&) = default;


    /*!
     * @brief construct an object of class System from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed System object
     */
    static System from_json(const json::Json& json);


    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;


    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return System::collection_name;
    }


    /*!
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
    void set_boot_override(const OptionalField<enums::BootOverride>& boot_override) {
        m_boot_override = boot_override;
    }


    /*!
     * @brief Get boot override
     * @return Boot override
     * */
    const OptionalField<enums::BootOverride>& get_boot_override() const {
        return m_boot_override;
    }


    /*!
     * @brief Set boot override mode
     * @param[in] boot_override_mode blade boot override mode
     * */
    void set_boot_override_mode(const OptionalField<enums::BootOverrideMode>& boot_override_mode) {
        m_boot_override_mode = boot_override_mode;
    }


    /*!
     * @brief Get boot override mode
     * @return Boot override mode
     * */
    const OptionalField<enums::BootOverrideMode>& get_boot_override_mode() const {
        return m_boot_override_mode;
    }


    /*!
     * @brief Set boot override target
     * @param[in] boot_override_target blade boot override target
     * */
    void set_boot_override_target(const OptionalField<enums::BootOverrideTarget>& boot_override_target) {
        m_boot_override_target = boot_override_target;
    }


    /*!
     * @brief Get boot override target
     * @return Boot override target
     * */
    const OptionalField<enums::BootOverrideTarget>& get_boot_override_target() const {
        return m_boot_override_target;
    }


    /*!
     * @brief Set boot override supported
     * @param[in] boot_override_supported blade boot override supported
     * */
    void set_boot_override_supported(const BootOverrideSupported& boot_override_supported) {
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
    void set_power_state(const OptionalField<enums::PowerState>& power_state) {
        m_power_state = power_state;
    }


    /*!
     * @brief Get power state
     * @return power state
     * */
    const OptionalField<enums::PowerState>& get_power_state() const {
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
     * @brief Get FRUInfo
     * @return FRUInfo
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
     * @brief Get system type
     * @return system type
     * */
    const OptionalField<enums::SystemType>& get_system_type() const {
        return m_system_type;
    }


    /*!
     * @brief Set system type
     * @param[in] system_type system type
     * */
    void set_system_type(const OptionalField<enums::SystemType>& system_type) {
        m_system_type = system_type;
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
    void add_boot_override_supported(const enums::BootOverrideTarget& boot_override_supported) {
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
    void set_guid(const OptionalField<std::string>& guid) {
        m_guid = guid;
    }


    /*!
     * @brief Get Cable IDs
     * @return Cable IDs
     */
    const CableIds& get_cable_ids() const {
        return m_cable_ids;
    }


    /*!
     * @brief Set Cable IDs
     * @param[in] cable_ids Cable IDs
     * */
    void set_cable_ids(const CableIds& cable_ids) {
        m_cable_ids = cable_ids;
    }


    /*!
     * @brief Add Cable Id
     * @param[in] cable_id Cable ID
     * */
    void add_cable_id(const std::string& cable_id) {
        m_cable_ids.add_entry(cable_id);
    }


    /*!
     * @brief Get System GUID
     * @return System GUID
     */
    const OptionalField<std::string>& get_guid() const {
        return m_guid;
    }


    /*!
     * @brief Tells the caller if the Model represents Chassis object
     * @return Information if Model represents chassis
     * */
    static bool is_chassis() {
        return true;
    }


    /*!
     * @brief Informs whether Trusted Execution Technology is Enabled or not
     * @return true if txt is enabled, false otherwise
     * */
    const OptionalField<bool>& is_txt_enabled() const {
        return m_txt_enabled;
    }


    /*!
     * @brief Set txt enabled
     * @param[in] value value that informs whether txt is enabled ot not
     * */
    void set_txt_enabled(const OptionalField<bool>& value) {
        m_txt_enabled = value;
    }


    /*!
     * @brief Check if user mode is enabled.
     * @return True if is enabled, false if not, empty OptionalField if not set.
     */
    const OptionalField<bool>& is_user_mode_enabled() const {
        return m_user_mode_enabled;
    }


    /*!
     * @brief Set user mode enabled flag.
     * @param[in] user_mode_enabled User mode enabled flag.
     */
    void set_user_mode_enabled(const OptionalField<bool>& user_mode_enabled) {
        m_user_mode_enabled = user_mode_enabled;
    }


    /*!
     * @brief Get current performance configuration ID.
     * @return Configuration ID.
     */
    const OptionalField<uint64_t>& get_current_performance_configuration() const {
        return m_current_performance_configuration;
    }


    /*!
     * @brief Set performance configuration ID.
     * @param[in] current_performance_configuration Configuration ID.
     */
    void set_current_performance_configuration(const OptionalField<uint64_t>& current_performance_configuration) {
        m_current_performance_configuration = current_performance_configuration;
    }


    /*!
     * @brief return an object performance_configurations of class PerformanceConfigurations
     *
     * @return the performance_configuration value
     */
    const PerformanceConfigurations& get_performance_configurations() const {
        return m_performance_configurations;
    }


    /*!
     * @brief setter for performance_configurations attribute
     *
     * @param performance_configurations of type PerformanceConfigurations
     */
    void set_performance_configurations(const PerformanceConfigurations& performance_configurations) {
        m_performance_configurations = performance_configurations;
    }


    /*!
     * @brief adds performance_configuration
     *
     * @param performance_configuration of type PerformanceConfiguration
     */
    void add_performance_configuration(const attribute::PerformanceConfiguration& performance_configuration) {
        m_performance_configurations.add_entry(performance_configuration);
    }


    /*!
     * @brief Check if Rackscale mode is enabled.
     * @return True if is enabled, false if not.
     */
    bool is_rackscale_mode_enabled() const {
        return m_rackscale_mode_enabled;
    }


    /*!
     * @brief Set rackscale mode enabled flag.
     * @param[in] rackscale_mode_enabled Rackscale mode enabled flag.
     */
    void set_rackscale_mode_enabled(bool rackscale_mode_enabled) {
        m_rackscale_mode_enabled = rackscale_mode_enabled;
    }


    /*!
     * @brief Set power state "lock" to appropriate value.
     * @param is_being_changed if power state command is being changed (task is running)
     * @warning Not in the model, modified only in set_component_attributes in compute agent.
     */
    void set_power_state_being_changed(bool is_being_changed) {
        power_state_is_being_changed = is_being_changed;
    }

    /*!
     * @brief Check if power state command is running for this system
     * @return true if power state command is running for this system
     */
    bool is_power_state_being_changed() const {
        return power_state_is_being_changed;
    }

    /*!
     * @brief Get DCPMEM Erase task
     * @return Memory task
     * */
    const OptionalField<std::string>& get_dcpmem_erase_task() const {
        return m_dcpmem_erase_task;
    }

    /*!
     * @brief Set DCPMEM Erase task
     * @param[in] task the Memory task
     * */
    void set_dcpmem_erase_task(const OptionalField<std::string>& dcpmem_erase_task) {
        m_dcpmem_erase_task = dcpmem_erase_task;
    }

    /*!
     * @brief Get performance configuration task
     * @return Perfomance configuration task
     * */
    bool is_performance_configuration_task_running() const {
        return m_is_performance_configuration_task_running;
    }

    /*!
     * @brief Set performance configuration task
     * @param[in] performance_configuration_task the configuration task
     * */
    void set_is_performance_configuration_task_running(bool performance_configuration_task_running) {
        m_is_performance_configuration_task_running = performance_configuration_task_running;
    }

private:
    OptionalField<enums::SystemType> m_system_type{enums::SystemType::Physical};
    OptionalField<std::string> m_bios_version{};
    OptionalField<enums::BootOverride> m_boot_override{};
    OptionalField<enums::BootOverrideMode> m_boot_override_mode{};
    OptionalField<enums::BootOverrideTarget> m_boot_override_target{};
    BootOverrideSupported m_boot_override_supported{};
    OptionalField<std::string> m_uefi_target{};
    OptionalField<enums::PowerState> m_power_state{};
    PciDevices m_pci_devices{};
    UsbDevices m_usb_devices{};
    attribute::FruInfo m_fru_info{};
    OptionalField<std::string> m_sku{};
    OptionalField<std::string> m_asset_tag{};
    OptionalField<enums::IndicatorLed> m_indicator_led{};
    OptionalField<std::string> m_chassis{};
    attribute::ConnectionData m_connection_data{};
    OptionalField<std::string> m_guid{};
    CableIds m_cable_ids{};
    OptionalField<bool> m_txt_enabled{};
    OptionalField<bool> m_user_mode_enabled{};
    OptionalField <uint64_t> m_current_performance_configuration{};
    PerformanceConfigurations m_performance_configurations{};
    // internal flag only, this values is not exposed on REST API,
    // but is necessary in terms of communication with BMC;
    // will be set to true after first "set system mode" command
    bool m_rackscale_mode_enabled{false};

    /*!
     * @brief Power state command is being running
     *
     * Internal flag only, this values is not exposed on REST API,
     * but is necessary to block another tasks which modify power state of the system.
     */
    bool power_state_is_being_changed{false};

    /*!
     * @brief m_dcpmem_erase_task - internal flag.
     * If it's not empty, it points to a running task
     * whose purpose is to set the DCPMEM's state.
     * Used to prevent duplicate tasks for the same DCPMEM module.
     */
    OptionalField<Uuid> m_dcpmem_erase_task{};

    /*!
     * @brief m_performance_configuration_task - internal flag.
     * If it's not empty, it points to a running task
     * whose purpose is to set the processor perfomance configuration.
     * Used to prevent duplicate tasks for the same system.
     */
    bool m_is_performance_configuration_task_running{false};

    static const enums::CollectionName collection_name;
    static const enums::Component component;
};
}
}
