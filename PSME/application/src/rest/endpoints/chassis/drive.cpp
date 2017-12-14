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
 * */

#include "psme/rest/endpoints/chassis/drive.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/drive.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/responses/common.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "psme/rest/server/error/error_factory.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::endpoint::utils;
using namespace psme::rest::validators;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Drive.Drive";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Drive.v1_1_1.Drive";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Drive";
    r[Common::DESCRIPTION] = "Drive description";

    r[Common::ACTIONS][PncDrive::SECURE_ERASE][Common::TARGET] = json::Value::Type::NIL;
    r[Common::ASSET_TAG] = json::Value::Type::NIL;
    r[PncDrive::INDICATOR_LED] = json::Value::Type::NIL;

    r[Common::LINKS][Common::ODATA_TYPE] = "#Drive.v1_1_0.Links";
    r[Common::LINKS][PncDrive::ENDPOINTS] = json::Value::Type::ARRAY;
    r[Common::LINKS][PncDrive::VOLUMES] = json::Value::Type::ARRAY;

    r[Common::LOCATION] = json::Value::Type::ARRAY;
    r[PncDrive::IDENTIFIERS] = json::Value::Type::ARRAY;

    r[Common::MANUFACTURER] = json::Value::Type::NIL;
    r[PncDrive::MEDIA_TYPE] = json::Value::Type::NIL;
    r[Common::MODEL] = json::Value::Type::NIL;
    r[PncDrive::CAPABLE_SPEED] = json::Value::Type::NIL;
    r[PncDrive::NEGOTIATED_SPEED] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.Drive";
    r[Common::OEM][Common::RACKSCALE][PncDrive::ERASE_ON_DETACH] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE][PncDrive::DRIVE_ERASED] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE][PncDrive::FIRMWARE_VERSION] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE][System::STORAGE] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE][PncDrive::PCIE_FUNCTION] = json::Value::Type::NIL;
    r[Common::PART_NUMBER] = json::Value::Type::NIL;
    r[PncDrive::PROTOCOL] = json::Value::Type::NIL;
    r[Common::SERIAL_NUMBER] = json::Value::Type::NIL;
    r[PncDrive::CAPACITY_BYTES] = json::Value::Type::NIL;

    r[Common::SKU] = json::Value::Type::NIL;
    r[PncDrive::STATUS_INDICATOR] = json::Value::Type::NIL;
    r[PncDrive::REVISION] = json::Value::Type::NIL;
    r[PncDrive::FAILURE_PREDICTED] = json::Value::Type::NIL;
    r[Common::ASSET_TAG] = json::Value::Type::NIL;
    r[PncDrive::HOTSPARE_TYPE] = json::Value::Type::NIL;
    r[PncDrive::ENCRYPTION_ABILITY] = json::Value::Type::NIL;
    r[PncDrive::ENCRYPTION_STATUS] = json::Value::Type::NIL;
    r[PncDrive::ROTATION_SPEED_RPM] = json::Value::Type::NIL;
    r[PncDrive::BLOCK_SIZE_BYTES] = json::Value::Type::NIL;
    r[PncDrive::PREDICTED_MEDIA_LIFE_LEFT] = json::Value::Type::NIL;

    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    return r;
}


