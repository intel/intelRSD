/*!
 * @brief Memory builder class implementation.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file memory_builder.cpp
 */

#include "agent-framework/module/utils/to_hex_string.hpp"
#include "discovery/builders/memory_builder.hpp"
#include "discovery/helpers/memory_helper.hpp"
#include "utils/conversion.hpp"
#include "utils/string_utils.hpp"
#include "utils/byte_swap.hpp"

using namespace agent::compute::discovery;
using namespace agent_framework::model;


agent_framework::model::Memory MemoryBuilder::build_default(const std::string& parent_uuid) {
    return Memory{parent_uuid};
}


void MemoryBuilder::update_smbios_data(agent_framework::model::Memory& memory,
                                       const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_MEMORY_DEVICE>& device,
                                       const std::vector<smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_PHYSICAL_MEMORY_ARRAY>>& arrays) {

    attribute::Status status{};
    attribute::FruInfo fru_info{};

    memory.set_smbios_handle(device.header.handle);
    memory.set_device_locator(device.get_string(device.data.device_locator));
    log_debug("smbios-discovery", "Discovering Memory (" << memory.get_device_locator() << ").");

    if (helpers::is_present(device.data)) {
        status.set_state(enums::State::Enabled);
        memory.add_memory_mode(enums::MemoryMode::Volatile);
        memory.set_name("DRAM");
        memory.set_description("DDR SDRAM");

        // Memory capacity in SMBIOS is actually provided with binary prefix - customary prefixes (kilo, mega and giga)
        // are often used interchangeably with binary prefixes (kibi, mibi and gibi) for memory.
        const auto capacity_mib = helpers::get_memory_size_mbytes(device.data);
        memory.set_capacity_mib(capacity_mib);

        fru_info.set_manufacturer(device.get_string(device.data.manufacturer));
        fru_info.set_serial_number(device.get_string(device.data.serial_number));
        fru_info.set_part_number(::utils::StringUtils::trim(device.get_string(device.data.part_number)));

        memory.set_module_type(helpers::get_base_module_type(device.data.form_factor));
        memory.set_device_type(helpers::get_device_type(device.data));
        memory.set_data_width_bits(device.data.data_width);
        memory.set_bus_width_bits(device.data.total_width);
        memory.set_rank_count(device.data.attributes & 0x0f);

        if (!helpers::is_non_volatile_memory(device.data)) {
            // Add region only for volatile memory - persistent memory regions info is initialized from ACPI table
            attribute::Region region{};
            region.set_region_id("0");
            region.set_memory_type(enums::MemoryClass::Volatile);
            region.set_offset_mib(0);
            region.set_size_mib(capacity_mib);
            memory.add_region(std::move(region));
        }

        const auto location = helpers::extract_memory_location(memory.get_device_locator());
        if (location.has_value()) {
            memory.set_location(location);
        }

        memory.set_error_correction(helpers::get_error_correction(arrays, device.data));

        memory.set_voltage_volt(device.data.voltage_configured / 1000.0);
        memory.set_min_voltage_volt(device.data.voltage_min / 1000.0);
        memory.set_max_voltage_volt(device.data.voltage_max / 1000.0);

        memory.set_operating_speed_mhz(device.data.configured_memory_clock_speed);
        memory.add_allowed_speed_mhz(device.data.speed);
        if (device.data.configured_memory_clock_speed != device.data.speed) {
            memory.add_allowed_speed_mhz(device.data.configured_memory_clock_speed);
        }
    }
    else {
        log_warning("smbios-discovery", "Memory (" << memory.get_device_locator() << ") is not present.");
        status.set_state(enums::State::Absent);
        status.set_health({}); // Set to null
    }

    memory.set_status(status);
    memory.set_fru_info(fru_info);

    /*
     * Set of properties which cannot be read from HW
     *
     * memory.set_memory_modes(TBD);
     * */
}


void MemoryBuilder::update_smbios_memory_device_extended_data(agent_framework::model::Memory& memory,
                                                              const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_MEMORY_DEVICE_EXTENDED_INFO_DATA>& smbios_data) {

    const auto& fw_api_version = smbios_data.get_string(smbios_data.data.firmware_api_version);
    if (!fw_api_version.empty()) {
        memory.set_firmware_api_version(helpers::extract_fw_number(fw_api_version));
    }

    const auto& fw_revision = smbios_data.get_string(smbios_data.data.firmware_revision);
    if (!fw_revision.empty()) {
        memory.set_firmware_revision(helpers::extract_fw_number(fw_revision));
    }

    memory.add_max_tdp_milliwatts(smbios_data.data.max_tdp);

    attribute::PowerManagementPolicy pm_policy{};
    pm_policy.set_max_tdp_milliwatts(smbios_data.data.max_tdp);
    memory.set_power_management_policy(pm_policy);

    memory.set_memory_type(helpers::get_memory_type(smbios_data.data.type));

    const auto media = helpers::get_media(smbios_data.data.media);
    if (media.has_value()) {
        memory.add_media(media);
    }
}

