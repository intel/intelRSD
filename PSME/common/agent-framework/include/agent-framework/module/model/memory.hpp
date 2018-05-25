/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * @file memory.hpp
 * @brief Memory model interface
 * */
#pragma once
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/compute.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! Memory */
class Memory : public Resource {
public:
    explicit Memory(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::System);
    ~Memory();

    Memory(const Memory&) = default;
    Memory& operator=(const Memory&) = default;
    Memory(Memory&&) = default;
    Memory& operator=(Memory&&) = default;

    /*!
     * @brief construct an object of class Memory from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed Memory object
     */
    static Memory from_json(const json::Json& json);

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
        return Memory::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return Memory::component;
    }

    /*!
     * @brief Set memory type
     * @param[in] memory_type memory type
     * */
    void set_memory_type(const OptionalField<enums::MemoryType>& memory_type) {
        m_memory_type = memory_type;
    }

    /*!
     * @brief Get memory type
     * @return memory type
     * */
    const OptionalField<enums::MemoryType>& get_memory_type() const {
        return m_memory_type;
    }

    /*!
     * @brief Set device type
     * @param[in] device_type device type
     * */
    void set_device_type(const OptionalField<enums::DeviceType>& device_type) {
        m_device_type = device_type;
    }

    /*!
     * @brief Get device type
     * @return device type
     * */
    const OptionalField<enums::DeviceType>& get_device_type() const {
        return m_device_type;
    }

    /*!
     * @brief Set module type
     * @param[in] module_type module type
     * */
    void set_module_type(const OptionalField<enums::MemoryModuleType>& module_type) {
        m_module_type = module_type;
    }

    /*!
     * @brief Get module type
     * @return module type
     * */
    const OptionalField<enums::MemoryModuleType>& get_module_type() const {
        return m_module_type;
    }

    /*!
     * @brief Add media
     * @param[in] media media
     * */
    void add_media(const enums::Media& media) {
        m_media.add_entry(media);
    }

    /*!
     * @brief Set media
     * @param[in] media  media array
     * */
    void set_media(const attribute::Array<enums::Media>& media) {
        m_media = media;
    }

    /*!
     * @brief Get memory media
     * @return memory media array
     * */
    const attribute::Array<enums::Media>& get_memory_media() const {
        return m_media;
    }

    /*!
     * @brief Add memory mode
     * @param[in] memory_mode memory mode
     * */
    void add_memory_mode(const enums::MemoryMode& memory_mode) {
        m_memory_modes.add_entry(memory_mode);
    }

    /*!
     * @brief Set memory modes
     * @param[in] memory_modes memory modes array
     * */
    void set_memory_modes(const attribute::Array<enums::MemoryMode>& memory_modes) {
        m_memory_modes = memory_modes;
    }

    /*!
     * @brief Get memory modes
     * @return memory modes array
     * */
    const attribute::Array<enums::MemoryMode>& get_memory_modes() const {
        return m_memory_modes;
    }

    /*!
     * @brief Set capacity in MB
     * @param[in] capacity Memory capacity
     * */
    void set_capacity_mb(const OptionalField<std::uint32_t>& capacity) {
        m_capacity_mb = capacity;
    }

    /*!
     * @brief Get capacity in MB
     * @return memory capacity
     * */
    OptionalField<std::uint32_t> get_capacity_mb() const {
        return m_capacity_mb;
    }

    /*!
     * @brief Set data width in bits
     * @param[in] data_width_bits Data width in bits
     * */
    void set_data_width_bits(const OptionalField<std::uint32_t>& data_width_bits) {
        m_data_width_bits = data_width_bits;
    }

    /*!
     * @brief Get data width bits
     * @return data width bits
     * */
    const OptionalField<std::uint32_t>& get_data_width_bits() const {
        return m_data_width_bits;
    }
    /*!
     * @brief Set bus width in bits
     * @param[in] bus_width_bits Total width in bits
     * */
    void set_bus_width_bits(const OptionalField<std::uint32_t>& bus_width_bits) {
        m_bus_width_bits = bus_width_bits;
    }

    /*!
     * @brief Get bus width bits
     * @return bus width bits
     * */
    const OptionalField<std::uint32_t>& get_bus_width_bits() const {
        return m_bus_width_bits;
    }

    /*!
     * @brief Set FruInfo for response
     * @param[in] fru_info FruInfo
     * */
    void set_fru_info(const attribute::FruInfo& fru_info) {
        m_fru_info = fru_info;
    }

    /*!
     * @brief Get FruInfo
     * @return FRU info
     * */
    const attribute::FruInfo& get_fru_info() const {
        return m_fru_info;
    }

    /*!
     * @brief Set firmware revision
     * @param[in] firmware_revision firmware revision
     * */
    void set_firmware_revision(const OptionalField<std::string>& firmware_revision) {
        m_firmware_revision = firmware_revision;
    }

    /*!
     * @brief Get firmware revision
     * @return firmware revision
     * */
    const OptionalField<std::string>& get_firmware_revision() const {
        return m_firmware_revision;
    }

    /*!
     * @brief Set firmware API version
     * @param[in] firmware_api_version firmware version
     * */
    void set_firmware_api_version(const OptionalField<std::string>& firmware_api_version) {
        m_firmware_api_version = firmware_api_version;
    }

    /*!
     * @brief Get firmware API version
     * @return firmware API version
     * */
    const OptionalField<std::string>& get_firmware_api_version() const {
        return m_firmware_api_version;
    }

    /*!
     * @brief Add memory class
     * @param[in] memory_class memory class
     * */
    void add_memory_class(const enums::MemoryClass& memory_class) {
        m_memory_classes.add_entry(memory_class);
    }

    /*!
     * @brief Set memory classes
     * @param[in] memory_classes memory classes array
     * */
    void set_memory_classes(const attribute::Array<enums::MemoryClass>& memory_classes) {
        m_memory_classes = memory_classes;
    }

    /*!
     * @brief Get memory classes
     * @return memory classes array
     * */
    const attribute::Array<enums::MemoryClass>& get_memory_classes() const {
        return m_memory_classes;
    }

    /*!
     * @brief Set vendor ID
     * @param[in] vendor_id vendor ID
     * */
    void set_vendor_id(const OptionalField<std::string>&
        vendor_id) {
        m_vendor_id= vendor_id;
    }

    /*!
     * @brief Get vendor ID
     * @return vendor ID
     * */
    const OptionalField<std::string>& get_vendor_id() const {
        return m_vendor_id;
    }

    /*!
     * @brief Set device ID
     * @param[in] device_id device ID
     * */
    void set_device_id(const OptionalField<std::string>&
        device_id) {
        m_device_id= device_id;
    }

    /*!
     * @brief Get device ID
     * @return device ID
     * */
    const OptionalField<std::string>& get_device_id() const {
        return m_device_id;
    }

    /*!
     * @brief Set memory operating speed in Mhz
     * @param[in] speed memory operating speed
     * */
    void set_operating_speed_mhz(const OptionalField<std::uint32_t>& speed) {
        m_operating_speed_mhz = speed;
    }

    /*!
     * @brief Get operating speed in MHz
     * @return memory operating speed in MHz
     * */
    const OptionalField<std::uint32_t>& get_operating_speed_mhz() const {
        return m_operating_speed_mhz;
    }

    /*!
     * @brief Add memory allowed speed in Mhz
     * @param[in] speed memory allowed speed
     * */
    void add_allowed_speed_mhz(const std::uint32_t speed) {
        m_allowed_speeds_mhz.add_entry(speed);
    }

    /*!
     * @brief Set memory allowed speeds in Mhz
     * @param[in] speeds memory allowed speeds in MHz array
     * */
    void set_allowed_speeds_mhz(const attribute::Array<std::uint32_t>& speeds) {
        m_allowed_speeds_mhz = speeds;
    }

    /*!
     * @brief Get allowed speeds in MHz
     * @return allowed speeds in MHz array
     * */
    const attribute::Array<std::uint32_t>& get_allowed_speeds_mhz() const {
        return m_allowed_speeds_mhz;
    }

    /*!
     * @brief Set memory voltage
     * @param[in] voltage_volt Memory voltage
     * */
    void set_voltage_volt(const OptionalField<double>& voltage_volt) {
        m_voltage_volt = voltage_volt;
    }

    /*!
     * @brief Get voltage Volt
     * @return voltage in Volts
     * */
    const OptionalField<double>& get_voltage_volt() const {
        return m_voltage_volt;
    }

    /*!
     * @brief Set minimum memory voltage
     * @param min_voltage_volt Minimum memory voltage
     * */
    void set_min_voltage_volt(const OptionalField<double>&
        min_voltage_volt) {
        m_min_voltage_volt = min_voltage_volt;
    }

    /*!
     * @brief Get minimum voltage Volt
     * @return Minimum voltage in Volts
     * */
    const OptionalField<double>& get_min_voltage_volt() const {
        return m_min_voltage_volt;
    }

    /*!
     * @brief Set maximum memory voltage
     * @param max_voltage_volt Maximum memory voltage
     * */
    void set_max_voltage_volt(const OptionalField<double>&
        max_voltage_volt) {
        m_max_voltage_volt = max_voltage_volt;
    }

    /*!
     * @brief Get maximum voltage Volt
     * @return Maximum voltage in Volts
     * */
    const OptionalField<double>& get_max_voltage_volt() const {
        return m_max_voltage_volt;
    }

    /*!
     * @brief Set device locator
     * @param[in] device_locator device locator (DIMM, SODIMM, ...)
     * */
    void set_device_locator(const OptionalField<std::string>& device_locator) {
        m_device_locator = device_locator;
    }

    /*!
     * @brief Get device locator
     * @return device locator
     * */
   const OptionalField<std::string>& get_device_locator() const {
        return m_device_locator;
    }

    /*!
     * @brief Set memory location
     * @param[in] location memory location
     * */
    void set_location(const attribute::MemoryLocation& location) {
        m_location = location;
    }

    /*!
     * @brief Get memory location
     * @return memory location
     * */
    const attribute::MemoryLocation& get_location() const {
        return m_location;
    }

    /*!
     * @brief Set rank count
     * @param[in] rank_count memory rank count
     * */
    void set_rank_count(const OptionalField<std::uint32_t>& rank_count) {
        m_rank_count = rank_count;
    }

    /*!
     * @brief Get rank count
     * @return memory rank count
     * */
    const OptionalField<std::uint32_t>& get_rank_count() const {
        return m_rank_count;
    }

    /*!
     * @brief Set error correction
     * @param[in] error_correction memory error correction
     * */
    void set_error_correction(const OptionalField<enums::ErrorCorrection>&
        error_correction) {
        m_error_correction = error_correction;
    }

    /*!
     * @brief Get error correction
     * @return memory error correction
     * */
    const OptionalField<enums::ErrorCorrection>& get_error_correction() const {
        return m_error_correction;
    }

    /*!
     * @brief Add region
     * @param[in] region memory region
     * */
    void add_region(const attribute::Region& region) {
        m_regions.add_entry(region);
    }

    /*!
     * @brief Set regions
     * @param[in] regions memory regions
     * */
    void set_regions(const attribute::Array<attribute::Region>& regions) {
        m_regions = regions;
    }

    /*!
     * @brief Get memory regions
     * @return memory regions
     * */
    const attribute::Array<attribute::Region>& get_regions() const {
        return m_regions;
    }

    /*!
     * @brief Set smbios handle
     * @param[in] smbios_handle smbios handle
     * */
    void set_smbios_handle(const OptionalField<std::uint16_t>& smbios_handle) {
        m_smbios_handle = smbios_handle;
    }

    /*!
     * @brief Get smbios handle
     * @return smbios handle
     * */
    const OptionalField<std::uint16_t>& get_smbios_handle() const {
        return m_smbios_handle;
    }

    /*!
     * @brief Set max tdp milliwats
     * @param[in] max_tdp_milliwats max tdp milliwats
     * */
    void set_max_tdp_milliwats(const OptionalField<std::uint32_t>& max_tdp_milliwats) {
        m_max_tdp_milliwats = max_tdp_milliwats;
    }

    /*!
     * @brief Get max tdp milliwats
     * @return max tdp milliwats
     * */
    const OptionalField<std::uint32_t>& get_max_tdp_milliwats() const {
        return m_max_tdp_milliwats;
    }