void add_links(const agent_framework::model::Drive& drive, json::Value& json) {
    // fill Storage link
    const auto storage_uuids = agent_framework::module::CommonComponents::get_instance()->get_storage_subsystem_drives_manager().get_parents(
        drive.get_uuid());
    if (storage_uuids.size() > 1) {
        log_error(GET_LOGGER("rest"), "Drive " + drive.get_uuid() + " is assigned to more than one storage subsystem!");
    }
    else if (storage_uuids.size() == 0) {
        log_error(GET_LOGGER("rest"), "Drive " + drive.get_uuid() + " is not assigned to any storage subsystems!");
    }
    else {
        const auto storage_uuid = storage_uuids.front();
        try {
            json[Common::OEM][Common::RACKSCALE][System::STORAGE][Common::ODATA_ID] = psme::rest::endpoint::utils::get_component_url(
                agent_framework::model::enums::Component::StorageSubsystem, storage_uuid);

        }
        catch (agent_framework::exceptions::InvalidUuid&) {
            log_error(GET_LOGGER("rest"),
                      "Drive " + drive.get_uuid() + " is assigned to a non existent storage subsystem!");
        }
    }

    // fill PCIeFunction link
    for (const auto& function_uuid : agent_framework::module::PncComponents::get_instance()->get_drive_function_manager().get_children(
        drive.get_uuid())) {
        try {
            json[Common::OEM][Common::RACKSCALE][PncDrive::PCIE_FUNCTION][Common::ODATA_ID] = psme::rest::endpoint::utils::get_component_url(
                agent_framework::model::enums::Component::PcieFunction, function_uuid);
        }
        catch (agent_framework::exceptions::InvalidUuid&) {
            log_error(GET_LOGGER("rest"),
                      "Drive " + drive.get_uuid() + " has non-existing PCIeFunctions!");
        }
    }

    // fill endpoints links
    auto& manager = agent_framework::module::get_manager<agent_framework::model::Endpoint>();
    for (const auto& endpoint_uuid : manager.get_keys()) {
        const auto endpoint = manager.get_entry(endpoint_uuid);
        if (endpoint.has_drive_entity(drive.get_uuid())) {
            json::Value endpoint_link;
            endpoint_link[Common::ODATA_ID] = psme::rest::endpoint::utils::get_component_url(
                agent_framework::model::enums::Component::Endpoint, endpoint_uuid);
            json[Common::LINKS][PncDrive::ENDPOINTS].push_back(std::move(endpoint_link));
        }
    }
}


static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::Drive::ASSET_TAG, constants::Common::ASSET_TAG},
    {agent_framework::model::literals::Drive::ERASED,
     endpoint::PathBuilder(Common::OEM)
         .append(Common::RACKSCALE)
         .append(constants::PncDrive::DRIVE_ERASED)
         .build()
    }
};

}


endpoint::Drive::Drive(const std::string& path) : EndpointBase(path) {}


endpoint::Drive::~Drive() {}


