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
 * */

#include "psme/rest/endpoints/system/memory.hpp"
#include "psme/rest/utils/status_helpers.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace agent_framework::model::enums;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Memory.Memory";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Memory.v1_6_0.Memory";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Memory";
    r[Common::DESCRIPTION] = "Memory description";
    r[Memory::MEMORY_TYPE] = json::Json::value_t::null;
    r[Memory::MEMORY_DEVICE_TYPE] = json::Json::value_t::null;
    r[Memory::BASE_MODULE_TYPE] = json::Json::value_t::null;

    r[Memory::MEMORY_MEDIA] = json::Json::value_t::array;

    r[Memory::CAPACITY_MIB] = json::Json::value_t::null;
    r[Memory::DATA_WIDTH_BITS] = json::Json::value_t::null;
    r[Memory::BUS_WIDTH_BITS] = json::Json::value_t::null;
    r[Common::MANUFACTURER] = json::Json::value_t::null;
    r[Common::SERIAL_NUMBER] = json::Json::value_t::null;
    r[Common::PART_NUMBER] = json::Json::value_t::null;

    r[Memory::ALLOWED_SPEEDS_MHZ] = json::Json::value_t::array;
    r[Memory::MAX_TDP_MILLIWATTS] = json::Json::value_t::array;

    r[Memory::FIRMWARE_REVISION] = json::Json::value_t::null;
    r[Memory::FIRMWARE_API_VERSION] = json::Json::value_t::null;

    r[Memory::MODULE_MANUFACTURER_ID] = json::Json::value_t::null;
    r[Memory::MODULE_PRODUCT_ID] = json::Json::value_t::null;
    r[Memory::MEMORY_SUBSYSTEM_CONTROLLER_MANUFACTURER_ID] = json::Json::value_t::null;
    r[Memory::MEMORY_SUBSYSTEM_CONTROLLER_PRODUCT_ID] = json::Json::value_t::null;

    r[Memory::RANK_COUNT] = json::Json::value_t::null;
    r[Memory::DEVICE_LOCATOR] = json::Json::value_t::null;
    r[Memory::SPARE_DEVICE_COUNT] = json::Json::value_t::null;

    r[Memory::LOGICAL_SIZE_MIB] = json::Json::value_t::null;
    r[Memory::VOLATILE_SIZE_MIB] = json::Json::value_t::null;
    r[Memory::NON_VOLATILE_SIZE_MIB] = json::Json::value_t::null;
    r[Memory::VOLATILE_REGION_SIZE_LIMIT_MIB] = json::Json::value_t::null;
    r[Memory::PERSISTENT_REGION_SIZE_LIMIT_MIB] = json::Json::value_t::null;

    r[Memory::SECURITY_CAPABILITIES][Memory::PASSPHRASE_CAPABLE] = json::Json::value_t::null;
    r[Memory::SECURITY_CAPABILITIES][Memory::MAX_PASSPHRASE_COUNT] = json::Json::value_t::null;
    r[Memory::SECURITY_CAPABILITIES][Memory::SECURITY_STATES] = json::Json::value_t::array;

    r[Memory::POWER_MANAGEMENT_POLICY][Memory::POLICY_ENABLED] = json::Json::value_t::null;
    r[Memory::POWER_MANAGEMENT_POLICY][Memory::MAX_TDP_MILLIWATTS] = json::Json::value_t::null;
    r[Memory::POWER_MANAGEMENT_POLICY][Memory::AVERAGE_POWER_BUDGET_MILLIWATTS] = json::Json::value_t::null;
    r[Memory::POWER_MANAGEMENT_POLICY][Memory::PEAK_POWER_BUDGET_MILLIWATTS] = json::Json::value_t::null;

    r[Memory::MEMORY_LOCATION][Memory::SOCKET] = json::Json::value_t::null;
    r[Memory::MEMORY_LOCATION][Memory::MEMORY_CONTROLLER] = json::Json::value_t::null;
    r[Memory::MEMORY_LOCATION][Memory::CHANNEL] = json::Json::value_t::null;
    r[Memory::MEMORY_LOCATION][Memory::SLOT] = json::Json::value_t::null;

    r[Memory::ERROR_CORRECTION] = json::Json::value_t::null;
    r[Memory::OPERATING_SPEED_MHZ] = json::Json::value_t::null;
    r[Memory::REGIONS] = json::Json::value_t::array;
    r[Memory::OPERATING_MEMORY_MODES] = json::Json::value_t::array;

    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;

    r[Common::METRICS] = json::Json::value_t::null;

    json::Json rs = json::Json();
    rs[Common::ODATA_TYPE] = "#Intel.Oem.Memory";
    rs[Memory::VOLTAGE_VOLT] = json::Json::value_t::null;
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

    auto memory = psme::rest::model::find<agent_framework::model::System, agent_framework::model::Memory>(req.params)
        .get();

    utils::fill_name_and_description(memory, r);

    r[constants::Memory::MEMORY_TYPE] = memory.get_memory_type();
    r[constants::Memory::MEMORY_DEVICE_TYPE] = memory.get_device_type();
    r[constants::Memory::BASE_MODULE_TYPE] = memory.get_module_type();
    for (const auto& key : memory.get_memory_media()) {
        json::Json link_elem(json::Json::value_t::string);
        link_elem = key.to_string();
        r[constants::Memory::MEMORY_MEDIA].push_back(std::move(link_elem));
    }
    r[constants::Memory::CAPACITY_MIB] =
        memory.get_capacity_mib().has_value() ?
        json::Json(memory.get_capacity_mib()) :
        json::Json(json::Json::value_t::null);

    r[constants::Memory::DATA_WIDTH_BITS] = memory.get_data_width_bits();
    r[constants::Memory::BUS_WIDTH_BITS] = memory.get_bus_width_bits();
    r[Common::MANUFACTURER] = memory.get_fru_info().get_manufacturer();
    r[Common::SERIAL_NUMBER] = memory.get_fru_info().get_serial_number();
    r[Common::PART_NUMBER] = memory.get_fru_info().get_part_number();

    for (const auto& speed : memory.get_allowed_speeds_mhz()) {
        r[constants::Memory::ALLOWED_SPEEDS_MHZ].push_back(speed);
    }

    for (const auto& max_tdp : memory.get_max_tdp_milliwats()) {
        r[constants::Memory::MAX_TDP_MILLIWATTS].push_back(max_tdp);
    }

    r[constants::Memory::FIRMWARE_REVISION] = memory.get_firmware_revision();
    r[constants::Memory::FIRMWARE_API_VERSION] = memory.get_firmware_api_version();

    r[constants::Memory::MODULE_MANUFACTURER_ID] = memory.get_module_manufacturer_id();
    r[constants::Memory::MODULE_PRODUCT_ID] = memory.get_module_product_id();
    r[constants::Memory::MEMORY_SUBSYSTEM_CONTROLLER_MANUFACTURER_ID] = memory.get_memory_subsystem_controller_manufacturer_id();
    r[constants::Memory::MEMORY_SUBSYSTEM_CONTROLLER_PRODUCT_ID] = memory.get_memory_subsystem_controller_product_id();

    r[constants::Memory::RANK_COUNT] = memory.get_rank_count();
    r[constants::Memory::DEVICE_LOCATOR] = memory.get_device_locator();
    r[constants::Memory::SPARE_DEVICE_COUNT] = memory.get_spare_device_count();

    r[constants::Memory::LOGICAL_SIZE_MIB] = memory.get_logical_size_mib();
    r[constants::Memory::VOLATILE_SIZE_MIB] = memory.get_volatile_size_mib();
    r[constants::Memory::NON_VOLATILE_SIZE_MIB] = memory.get_non_volatile_size_mib();
    r[constants::Memory::VOLATILE_REGION_SIZE_LIMIT_MIB] = memory.get_volatile_region_size_limit_mib();
    r[constants::Memory::PERSISTENT_REGION_SIZE_LIMIT_MIB] = memory.get_persistent_region_size_limit_mib();

    const auto& location = memory.get_location();
    json::Json location_json(json::Json::value_t::object);
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
        json::Json link_elem(json::Json::value_t::object);
        link_elem[constants::Memory::REGION_ID] = region.get_region_id();
        link_elem[constants::Memory::MEMORY_CLASSIFICATION] = region.get_memory_type();
        link_elem[constants::Memory::OFFSET_MIB] = region.get_offset_mib();
        link_elem[constants::Memory::SIZE_MIB] = region.get_size_mib();
        r[constants::Memory::REGIONS].push_back(std::move(link_elem));
    }

    for (const auto& mode : memory.get_memory_modes()) {
        json::Json link_elem(json::Json::value_t::string);
        link_elem = mode.to_string();
        r[constants::Memory::OPERATING_MEMORY_MODES].push_back(std::move(link_elem));
    }

    const auto& security_capabilities = memory.get_security_capabilities();
    json::Json security_capabilities_json(json::Json::value_t::object);
    security_capabilities_json[constants::Memory::PASSPHRASE_CAPABLE] = security_capabilities.get_passphrase_capable();
    security_capabilities_json[constants::Memory::MAX_PASSPHRASE_COUNT] = security_capabilities.get_max_passphrase_count();
    for (const auto& security_state : security_capabilities.get_security_states()) {
        json::Json link_elem(json::Json::value_t::string);
        link_elem = security_state.to_string();
        security_capabilities_json[constants::Memory::SECURITY_STATES].push_back(std::move(link_elem));
    }
    r[constants::Memory::SECURITY_CAPABILITIES] = std::move(security_capabilities_json);

    const auto& power_management_policy = memory.get_power_management_policy();
    json::Json power_management_policy_json(json::Json::value_t::object);
    power_management_policy_json[constants::Memory::POLICY_ENABLED] = power_management_policy.get_policy_enabled();
    power_management_policy_json[constants::Memory::MAX_TDP_MILLIWATTS] = power_management_policy.get_max_tdp_milliwatts();
    power_management_policy_json[constants::Memory::AVERAGE_POWER_BUDGET_MILLIWATTS] = power_management_policy.get_average_power_budget_milliwatts();
    power_management_policy_json[constants::Memory::PEAK_POWER_BUDGET_MILLIWATTS] = power_management_policy.get_peak_power_budget_milliwatts();
    r[constants::Memory::POWER_MANAGEMENT_POLICY] = std::move(power_management_policy_json);

    r[constants::Common::METRICS][Common::ODATA_ID] = PathBuilder(req).append(constants::Common::METRICS).build();
    r[Common::OEM][Common::RACKSCALE][constants::Memory::VOLTAGE_VOLT] = memory.get_voltage_volt();

    set_response(res, r);
}
