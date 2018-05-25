/*!
 * @brief Grantley memory builder class implementation.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file grantley_memory_builder.hpp
 */

#include "discovery/builders/memory_builder.hpp"

#include <bitset>
#include <algorithm>



using namespace agent::compute::discovery;
using namespace smbios::parser;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;

namespace {

bool is_present(const MEMORY_DEVICE& device) {
    return device.size > 0;
}


OptionalField<std::uint32_t> get_memory_size_mbytes(const MEMORY_DEVICE& device) {
    auto capacity = device.size;
    if (0xffff == capacity) {
        log_warning("smbios-discovery", "SMBIOS reports 'unknown' memory device size!");
        return {};
    }

    if (capacity & (1 << 15)) {
        // convert kilobytes to megabytes
        capacity = uint16_t(capacity / 1000);
    }
    return std::uint32_t(capacity);
}


OptionalField<DeviceType> get_device_type(const MEMORY_DEVICE& device) {
    switch (device.memory_type) {
        case MemoryTypeEnum::DDR:
            return DeviceType::DDR;
        case MemoryTypeEnum::DDR2:
            return DeviceType::DDR2;
        case MemoryTypeEnum::DDR3:
            return DeviceType::DDR3;
        case MemoryTypeEnum::DDR4:
            return DeviceType::DDR4;
        case MemoryTypeEnum::ROM:
            return DeviceType::ROM;
        case MemoryTypeEnum::SDRAM:
            return DeviceType::SDRAM;

        case MemoryTypeEnum::DRAM:
        case MemoryTypeEnum::OTHER:
        case MemoryTypeEnum::UNKNOWN:
        case MemoryTypeEnum::EDRAM:
        case MemoryTypeEnum::VRAM:
        case MemoryTypeEnum::SRAM:
        case MemoryTypeEnum::RAM:
        case MemoryTypeEnum::FLASH:
        case MemoryTypeEnum::EEPROM:
        case MemoryTypeEnum::FEPROM:
        case MemoryTypeEnum::EPROM:
        case MemoryTypeEnum::CDRAM:
        case MemoryTypeEnum::RAM_3D:
        case MemoryTypeEnum::SGRAM:
        case MemoryTypeEnum::RDRAM:
        case MemoryTypeEnum::DDR2_FB_DIMM:
        case MemoryTypeEnum::FBD2:
        case MemoryTypeEnum::LPDDR:
        case MemoryTypeEnum::LPDDR2:
        case MemoryTypeEnum::LPDDR3:
        case MemoryTypeEnum::LPDDR4:
        default:
            return OptionalField<DeviceType>{};
    }
}


OptionalField<Media> get_media(const smbios::parser::MemoryDeviceExtendedMedia& media) {
    using namespace smbios::parser;
    switch (media) {
        case MemoryDeviceExtendedMedia::DRAM:
            return Media::DRAM;
        case MemoryDeviceExtendedMedia::NAND:
            return Media::NAND;
        case MemoryDeviceExtendedMedia::PROPRIETARY:
            return Media::Proprietary;
        default:
            return {};
    }
}

OptionalField<MemoryType> get_memory_type(const smbios::parser::MemoryDeviceExtendedType& type) {
    using namespace smbios::parser;
    switch (type) {
        case MemoryDeviceExtendedType::DIMM:
            return MemoryType::DRAM;
        case MemoryDeviceExtendedType::NVDIMM_F:
            return MemoryType::NVDIMM_F;
        case MemoryDeviceExtendedType::NVDIMM_N:
            return MemoryType::NVDIMM_N;
        case MemoryDeviceExtendedType::NVDIMM_P:
            return MemoryType::NVDIMM_P;
        default:
            return {};
    }
}


using MemoryArrays = const SmbiosParser::StructVec<SMBIOS_PHYSICAL_MEMORY_ARRAY>;


OptionalField<enums::ErrorCorrection> get_error_correction(MemoryArrays& arrays, const MEMORY_DEVICE& device) {
    auto array_handle = device.physical_memory_array_handle;
    const auto& array =
        std::find_if(arrays.cbegin(), arrays.cend(),
                     [array_handle](const SmbiosParser::StructEnhanced<SMBIOS_PHYSICAL_MEMORY_ARRAY>& entry) {
                         return entry.header.handle == array_handle;
                     }
        );

    if (arrays.cend() == array) {
        return OptionalField<enums::ErrorCorrection>{};
    }

    switch (array->data.memory_error_correction) {
        case MemoryErrorCorrectionEnum::NONE:
            return enums::ErrorCorrection::NoECC;
        case MemoryErrorCorrectionEnum::PARITY:
            return enums::ErrorCorrection::AddressParity;
        case MemoryErrorCorrectionEnum::SINGLE_BIT_ECC:
            return enums::ErrorCorrection::SingleBitECC;
        case MemoryErrorCorrectionEnum::MULTI_BIT_ECC:
            return enums::ErrorCorrection::MultiBitECC;
        case MemoryErrorCorrectionEnum::CRC:
        case MemoryErrorCorrectionEnum::OTHER:
        case MemoryErrorCorrectionEnum::UNKNOWN:
        default:
            return OptionalField<enums::ErrorCorrection>{};
    }
}

}


