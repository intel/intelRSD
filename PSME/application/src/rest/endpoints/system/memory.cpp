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
 * */

#include "psme/rest/endpoints/system/memory.hpp"
#include "psme/rest/utils/status_helpers.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace agent_framework::model::enums;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Memory.Memory";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Memory.v1_1_0.Memory";
    r[Common::ID] =  json::Value::Type::NIL;
    r[Common::NAME] = "Memory";
    r[Common::DESCRIPTION] = "Memory description";
    r[Memory::MEMORY_TYPE] = json::Value::Type::NIL;
    r[Memory::MEMORY_DEVICE_TYPE] = json::Value::Type::NIL;
    r[Memory::BASE_MODULE_TYPE] = json::Value::Type::NIL;

    r[Memory::MEMORY_MEDIA] = json::Value::Type::ARRAY;

    r[Memory::CAPACITY_MIB] = json::Value::Type::NIL;
    r[Memory::DATA_WIDTH_BITS] = json::Value::Type::NIL;
    r[Memory::BUS_WIDTH_BITS] = json::Value::Type::NIL;
    r[Common::MANUFACTURER] = json::Value::Type::NIL;
    r[Common::SERIAL_NUMBER] = json::Value::Type::NIL;
    r[Common::PART_NUMBER] = json::Value::Type::NIL;

    r[Memory::ALLOWED_SPEEDS_MHZ] = json::Value::Type::ARRAY;

    r[Memory::FIRMWARE_REVISION] = json::Value::Type::NIL;
    r[Memory::FIRMWARE_API_VERSION] = json::Value::Type::NIL;

    r[Memory::FUNCTION_CLASSES] = json::Value::Type::ARRAY;

    r[Memory::VENDOR_ID] = json::Value::Type::NIL;
    r[Memory::DEVICE_ID] = json::Value::Type::NIL;
    r[Memory::RANK_COUNT] =json::Value::Type::NIL;
    r[Memory::DEVICE_LOCATOR] = json::Value::Type::NIL;

    r[Memory::MEMORY_LOCATION][Memory::SOCKET] = json::Value::Type::NIL;
    r[Memory::MEMORY_LOCATION][Memory::MEMORY_CONTROLLER] = json::Value::Type::NIL;
    r[Memory::MEMORY_LOCATION][Memory::CHANNEL] = json::Value::Type::NIL;
    r[Memory::MEMORY_LOCATION][Memory::SLOT] = json::Value::Type::NIL;

    r[Memory::ERROR_CORRECTION] = json::Value::Type::NIL;
    r[Memory::OPERATING_SPEED_MHZ] = json::Value::Type::NIL;
    r[Memory::REGIONS] = json::Value::Type::ARRAY;
    r[Memory::OPERATING_MEMORY_MODES] = json::Value::Type::ARRAY;

    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    r[Common::METRICS] = json::Value::Type::NIL;

    json::Value rs{};
    rs[Common::ODATA_TYPE] = "#Intel.Oem.Memory";
    rs[Memory::VOLTAGE_VOLT] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE] = std::move(rs);

    return r;
}
}

endpoint::Memory::Memory(const std::string& path) : EndpointBase(path) {}
endpoint::Memory::~Memory() {}

