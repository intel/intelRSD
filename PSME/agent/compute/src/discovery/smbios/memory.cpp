/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @file memory.cpp
 *
 * @brief SMBIOS-way of discovering Memory properties
 * */

#include "smbios/structs/memory.hpp"
#include "discovery/smbios/memory.hpp"
#include "agent-framework/module/model/memory.hpp"
#include "agent-framework/module/compute_components.hpp"

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace smbios::parser;

namespace {

bool is_present(const MEMORY_DEVICE& device) {
    return device.size > 0;
}

OptionalField<std::uint32_t> get_memory_size_mbytes(const MEMORY_DEVICE& device) {
    auto capacity = device.size;
    if (0xffff == capacity) {
        log_warning(GET_LOGGER("smbios-discovery"), "SMBIOS reports 'unknown' memory device size!");
        return {};
    }

    if (capacity & (1 << 15)) {
        // convert kilobytes to megabytes
        capacity = uint16_t(capacity / 1000);
    }
    return std::uint32_t(capacity);
}

DeviceType get_device_type(const MEMORY_DEVICE& device) {
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
            return DeviceType::Unknown;
    }
}

using MemoryArrays = const SmbiosParser::StructVec<SMBIOS_PHYSICAL_MEMORY_ARRAY>;
enums::ErrorCorrection get_error_correction(MemoryArrays& arrays, const MEMORY_DEVICE& device) {
    auto array_handle = device.physical_memory_array_handle;
    const auto& array =
            std::find_if(arrays.cbegin(), arrays.cend(),
                         [array_handle](const SmbiosParser::StructEnhanced<SMBIOS_PHYSICAL_MEMORY_ARRAY>& entry) {
                             return entry.header.handle == array_handle;
                         }
            );

    if (arrays.cend() == array) {
        return enums::ErrorCorrection::Unknown;
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
            return enums::ErrorCorrection::Unknown;
    }
}

}

namespace agent {
namespace compute {
namespace discovery {
namespace smbios {

std::tuple<bool, enums::Health> discover_memory_modules(::smbios::parser::SmbiosParser::Ptr smbios, System& system) {
    if (!smbios) {
        return std::make_tuple(false, enums::Health::OK);
    }
    auto arrays = smbios->get_all<SMBIOS_PHYSICAL_MEMORY_ARRAY>();
    auto devices = smbios->get_all<SMBIOS_MEMORY_DEVICE>();

    if (devices.empty()) {
        log_info(GET_LOGGER("smbios-discovery"), "No Memory Device found in SMBIOS");
        return std::make_tuple(false, enums::Health::OK);
    }

    for (auto& device : devices) {
        attribute::Status status{};
        attribute::FruInfo fru_info{};
        Memory memory{system.get_uuid()};

        memory.set_device_locator(device.get_string(device.data.device_locator));
        log_debug(GET_LOGGER("smbios-discovery"), "Discovering Memory (" << memory.get_device_locator() << ").");

        if (is_present(device.data)) {
            status.set_state(enums::State::Enabled);

            const auto capacity_mb = get_memory_size_mbytes(device.data);
            memory.set_capacity_mb(capacity_mb);

            fru_info.set_manufacturer(device.get_string(device.data.manufacturer));
            fru_info.set_serial_number(device.get_string(device.data.serial_number));
            fru_info.set_part_number(device.get_string(device.data.part_number));

            memory.set_device_type(get_device_type(device.data));
            memory.set_data_width_bits(device.data.data_width);
            memory.set_bus_width_bits(device.data.total_width);
            memory.set_rank_count(device.data.attributes & 0x0f);

            attribute::Region region{};
            region.set_region_id("0");
            region.set_offset_mb(0);
            region.set_size_mb(capacity_mb);
            memory.add_region(std::move(region));

            memory.set_error_correction(get_error_correction(arrays, device.data));

            memory.set_voltage_volt(device.data.voltage_configured / 1000.0);
            memory.set_min_voltage_volt(device.data.voltage_min / 1000.0);
            memory.set_max_voltage_volt(device.data.voltage_max / 1000.0);

            memory.set_operating_speed_mhz(device.data.configured_memory_clock_speed);
            memory.add_allowed_speed_mhz(device.data.speed);
            if (device.data.configured_memory_clock_speed != device.data.speed) {
                memory.add_allowed_speed_mhz(device.data.configured_memory_clock_speed);
            }

            memory.add_media(enums::Media::DRAM);
            memory.set_memory_type(enums::MemoryType::DRAM);
        }
        else {
            log_warning(GET_LOGGER("basic-discovery"), "Memory (" << memory.get_device_locator() << ") is not present.");
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
         * memory.set_firmware_revision(TBD);
         * memory.set_firmware_api_version(TBD);
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

        // Adding memory to model
        ComputeComponents::get_instance()->get_memory_manager().add_entry(std::move(memory));
    }

    log_info(GET_LOGGER("smbios-discovery"), "Memory Modules Discovery was successful.");
    return std::make_tuple(true, enums::Health::OK);
}

}
}
}
}
