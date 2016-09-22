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
 * */

#include "psme/rest-ref/endpoints/dimm_config.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include <regex>

using namespace psme::rest;
using namespace app::rest;
using namespace app::rest::constants;
using namespace agent_framework::model::enums;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Systems/Members/__SYSTEM_ID__/Memory/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Memory.1.0.0.Memory";
    r[Common::ID] =  json::Value::Type::NIL;
    r[Common::NAME] = "Memory";
    r[Common::DESCRIPTION] = "Memory description";
    r[DimmConfig::DIMM_TYPE] = json::Value::Type::NIL;
    r[DimmConfig::DIMM_DEVICE_TYPE] = json::Value::Type::NIL;
    r[DimmConfig::BASE_MODULE_TYPE] = json::Value::Type::NIL;

    r[DimmConfig::DIMM_MEDIA] = json::Value::Type::ARRAY;

    r[DimmConfig::CAPACITY_MIB] = json::Value::Type::NIL;
    r[DimmConfig::DATA_WIDTH_BITS] = json::Value::Type::NIL;
    r[DimmConfig::BUS_WIDTH_BITS] = json::Value::Type::NIL;
    r[Common::MANUFACTURER] = json::Value::Type::NIL;
    r[Common::SERIAL] = json::Value::Type::NIL;
    r[Common::PART_NUMBER] = json::Value::Type::NIL;

    r[DimmConfig::ALLOWED_SPEEDS_MHZ] = json::Value::Type::ARRAY;

    r[DimmConfig::FIRMWARE_REVISION] = json::Value::Type::NIL;
    r[DimmConfig::FIRMWARE_API_VERSION] = json::Value::Type::NIL;

    r[DimmConfig::FUNCTION_CLASSES] = json::Value::Type::ARRAY;

    r[DimmConfig::VENDOR_ID] = json::Value::Type::NIL;
    r[DimmConfig::DEVICE_ID] = json::Value::Type::NIL;
    r[DimmConfig::RANK_COUNT] =json::Value::Type::NIL;
    r[DimmConfig::DEVICE_LOCATOR] = json::Value::Type::NIL;

    r[DimmConfig::DIMM_LOCATION][DimmConfig::SOCKET] = json::Value::Type::NIL;
    r[DimmConfig::DIMM_LOCATION][DimmConfig::MEMORY_CONTROLLER] = json::Value::Type::NIL;
    r[DimmConfig::DIMM_LOCATION][DimmConfig::CHANNEL] = json::Value::Type::NIL;
    r[DimmConfig::DIMM_LOCATION][DimmConfig::SLOT] = json::Value::Type::NIL;

    r[DimmConfig::ERROR_CORRECTION] = json::Value::Type::NIL;
    r[DimmConfig::OPERATING_SPEED_MHZ] = json::Value::Type::NIL;
    r[DimmConfig::REGIONS] = json::Value::Type::ARRAY;
    r[DimmConfig::OPERATING_MEMORY_MODES] = json::Value::Type::ARRAY;

    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    json::Value rs;
    rs[Common::ODATA_TYPE] = "#Intel.Oem.Memory";
    rs[DimmConfig::VOLTAGE_VOLT] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE] = std::move(rs);

    return r;
}

std::string to_rest_dimm_device_type(const DimmType& ddt) {
    switch(ddt) {
        case DimmType::DDR:
        case DimmType::DDR2:
        case DimmType::DDR3:
        case DimmType::DDR4:
        case DimmType::ROM:
        case DimmType::SDRAM:
        case DimmType::EDO:
        case DimmType::Unknown:
            return ddt.to_string();
        case DimmType::DDR4_SDRAM:
            return std::string{"DDR4 SDRAM"};
        case DimmType::DDR4E_SDRAM:
            return std::string{"DDR4E SDRAM"};
        case DimmType::LPDDR4_SDRAM:
            return std::string{"LPDDR4 SDRAM"};
        case DimmType::DDR3_SDRAM:
            return std::string{"DDR3 SDRAM"};
        case DimmType::LPDDR3_SDRAM:
            return std::string{"LPDDR3 SDRAM"};
        case DimmType::DDR2_SDRAM:
            return std::string{"DDR2 SDRAM"};
        case DimmType::DDR2_SDRAM_FB_DIMM:
            return std::string{"DDR2 SDRAM FB-DIMM"};
        case DimmType::DDR2_SDRAM_FB_DIMM_PROBE:
            return std::string{"DDR2 SDRAM FB-DIMM PROBE"};
        case DimmType::DDR_SGRAM:
            return std::string{"DDR SGRAM"};
        case DimmType::DDR_SDRAM:
            return std::string{"DDR SDRAM"};
        case DimmType::Fast_Page_Mode:
            return std::string{"Fast Page Mode"};
        case DimmType::PipelinedNibble:
            return std::string{"Pipelined Nibble"};
        default:
            return std::string{"Not handled MemoryDeviceType enum value"};
    }
}

}

endpoint::DimmConfig::DimmConfig(const std::string& path) : Endpoint(path) {}
endpoint::DimmConfig::~DimmConfig() {}