void endpoint::Memory::get(const server::Request& req, server::Response& res) {

    auto r = make_prototype();
    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ID] = req.params[PathParam::MEMORY_ID];

    auto memory = psme::rest::model::Find<agent_framework::model::Memory>(req.params[PathParam::MEMORY_ID])
        .via<agent_framework::model::System>(req.params[PathParam::SYSTEM_ID]).get();

    r[constants::Memory::MEMORY_TYPE] = memory.get_memory_type();
    r[constants::Memory::MEMORY_DEVICE_TYPE] = memory.get_device_type();
    r[constants::Memory::BASE_MODULE_TYPE] = memory.get_module_type();
    for (const auto& key : memory.get_memory_media()) {
        json::Value link_elem(json::Value::Type::STRING);
        link_elem = key.to_string();
        r[constants::Memory::MEMORY_MEDIA].push_back(std::move(link_elem));
    }
    r[constants::Memory::CAPACITY_MIB] =
        memory.get_capacity_mb().has_value() ?
            json::Value(endpoint::utils::mb_to_mib(memory.get_capacity_mb())) :
            json::Value(json::Value::Type::NIL);

    r[constants::Memory::DATA_WIDTH_BITS] = memory.get_data_width_bits();
    r[constants::Memory::BUS_WIDTH_BITS] = memory.get_bus_width_bits();
    r[Common::MANUFACTURER] = memory.get_fru_info().get_manufacturer();
    r[Common::SERIAL_NUMBER] = memory.get_fru_info().get_serial_number();
    r[Common::PART_NUMBER] = memory.get_fru_info().get_part_number();

    for (const auto& speed : memory.get_allowed_speeds_mhz()) {
        json::Value link_elem(json::Value::Type::NUMBER);
        link_elem = speed;
        r[constants::Memory::ALLOWED_SPEEDS_MHZ].push_back(std::move(link_elem));
    }

    r[constants::Memory::FIRMWARE_REVISION] = memory.get_firmware_revision();
    r[constants::Memory::FIRMWARE_API_VERSION] =
        memory.get_firmware_api_version();

    for (const auto& function : memory.get_memory_classes()) {
        json::Value link_elem(json::Value::Type::STRING);
        link_elem = function.to_string();
        r[constants::Memory::FUNCTION_CLASSES].push_back(std::move(link_elem));
    }

    r[constants::Memory::VENDOR_ID] = memory.get_vendor_id();
    r[constants::Memory::DEVICE_ID] = memory.get_device_id();
    r[constants::Memory::RANK_COUNT] = memory.get_rank_count();
    r[constants::Memory::DEVICE_LOCATOR] = memory.get_device_locator();

    auto location = memory.get_location();
    json::Value location_json(json::Value::Type::OBJECT);
    location_json[constants::Memory::SOCKET] = location.get_socket();
    location_json[constants::Memory::MEMORY_CONTROLLER] = location.get_controller();
    location_json[constants::Memory::CHANNEL] = location.get_channel();
    location_json[constants::Memory::SLOT] = location.get_slot();
    r[constants::Memory::MEMORY_LOCATION] = std::move(location_json);

    r[constants::Memory::ERROR_CORRECTION] = memory.get_error_correction();
    endpoint::status_to_json(memory, r);
    r[Common::STATUS][Common::HEALTH_ROLLUP] = memory.get_status().get_health();
    r[constants::Memory::OPERATING_SPEED_MHZ] = memory.get_operating_speed_mhz();

    for (const auto& region : memory.get_regions()) {
        json::Value link_elem(json::Value::Type::OBJECT);
        link_elem[constants::Memory::REGION_ID] = region.get_region_id();
        link_elem[constants::Memory::MEMORY_CLASSIFICATION] = region.get_memory_type();
        link_elem[constants::Memory::OFFSET_MIB] = (region.get_offset_mb().has_value()) ?
            json::Value(uint32_t(endpoint::utils::mb_to_mib(region.get_offset_mb().value()))) : json::Value::Type::NIL;
        link_elem[constants::Memory::SIZE_MIB] = (region.get_size_mb().has_value()) ?
            json::Value(uint32_t(endpoint::utils::mb_to_mib(region.get_size_mb().value()))) : json::Value::Type::NIL;
        r[constants::Memory::REGIONS].
            push_back(std::move(link_elem));
    }

    for (const auto& mode : memory.get_memory_modes()) {
        json::Value link_elem(json::Value::Type::STRING);
        link_elem = mode.to_string();
        r[constants::Memory::OPERATING_MEMORY_MODES].push_back(std::move(link_elem));
    }

    r[constants::Common::METRICS][Common::ODATA_ID] = PathBuilder(req).append(constants::Common::METRICS).build();
    r[Common::OEM][Common::RACKSCALE][constants::Memory::VOLTAGE_VOLT] = memory.get_voltage_volt();

    set_response(res, r);
}