void endpoint::Drive::get(const server::Request& req, server::Response& res) {
    auto r = make_prototype();

    r[Common::ODATA_ID] = PathBuilder(req).build();

    auto drive = psme::rest::model::Find<agent_framework::model::Drive>(req.params[PathParam::DRIVE_ID])
        .via<agent_framework::model::Chassis>(req.params[PathParam::CHASSIS_ID]).get();

    r[Common::ID] = req.params[PathParam::DRIVE_ID];

    r[Common::ACTIONS][PncDrive::SECURE_ERASE][Common::TARGET] = endpoint::PathBuilder(req)
        .append(Common::ACTIONS)
        .append(PncDrive::SECURE_ERASE_ENDPOINT).build();

    add_links(drive, r);

    const auto& fru = drive.get_fru_info();
    r[Common::MANUFACTURER] = fru.get_manufacturer();
    r[Common::MODEL] = fru.get_model_number();
    r[Common::PART_NUMBER] = fru.get_part_number();
    r[Common::SERIAL_NUMBER] = fru.get_serial_number();

    r[Common::ASSET_TAG] = drive.get_asset_tag();
    r[PncDrive::MEDIA_TYPE] = drive.get_type();
    r[PncDrive::CAPABLE_SPEED] = drive.get_capable_speed_gbs();
    r[PncDrive::PROTOCOL] = drive.get_interface();
    r[PncDrive::NEGOTIATED_SPEED] = drive.get_negotiated_speed_gbs();

    r[Common::SKU] = drive.get_sku();
    r[PncDrive::STATUS_INDICATOR] = drive.get_status_indicator();
    r[PncDrive::REVISION] = drive.get_revision();
    r[PncDrive::FAILURE_PREDICTED] = drive.get_failure_predicted();
    r[PncDrive::HOTSPARE_TYPE] = drive.get_hotspare_type();
    r[PncDrive::ENCRYPTION_ABILITY] = drive.get_encryption_ability();
    r[PncDrive::ENCRYPTION_STATUS] = drive.get_encryption_status();
    r[PncDrive::ROTATION_SPEED_RPM] = drive.get_rpm();
    r[PncDrive::BLOCK_SIZE_BYTES] = drive.get_block_size_bytes();
    r[PncDrive::PREDICTED_MEDIA_LIFE_LEFT] = drive.get_predicted_media_life_left();

    r[Common::OEM][Common::RACKSCALE][PncDrive::DRIVE_ERASED] = drive.get_erased();
    r[Common::OEM][Common::RACKSCALE][PncDrive::FIRMWARE_VERSION] = drive.get_firmware_version();

    for (const auto& location : drive.get_locations()) {
        json::Value l{};
        l[PncDrive::INFO] = location.get_info();
        l[PncDrive::INFO_FORMAT] = location.get_info_format();
        r[Common::LOCATION].push_back(std::move(l));
    }

    for (const auto& identifier : drive.get_identifiers()) {
        json::Value id{};
        id[PncDrive::DURABLE_NAME] = identifier.get_durable_name();
        id[PncDrive::DURABLE_NAME_FORMAT] = identifier.get_durable_name_format().to_string();
        r[PncDrive::IDENTIFIERS].push_back(std::move(id));
    }

    if (drive.get_capacity_gb().has_value()) {
        r[PncDrive::CAPACITY_BYTES] =
            double(endpoint::utils::gb_to_b(drive.get_capacity_gb().value()));
    }

    endpoint::status_to_json(drive, r);
    r[Common::STATUS][Common::HEALTH_ROLLUP] = drive.get_status().get_health();

    set_response(res, r);
}


void endpoint::Drive::patch(const server::Request& request, server::Response& response) {

    auto drive = psme::rest::model::Find<agent_framework::model::Drive>(request.params[PathParam::DRIVE_ID])
        .via<agent_framework::model::Chassis>(request.params[PathParam::CHASSIS_ID]).get();
    agent_framework::model::attribute::Attributes attributes{};

    const auto& json = JsonValidator::validate_request_body<schema::DrivePatchSchema>(request);

    if (json[Common::ASSET_TAG].is_string()) {
        attributes.set_value(agent_framework::model::literals::Drive::ASSET_TAG,
                             json[Common::ASSET_TAG].as_string());
    }

    if (json[Common::OEM][Common::RACKSCALE][PncDrive::DRIVE_ERASED].is_boolean()) {
        bool drive_erased = json[Common::OEM][Common::RACKSCALE][PncDrive::DRIVE_ERASED].as_bool();
        attributes.set_value(agent_framework::model::literals::Drive::ERASED, drive_erased);
    }

    if (!attributes.empty()) {
        agent_framework::model::requests::SetComponentAttributes set_component_attributes_request{drive.get_uuid(),
                                                                                                  attributes};

        const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(drive.get_agent_id());

        auto set_drive_attributes = [&, gami_agent] {
            const auto& set_component_attributes_response = gami_agent->
                execute<agent_framework::model::responses::SetComponentAttributes>(set_component_attributes_request);

            const auto& result_statuses = set_component_attributes_response.get_statuses();
            if (!result_statuses.empty()) {
                const auto& error = error::ErrorFactory::create_error_from_set_component_attributes_results(
                    result_statuses, gami_to_rest_attributes);
                throw error::ServerException(error);
            }

            psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                agent_framework::model::enums::Component::Drive)->
                load(gami_agent, drive.get_parent_uuid(),
                     agent_framework::model::enums::Component::Chassis, drive.get_uuid(), false);
        };

        gami_agent->execute_in_transaction(set_drive_attributes);
    }
    get(request, response);
}