void MemoryBuilder::update_general_dcpmem_data(agent_framework::model::Memory& memory) {
    memory.set_name("DCPMM");
    memory.set_description("Intel Optane DC Persistent Memory Module");
    memory.set_memory_type(enums::MemoryType::IntelOptane);
    memory.add_memory_mode(enums::MemoryMode::PMEM);
}

using NvdimmControlRegion = acpi::parser::AcpiParser::StructEnhanced<acpi::structs::NFIT_NVDIMM_CONTROL_REGION_STRUCTURE>;
using NvdimmRegionMapping = acpi::parser::AcpiParser::StructEnhanced<acpi::structs::NFIT_NVDIMM_REGION_MAPPING_STRUCTURE>;
using NvdimmSpaRange      = acpi::parser::AcpiParser::StructEnhanced<acpi::structs::NFIT_SPA_RANGE_STRUCTURE>;
void MemoryBuilder::update_acpi_nfit_data(agent_framework::model::Memory& memory,
                                          const OptionalField<NvdimmControlRegion>& nfit_control_region,
                                          const OptionalField<std::vector<NvdimmRegionMapping>>& nfit_region_mappings,
                                          const OptionalField<std::vector<NvdimmSpaRange>>& nfit_spa_ranges) {

    if (nfit_control_region.has_value()) {
        const auto& nfit_control_region_data = nfit_control_region.value().data;
        memory.set_module_manufacturer_id(agent_framework::model::utils::to_hex_string<2>(::utils::byte_swap_16(nfit_control_region_data.VendorID)));
        memory.set_module_product_id(agent_framework::model::utils::to_hex_string<2>(::utils::byte_swap_16(nfit_control_region_data.DeviceID)));
        memory.set_memory_subsystem_controller_manufacturer_id(agent_framework::model::utils::to_hex_string<2>(::utils::byte_swap_16(nfit_control_region_data.SubsystemVendorID)));
        memory.set_memory_subsystem_controller_product_id(agent_framework::model::utils::to_hex_string<2>(nfit_control_region_data.SubsystemDeviceID));
    }

    if (nfit_region_mappings.has_value()) {
        attribute::Array<attribute::Region> regions{};
        attribute::Region region{};

        for (auto& nfit_region_mapping : nfit_region_mappings.value()) {
            const auto& nfit_region_mapping_data = nfit_region_mapping.data;

            region.set_region_id(std::to_string((int) nfit_region_mapping_data.NvdimmRegionID));

            region.set_offset_mib(::utils::convert_byte_units((double) nfit_region_mapping_data.RegionOffset,
                                                              ::utils::BytePrefix::Noop, ::utils::BytePrefix::Mibi));
            region.set_size_mib(::utils::convert_byte_units((double) nfit_region_mapping_data.NvdimmRegionSize,
                                                            ::utils::BytePrefix::Noop, ::utils::BytePrefix::Mibi));

            OptionalField<enums::MemoryClass> memory_class{};

            if (nfit_spa_ranges.has_value()) {
                const auto& nfit_spa_range = helpers::get_spa_range_structure(nfit_region_mapping, nfit_spa_ranges.value());
                memory_class = helpers::get_memory_class_of_spa_range(nfit_spa_range);
                region.set_memory_type(memory_class);

                if (memory_class == enums::MemoryClass::Volatile) {
                    memory.set_volatile_size_mib(::utils::convert_byte_units((double) nfit_region_mapping_data.NvdimmRegionSize,
                                                                                 ::utils::BytePrefix::Noop, ::utils::BytePrefix::Mibi));
                }
                else if (memory_class == enums::MemoryClass::ByteAccessiblePersistent) {
                    memory.set_non_volatile_size_mib(::utils::convert_byte_units((double) nfit_region_mapping_data.NvdimmRegionSize,
                                                                                 ::utils::BytePrefix::Noop, ::utils::BytePrefix::Mibi));
                }
            }

            const auto logical_size_mib = memory.get_volatile_size_mib() + memory.get_non_volatile_size_mib();
            memory.set_logical_size_mib(logical_size_mib);

            if (memory_class.has_value()) {
                regions.add_entry(region);
                memory.set_regions(regions);
            }
        }
    }
}
