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
 * @file dimm.hpp
 * @brief Dimm model interface
 * */
#ifndef AGENT_FRAMEWORK_MODULE_MODEL_DIMM_HPP
#define AGENT_FRAMEWORK_MODULE_MODEL_DIMM_HPP

#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"
#include "agent-framework/module-ref/model/resource.hpp"
#include "agent-framework/module-ref/enum/compute.hpp"
#include "agent-framework/module-ref/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! Dimm */
class Dimm : public Resource {
public:
    explicit Dimm(const std::string& parent_uuid = {});
    ~Dimm();

    Dimm(const Dimm&) = default;
    Dimm& operator=(const Dimm&) = default;
    Dimm(Dimm&&) = default;
    Dimm& operator=(Dimm&&) = default;

    /*!
     * @brief construct an object of class Dimm from JSON
     *
     * @param json the Json::Value deserialized to object
     *
     * @return the newly constructed Dimm object
     */
    static Dimm from_json(const Json::Value& json);

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
        return Dimm::collection_name;
    }

    /**
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return Dimm::component;
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
     * @brief Set dimm type
     * @param[in] dimm_type dimm type
     * */
    void set_dimm_type(const OptionalField<enums::DimmType>& dimm_type) {
        m_dimm_type = dimm_type;
    }

    /*!
     * @brief Get dimm type
     * @return dimm type
     * */
    const OptionalField<enums::DimmType>& get_dimm_type() const {
        return m_dimm_type;
    }

    /*!
     * @brief Set module type
     * @param[in] module_type module type
     * */
    void set_module_type(const OptionalField<enums::DimmModuleType>& module_type) {
        m_module_type = module_type;
    }

    /*!
     * @brief Get module type
     * @return module type
     * */
    const OptionalField<enums::DimmModuleType>& get_module_type() const {
        return m_module_type;
    }

    /*!
     * @brief Add dimm media
     * @param[in] dimm_media dimm media
     * */
    void add_dimm_media(const enums::DimmMedia& dimm_media) {
        m_dimm_media.add_entry(dimm_media);
    }

    /*!
     * @brief Set dimm media
     * @param[in] dimm_media dimm media array
     * */
    void set_dimm_media(const attribute::Array<enums::DimmMedia>& dimm_media) {
        m_dimm_media = dimm_media;
    }

    /*!
     * @brief Get dimm media
     * @return dimm media array
     * */
    const attribute::Array<enums::DimmMedia>& get_dimm_media() const {
        return m_dimm_media;
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
     * @param[in] capacity Dimm capacity
     * */
    void set_capacity_mb(const OptionalField<double>& capacity) {
        m_capacity_mb = capacity;
    }

    /*!
     * @brief Get capacity in MB
     * @return dimm capacity
     * */
    OptionalField<double> get_capacity_mb() const {
        return m_capacity_mb;
    }

    /*!
     * @brief Set data width in bits
     * @param[in] data_width_bits Data width in bits
     * */
    void set_data_width_bits(const OptionalField<std::uint32_t>&
        data_width_bits) {
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
    void set_bus_width_bits(const OptionalField<std::uint32_t>&
        bus_width_bits) {
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
    void set_firmware_revision(const OptionalField<std::string>&
        firmware_revision) {
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
    void set_firmware_api_version(const OptionalField<std::string>&
        firmware_api_version) {
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
    void set_memory_classes(const attribute::Array<enums::MemoryClass>&
        memory_classes) {
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
     * @brief Set dimm operating speed in Mhz
     * @param[in] speed dimm operating speed
     * */
    void set_operating_speed_mhz(const OptionalField<std::uint32_t>& speed) {
        m_operating_speed_mhz = speed;
    }

    /*!
     * @brief Get operating speed in MHz
     * @return dimm operating speed in MHz
     * */
    const OptionalField<std::uint32_t>& get_operating_speed_mhz() const {
        return m_operating_speed_mhz;
    }

    /*!
     * @brief Add dimm allowed speed in Mhz
     * @param[in] speed dimm allowed speed
     * */
    void add_allowed_speed_mhz(const std::uint32_t speed) {
        m_allowed_speeds_mhz.add_entry(speed);
    }

    /*!
     * @brief Set dimm allowed speeds in Mhz
     * @param[in] speeds dimm allowed speeds in MHz array
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
     * @brief Set dimm voltage
     * @param[in] voltage_volt Dimm voltage
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
     * @brief Set minimum dimm voltage
     * @param min_voltage_volt Minimum dimm voltage
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
     * @brief Set maximum dimm voltage
     * @param max_voltage_volt Maximum dimm voltage
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
     * @brief Set dimm location
     * @param[in] dimm_location dimm location
     * */
    void set_dimm_location(const attribute::DimmLocation& dimm_location) {
        m_dimm_location = dimm_location;
    }

    /*!
     * @brief Get dimm location
     * @return dimm location
     * */
    const attribute::DimmLocation& get_dimm_location() const {
        return m_dimm_location;
    }

    /*!
     * @brief Set rank count
     * @param[in] rank_count dimm rank count
     * */
    void set_rank_count(const OptionalField<std::uint32_t>& rank_count) {
        m_rank_count = rank_count;
    }

    /*!
     * @brief Get rank count
     * @return dimm rank count
     * */
    const OptionalField<std::uint32_t>& get_rank_count() const {
        return m_rank_count;
    }

    /*!
     * @brief Set error correction
     * @param[in] error_correction dimm error correction
     * */
    void set_error_correction(const OptionalField<enums::ErrorCorrection>&
        error_correction) {
        m_error_correction = error_correction;
    }

    /*!
     * @brief Get error correction
     * @return dimm error correction
     * */
    const OptionalField<enums::ErrorCorrection>& get_error_correction() const {
        return m_error_correction;
    }

    /*!
     * @brief Add region
     * @param[in] region dimm region
     * */
    void add_region(const attribute::Region& region) {
        m_regions.add_entry(region);
    }

    /*!
     * @brief Set regions
     * @param[in] regions dimm regions
     * */
    void set_regions(const attribute::Array<attribute::Region>& regions) {
        m_regions = regions;
    }

    /*!
     * @brief Get dimm regions
     * @return dimm regions
     * */
    const attribute::Array<attribute::Region>& get_regions() const {
        return m_regions;
    }

private:
    OptionalField<enums::MemoryType> m_memory_type{enums::MemoryType::DRAM};
    OptionalField<enums::DimmType> m_dimm_type{enums::DimmType::DDR};
    OptionalField<enums::DimmModuleType> m_module_type{enums::DimmModuleType::RDIMM};
    attribute::Array<enums::DimmMedia> m_dimm_media{};
    attribute::Array<enums::MemoryMode> m_memory_modes{};
    OptionalField<double> m_capacity_mb{};
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
    OptionalField<double> m_voltage_volt{};
    OptionalField<double> m_min_voltage_volt{};
    OptionalField<double> m_max_voltage_volt{};
    OptionalField<std::string> m_device_locator{};
    attribute::DimmLocation m_dimm_location{};
    OptionalField<std::uint32_t> m_rank_count{};
    OptionalField<enums::ErrorCorrection> m_error_correction{};
    attribute::Array<attribute::Region> m_regions{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_MODEL_DIMM_HPP */
