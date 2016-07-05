/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * */

#include "agent-framework/module-ref/model/dimm.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"
#include "agent-framework/module-ref/enum/common.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Dimm::component = enums::Component::Dimm;
const enums::CollectionName Dimm::collection_name =
    enums::CollectionName::Memories;

Dimm::Dimm(const std::string& parent_uuid) : Resource{parent_uuid,
    enums::Component::System} {}

Dimm::~Dimm() {}

Json::Value Dimm::to_json() const {
    Json::Value result;
    result[literals::Dimm::STATUS] = get_status().to_json();
    result[literals::Dimm::MEMORY_TYPE] = get_memory_type();
    result[literals::Dimm::DIMM_TYPE] = get_dimm_type();
    result[literals::Dimm::MODULE_TYPE] = get_module_type();
    result[literals::Dimm::DIMM_MEDIA] = get_dimm_media().to_json();
    result[literals::Dimm::MEMORY_MODES] = get_memory_modes().to_json();
    result[literals::Dimm::CAPACITY_MB] = get_capacity_mb();
    result[literals::Dimm::DATA_WIDTH_BITS] = get_data_width_bits();
    result[literals::Dimm::BUS_WIDTH_BITS] = get_bus_width_bits();
    result[literals::Dimm::FRU_INFO] = get_fru_info().to_json();
    result[literals::Dimm::FIRMWARE_REVISION] = get_firmware_revision();
    result[literals::Dimm::FIRMWARE_API_VERSION] =
                                                get_firmware_api_version();
    result[literals::Dimm::MEMORY_CLASSES] =
                                            get_memory_classes().to_json();
    result[literals::Dimm::VENDOR_ID] = get_vendor_id();
    result[literals::Dimm::DEVICE_ID] = get_device_id();
    result[literals::Dimm::OPERATING_SPEED_MHZ] = get_operating_speed_mhz();
    result[literals::Dimm::ALLOWED_SPEEDS_MHZ] = get_allowed_speeds_mhz().to_json();
    result[literals::Dimm::VOLTAGE] = get_voltage_volt();
    result[literals::Dimm::MIN_VOLTAGE] = get_min_voltage_volt();
    result[literals::Dimm::MAX_VOLTAGE] = get_max_voltage_volt();
    result[literals::Dimm::DEVICE_LOCATOR] = get_device_locator();
    result[literals::Dimm::DIMM_LOCATION] = get_dimm_location().to_json();
    result[literals::Dimm::RANK_COUNT] = get_rank_count();
    result[literals::Dimm::ERROR_CORRECTION] = get_error_correction();
    result[literals::Dimm::REGIONS] = get_regions().to_json();
    result[literals::Dimm::OEM] = get_oem().to_json();
    return result;
}

Dimm Dimm::from_json(const Json::Value& json) {
    using namespace agent_framework::model::attribute;
    Dimm dimm;
    dimm.set_status(attribute::Status::from_json(
        json[literals::Dimm::STATUS]));
    dimm.set_memory_type(
        json[literals::Dimm::MEMORY_TYPE]);
    dimm.set_dimm_type(
        json[literals::Dimm::DIMM_TYPE]);
    dimm.set_module_type(
        json[literals::Dimm::MODULE_TYPE]);
    dimm.set_dimm_media(attribute::Array<enums::DimmMedia>::from_json(
        json[literals::Dimm::MODULE_TYPE]));
    dimm.set_memory_modes(attribute::Array<enums::MemoryMode>::from_json(
        json[literals::Dimm::MEMORY_MODES]));
    dimm.set_capacity_mb(json[literals::Dimm::CAPACITY_MB]);
    dimm.set_data_width_bits(json[literals::Dimm::DATA_WIDTH_BITS]);
    dimm.set_bus_width_bits(json[literals::Dimm::BUS_WIDTH_BITS]);
    dimm.set_fru_info(FruInfo::from_json(json[literals::Dimm::FRU_INFO]));
    dimm.set_firmware_revision(json[literals::Dimm::FIRMWARE_REVISION]);
    dimm.set_firmware_api_version(
        json[literals::Dimm::FIRMWARE_API_VERSION]);
    dimm.set_memory_classes(attribute::Array<enums::MemoryClass>::from_json(
        json[literals::Dimm::MEMORY_CLASSES]));
    dimm.set_vendor_id(json[literals::Dimm::VENDOR_ID]);
    dimm.set_device_id(json[literals::Dimm::DEVICE_ID]);
    dimm.set_operating_speed_mhz(json[literals::Dimm::OPERATING_SPEED_MHZ]);
    dimm.set_allowed_speeds_mhz(attribute::Array<std::uint32_t>::from_json(
        json[literals::Dimm::ALLOWED_SPEEDS_MHZ]));
    dimm.set_voltage_volt(json[literals::Dimm::VOLTAGE]);
    dimm.set_min_voltage_volt(json[literals::Dimm::MIN_VOLTAGE]);
    dimm.set_max_voltage_volt(json[literals::Dimm::MAX_VOLTAGE]);
    dimm.set_device_locator(json[literals::Dimm::DEVICE_LOCATOR]);
    dimm.set_dimm_location(attribute::DimmLocation::from_json(
        json[literals::Dimm::DIMM_LOCATION]));
    dimm.set_rank_count(json[literals::Dimm::RANK_COUNT]);
    dimm.set_error_correction(
        json[literals::Dimm::ERROR_CORRECTION]);
    dimm.set_regions(attribute::Array<attribute::Region>::from_json(
        json[literals::Dimm::REGIONS]));
    dimm.set_dimm_media(attribute::Array<enums::DimmMedia>::from_json(
        json[literals::Dimm::DIMM_MEDIA]));
    dimm.set_oem(Oem::from_json(json[literals::Dimm::OEM]));
    return dimm;
}