private:
    OptionalField<enums::MemoryType> m_memory_type{};
    OptionalField<enums::DeviceType> m_device_type{};
    OptionalField<enums::MemoryModuleType> m_module_type{};
    attribute::Array<enums::Media> m_media{};
    attribute::Array<enums::MemoryMode> m_memory_modes{};
    OptionalField<std::uint32_t> m_capacity_mb{};
    OptionalField<std::uint32_t> m_data_width_bits{};
    OptionalField<std::uint32_t> m_bus_width_bits{};
    attribute::FruInfo m_fru_info{};
    OptionalField<std::string> m_firmware_revision{};
    OptionalField<std::string> m_firmware_api_version{};
    attribute::Array<enums::MemoryClass> m_memory_classes{};
    OptionalField<std::string> m_vendor_id{};
    OptionalField<std::string> m_device_id{};
    OptionalField<std::uint32_t> m_operating_speed_mhz{};
    attribute::Array<std::uint32_t> m_allowed_speeds_mhz{};
    OptionalField<std::uint32_t> m_max_tdp_milliwats{};
    OptionalField<double> m_voltage_volt{};
    OptionalField<double> m_min_voltage_volt{};
    OptionalField<double> m_max_voltage_volt{};
    OptionalField<std::string> m_device_locator{};
    attribute::MemoryLocation m_location{};
    OptionalField<std::uint32_t> m_rank_count{};
    OptionalField<enums::ErrorCorrection> m_error_correction{};
    attribute::Array<attribute::Region> m_regions{};

    // needed for matching with smbios device extended structs
    OptionalField<std::uint16_t> m_smbios_handle{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}

