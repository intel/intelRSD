/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * */

#include "agent-framework/module/model/memory.hpp"
#include "agent-framework/module/constants/compute.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Memory::component = enums::Component::Memory;
const enums::CollectionName Memory::collection_name = enums::CollectionName::Memory;

Memory::Memory(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

Memory::~Memory() {}

json::Json Memory::to_json() const {
    json::Json result{};
    result[literals::Memory::STATUS] = get_status().to_json();
    result[literals::Memory::MEMORY_TYPE] = get_memory_type();
    result[literals::Memory::DEVICE_TYPE] = get_device_type();
    result[literals::Memory::MODULE_TYPE] = get_module_type();
    result[literals::Memory::MEDIA] = get_memory_media().to_json();
    result[literals::Memory::OPERATING_MEMORY_MODES] = get_memory_modes().to_json();
    result[literals::Memory::CAPACITY_MB] = get_capacity_mb();
    result[literals::Memory::DATA_WIDTH_BITS] = get_data_width_bits();
    result[literals::Memory::BUS_WIDTH_BITS] = get_bus_width_bits();
    result[literals::Memory::FRU_INFO] = get_fru_info().to_json();
    result[literals::Memory::FIRMWARE_REVISION] = get_firmware_revision();
    result[literals::Memory::FIRMWARE_API_VERSION] = get_firmware_api_version();
    result[literals::Memory::MEMORY_CLASSES] = get_memory_classes().to_json();
    result[literals::Memory::VENDOR_ID] = get_vendor_id();
    result[literals::Memory::DEVICE_ID] = get_device_id();
    result[literals::Memory::OPERATING_SPEED_MHZ] = get_operating_speed_mhz();
    result[literals::Memory::ALLOWED_SPEEDS_MHZ] = get_allowed_speeds_mhz().to_json();
    result[literals::Memory::VOLTAGE] = get_voltage_volt();
    result[literals::Memory::MIN_VOLTAGE] = get_min_voltage_volt();
    result[literals::Memory::MAX_VOLTAGE] = get_max_voltage_volt();
    result[literals::Memory::DEVICE_LOCATOR] = get_device_locator();
    result[literals::Memory::LOCATION] = get_location().to_json();
    result[literals::Memory::RANK_COUNT] = get_rank_count();
    result[literals::Memory::ERROR_CORRECTION] = get_error_correction();
    result[literals::Memory::REGIONS] = get_regions().to_json();
    result[literals::Memory::OEM] = get_oem().to_json();
    return result;
}

Memory Memory::from_json(const json::Json& json) {
    using namespace agent_framework::model::attribute;
    Memory memory{};
    memory.set_status(attribute::Status::from_json(json[literals::Memory::STATUS]));
    memory.set_memory_type(json[literals::Memory::MEMORY_TYPE]);
    memory.set_device_type(json[literals::Memory::DEVICE_TYPE]);
    memory.set_module_type(json[literals::Memory::MODULE_TYPE]);
    memory.set_media(attribute::Array<enums::Media>::from_json(json[literals::Memory::MEDIA]));
    memory.set_memory_modes(attribute::Array<enums::MemoryMode>::from_json(json[literals::Memory::OPERATING_MEMORY_MODES]));
    memory.set_capacity_mb(json[literals::Memory::CAPACITY_MB]);
    memory.set_data_width_bits(json[literals::Memory::DATA_WIDTH_BITS]);
    memory.set_bus_width_bits(json[literals::Memory::BUS_WIDTH_BITS]);
    memory.set_fru_info(FruInfo::from_json(json[literals::Memory::FRU_INFO]));
    memory.set_firmware_revision(json[literals::Memory::FIRMWARE_REVISION]);
    memory.set_firmware_api_version(json[literals::Memory::FIRMWARE_API_VERSION]);
    memory.set_memory_classes(
        attribute::Array<enums::MemoryClass>::from_json(json[literals::Memory::MEMORY_CLASSES]));
    memory.set_vendor_id(json[literals::Memory::VENDOR_ID]);
    memory.set_device_id(json[literals::Memory::DEVICE_ID]);
    memory.set_operating_speed_mhz(json[literals::Memory::OPERATING_SPEED_MHZ]);
    memory.set_allowed_speeds_mhz(
        attribute::Array<std::uint32_t>::from_json(json[literals::Memory::ALLOWED_SPEEDS_MHZ]));
    memory.set_voltage_volt(json[literals::Memory::VOLTAGE]);
    memory.set_min_voltage_volt(json[literals::Memory::MIN_VOLTAGE]);
    memory.set_max_voltage_volt(json[literals::Memory::MAX_VOLTAGE]);
    memory.set_device_locator(json[literals::Memory::DEVICE_LOCATOR]);
    memory.set_location(attribute::MemoryLocation::from_json(json[literals::Memory::LOCATION]));
    memory.set_rank_count(json[literals::Memory::RANK_COUNT]);
    memory.set_error_correction(json[literals::Memory::ERROR_CORRECTION]);
    memory.set_regions(attribute::Array<attribute::Region>::from_json(json[literals::Memory::REGIONS]));
    memory.set_oem(Oem::from_json(json[literals::Memory::OEM]));

    memory.set_resource_hash(json);
    return memory;
}