void endpoint::DimmConfig::get(const server::Request& req, server::Response& res) {
    auto system_id = psme::rest::model::Find
            <agent_framework::model::System>
            (req.params[PathParam::SYSTEM_ID]).get_one()->get_id();

    auto r = make_prototype();
    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ODATA_CONTEXT] = std::regex_replace(r[Common::ODATA_CONTEXT].as_string(),
                                                  std::regex("__SYSTEM_ID__"), std::to_string(system_id));
    r[Common::ID] = req.params[PathParam::DIMM_CONFIG_ID];
    auto dimm = psme::rest::model::Find
        <agent_framework::model::Dimm>
        (req.params[PathParam::DIMM_CONFIG_ID]).via
        <agent_framework::model::System>
        (req.params[PathParam::SYSTEM_ID]).get();

    r[constants::DimmConfig::DIMM_TYPE] = dimm.get_memory_type();
    r[constants::DimmConfig::DIMM_DEVICE_TYPE] =
        (dimm.get_dimm_type().has_value() ?
        json::Value(to_rest_dimm_device_type(dimm.get_dimm_type())) :
        json::Value::Type::NIL );
    r[constants::DimmConfig::BASE_MODULE_TYPE] = dimm.get_module_type();
    for (const auto& key : dimm.get_dimm_media()) {
        json::Value link_elem(json::Value::Type::STRING);
        link_elem = key.to_string();
        r[constants::DimmConfig::DIMM_MEDIA].push_back(std::move(link_elem));
    }
    r[constants::DimmConfig::CAPACITY_MIB] = (dimm.get_capacity_mb().has_value() ?
        json::Value(uint32_t(endpoint::utils::mb_to_mib(dimm.get_capacity_mb()))) :
        json::Value(json::Value::Type::NIL) );

    r[constants::DimmConfig::DATA_WIDTH_BITS] = dimm.get_data_width_bits();
    r[constants::DimmConfig::BUS_WIDTH_BITS] = dimm.get_bus_width_bits();
    r[Common::MANUFACTURER] = dimm.get_fru_info().get_manufacturer();
    r[Common::SERIAL] = dimm.get_fru_info().get_serial_number();
    r[Common::PART_NUMBER] = dimm.get_fru_info().get_part_number();

    for (const auto& speed : dimm.get_allowed_speeds_mhz()) {
        json::Value link_elem(json::Value::Type::NUMBER);
        link_elem = speed;
        r[constants::DimmConfig::DIMM_MEDIA].push_back(std::move(link_elem));
    }

    r[constants::DimmConfig::FIRMWARE_REVISION] = dimm.get_firmware_revision();
    r[constants::DimmConfig::FIRMWARE_API_VERSION] =
        dimm.get_firmware_api_version();

    for (const auto& function : dimm.get_memory_classes()) {
        json::Value link_elem(json::Value::Type::STRING);
        link_elem = function.to_string();
        r[constants::DimmConfig::FUNCTION_CLASSES].
            push_back(std::move(link_elem));
    }

    r[constants::DimmConfig::VENDOR_ID] = dimm.get_vendor_id();
    r[constants::DimmConfig::DEVICE_ID] = dimm.get_device_id();
    r[constants::DimmConfig::RANK_COUNT] = dimm.get_rank_count();
    r[constants::DimmConfig::DEVICE_LOCATOR] = dimm.get_device_locator();

    auto location = dimm.get_dimm_location();
    json::Value location_json(json::Value::Type::OBJECT);
    location_json[constants::DimmConfig::SOCKET] = location.get_socket();
    location_json[constants::DimmConfig::MEMORY_CONTROLLER] =
        location.get_controller();
    location_json[constants::DimmConfig::CHANNEL] = location.get_channel();
    location_json[constants::DimmConfig::SLOT] = location.get_slot();
    r[constants::DimmConfig::DIMM_LOCATION] = std::move(location_json);

    r[constants::DimmConfig::ERROR_CORRECTION] =
        dimm.get_error_correction();
    endpoint::utils::status_to_json(dimm, r);
    r[constants::DimmConfig::OPERATING_SPEED_MHZ] =
        dimm.get_operating_speed_mhz();

    for (const auto& region : dimm.get_regions()) {
        json::Value link_elem(json::Value::Type::OBJECT);
        link_elem[constants::DimmConfig::REGION_ID] = region.get_region_id();
        link_elem[constants::DimmConfig::MEMORY_TYPE] = region.get_memory_type();
        link_elem[constants::DimmConfig::OFFSET_MIB] = (region.get_offset_mb().has_value()) ?
            json::Value(uint32_t(endpoint::utils::mb_to_mib(region.get_offset_mb().value()))) : json::Value::Type::NIL;
        link_elem[constants::DimmConfig::SIZE_MIB] = (region.get_size_mb().has_value()) ?
            json::Value(uint32_t(endpoint::utils::mb_to_mib(region.get_size_mb().value()))) : json::Value::Type::NIL;
        r[constants::DimmConfig::REGIONS].
            push_back(std::move(link_elem));
    }

    for (const auto& mode : dimm.get_memory_modes()) {
        json::Value link_elem(json::Value::Type::STRING);
        link_elem = mode.to_string();
        r[constants::DimmConfig::OPERATING_MEMORY_MODES].
            push_back(std::move(link_elem));
    }

    r[Common::OEM][Common::RACKSCALE][constants::DimmConfig::VOLTAGE_VOLT] =
        dimm.get_voltage_volt();

    set_response(res, r);
}