agent_framework::model::Memory MemoryBuilder::build_default(const std::string& parent_uuid) {
    return Memory{parent_uuid};
}


void MemoryBuilder::update_smbios_data(agent_framework::model::Memory& memory,
                                       const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_MEMORY_DEVICE>& device,
                                       const std::vector<smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_PHYSICAL_MEMORY_ARRAY>>& arrays) {

    attribute::Status status{};
    attribute::FruInfo fru_info{};

    memory.set_device_locator(device.get_string(device.data.device_locator));
    log_debug("smbios-discovery", "Discovering Memory (" << memory.get_device_locator() << ").");

    if (is_present(device.data)) {
        status.set_state(enums::State::Enabled);

        const auto capacity_mb = get_memory_size_mbytes(device.data);
        memory.set_capacity_mb(capacity_mb);

        fru_info.set_manufacturer(device.get_string(device.data.manufacturer));
        fru_info.set_serial_number(device.get_string(device.data.serial_number));
        fru_info.set_part_number(device.get_string(device.data.part_number));

        memory.set_device_type(::get_device_type(device.data));
        memory.set_data_width_bits(device.data.data_width);
        memory.set_bus_width_bits(device.data.total_width);
        memory.set_rank_count(device.data.attributes & 0x0f);

        attribute::Region region{};
        region.set_region_id("0");
        region.set_offset_mb(0);
        region.set_size_mb(capacity_mb);
        memory.add_region(std::move(region));

        memory.set_error_correction(::get_error_correction(arrays, device.data));

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
     * memory.set_module_type(TBD);
     * memory.set_memory_modes(TBD);
     * memory.set_device_id(TBD);
     * memory.set_memory_classes(TBD);
     * */

    /*
     * In theory 'bank_locator' contains information that we need here.
     * Unfortunately, this field contains string, which does not conform to any known format.
     * This means that we cannot use it to fill in socket, controller, channel, slot
     *
     * memory.set_dimm_location(TBD);
     * */
}


void MemoryBuilder::update_smbios_memory_device_extended_data(agent_framework::model::Memory& memory,
                                                              const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_MEMORY_DEVICE_EXTENDED_INFO_DATA>& smbios_data) {
    memory.set_firmware_api_version(smbios_data.get_string(smbios_data.data.firmware_api_version));
    memory.set_firmware_revision(smbios_data.get_string(smbios_data.data.firmware_revision));
    memory.set_max_tdp_milliwats(smbios_data.data.max_tdp);
    memory.set_memory_type(::get_memory_type(smbios_data.data.type));

    const auto media = ::get_media(smbios_data.data.media);
    if (media.has_value()) {
        memory.add_media(media);
    }
}
