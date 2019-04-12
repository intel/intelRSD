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

#include "psme/rest/endpoints/system/storage_subsystem.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/storage_subsystem.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "agent-framework/module/constants/compute.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/responses/common.hpp"



using namespace psme::rest;
using namespace psme::rest::error;
using namespace psme::rest::constants;
using namespace psme::rest::validators;
using namespace agent_framework::model;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Storage.Storage";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Storage.v1_1_0.Storage";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Storage Subsystem";
    r[Common::DESCRIPTION] = "Storage Subsystem description";
    r[constants::StorageSubsystem::STORAGE_CONTROLLERS] = json::Json::value_t::array;
    r[constants::StorageSubsystem::VOLUMES] = json::Json::value_t::null;
    r[constants::StorageSubsystem::DRIVES] = json::Json::value_t::array;
    r[constants::Common::REDUNDANCY] = json::Json::value_t::array;
    r[Common::LINKS] = json::Json::value_t::object;
    r[Common::ACTIONS] = json::Json::value_t::object;

    return r;
}


json::Json storage_controller_to_json(const std::string& controller_uuid) {
    const auto sc = agent_framework::module::get_manager<agent_framework::model::StorageController>()
        .get_entry(controller_uuid);

    json::Json json = json::Json();
    json[Common::ODATA_ID] = endpoint::utils::get_component_url(enums::Component::StorageController, controller_uuid);
    json[Common::ODATA_TYPE] = "#Storage.v1_1_0.StorageController";
    json[Common::MEMBER_ID] = std::to_string(sc.get_id());

    const auto& status = sc.get_status();
    json[Common::STATUS][Common::STATE] = status.get_state().to_string();
    json[Common::STATUS][Common::HEALTH] = status.get_health();
    json[Common::STATUS][Common::HEALTH_ROLLUP] = status.get_health();

    const auto& fru = sc.get_fru_info();
    json[Common::MANUFACTURER] = fru.get_manufacturer();
    json[Common::MODEL] = fru.get_model_number();
    json[Common::SERIAL_NUMBER] = fru.get_serial_number();
    json[Common::PART_NUMBER] = fru.get_part_number();

    json[Common::ASSET_TAG] = sc.get_asset_tag();
    json[Common::SKU] = sc.get_sku();
    json[constants::StorageSubsystem::SPEED_GBPS] = sc.get_speed_gbps();
    json[constants::StorageSubsystem::FIRMWARE_VERSION] = sc.get_firmware_version();

    json[constants::StorageSubsystem::SUPPORTED_CONTROLLER_PROTOCOLS] = json::Json::value_t::array;
    for (const auto& controller_protocol : sc.get_supported_controller_protocols()) {
        json[constants::StorageSubsystem::SUPPORTED_CONTROLLER_PROTOCOLS].push_back(
            controller_protocol.to_string());
    }

    json[constants::StorageSubsystem::SUPPORTED_DEVICE_PROTOCOLS] = json::Json::value_t::array;
    for (const auto& device_protocol : sc.get_supported_device_protocols()) {
        json[constants::StorageSubsystem::SUPPORTED_DEVICE_PROTOCOLS].push_back(
            device_protocol.to_string());
    }

    json[constants::StorageSubsystem::IDENTIFIERS] = json::Json::value_t::array;
    for (const auto& identifier : sc.get_identifiers()) {
        json::Json id = json::Json();
        id[constants::StorageSubsystem::DURABLE_NAME] = identifier.get_durable_name();
        id[constants::StorageSubsystem::DURABLE_NAME_FORMAT] = identifier.get_durable_name_format();
        json[constants::StorageSubsystem::IDENTIFIERS].push_back(std::move(id));
    }

    // this array will never have any links, because we do not create a StorageController in the PNC agent
    json[constants::Common::LINKS][constants::System::ENDPOINTS] = json::Json::value_t::array;

    return json;
}


static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::StorageSubsystem::ASSET_TAG, constants::StorageSubsystem::ASSET_TAG}
};

}


endpoint::StorageSubsystem::StorageSubsystem(const std::string& path) : EndpointBase(path) {}


endpoint::StorageSubsystem::~StorageSubsystem() {}


void endpoint::StorageSubsystem::get(const server::Request& req, server::Response& res) {
    auto r = make_prototype();
    auto storage = psme::rest::model::find<agent_framework::model::System, agent_framework::model::StorageSubsystem>(
        req.params).get();

    r[constants::Common::ODATA_ID] = PathBuilder(req).build();
    r[constants::Common::ID] = req.params[PathParam::STORAGE_ID];

    endpoint::status_to_json(storage, r);

    const auto controller_uuids = agent_framework::module::get_manager<agent_framework::model::StorageController>()
        .get_keys(storage.get_uuid());
    for (const auto& controller_uuid : controller_uuids) {
        r[constants::StorageSubsystem::STORAGE_CONTROLLERS].push_back(
            std::move(storage_controller_to_json(controller_uuid)));
    }

    const auto manager_uuid = psme::rest::model::find<agent_framework::model::System>(
        req.params).get().get_parent_uuid();

    const auto chassis_uuids = agent_framework::module::get_manager<agent_framework::model::Chassis>().get_keys(
        manager_uuid);
    if (chassis_uuids.size() == 0) {
        log_error("rest", "Local storage Chassis not found.");
    }
    else if (chassis_uuids.size() > 1) {
        log_error("rest", "Too many Local storage Chassis found.");
    }
    else {
        // fill Chassis link
        json::Json chassis_link = json::Json();
        chassis_link[Common::ODATA_ID] = endpoint::utils::get_component_url(
            agent_framework::model::enums::Component::Chassis, chassis_uuids.front());
        r[Common::LINKS][constants::StorageSubsystem::ENCLOSURES].push_back(std::move(chassis_link));
    }

    const auto drive_uuids = agent_framework::module::CommonComponents::get_instance()
        ->get_storage_subsystem_drives_manager().get_children(storage.get_uuid());

    for (const auto& drive_uuid: drive_uuids) {
        json::Json drive_link = json::Json();
        drive_link[Common::ODATA_ID] = endpoint::utils::get_component_url(
            agent_framework::model::enums::Component::Drive, drive_uuid);
        r[constants::StorageSubsystem::DRIVES].push_back(std::move(drive_link));
    }

    r[constants::StorageSubsystem::VOLUMES][Common::ODATA_ID] = PathBuilder(req).append(
        constants::StorageSubsystem::VOLUMES).build();

    set_response(res, r);
}
