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


#include "psme/rest/endpoints/chassis/drive.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/drive.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/responses/common.hpp"
#include "agent-framework/module/requests/common/delete_drive.hpp"
#include "agent-framework/module/responses/common/delete_drive.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::endpoint::utils;
using namespace psme::rest::validators;

namespace {

json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Drive.Drive";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Drive.v1_2_0.Drive";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Drive";
    r[Common::DESCRIPTION] = "Drive description";

    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;

    r[Common::ACTIONS] = json::Json::value_t::object;
    r[Common::ASSET_TAG] = json::Json::value_t::null;
    r[Drive::INDICATOR_LED] = json::Json::value_t::null;

    r[Common::LINKS][Common::ODATA_TYPE] = "#Drive.v1_2_0.Links";
    r[Common::LINKS][Drive::ENDPOINTS] = json::Json::value_t::array;
    r[Common::LINKS][Drive::VOLUMES] = json::Json::value_t::array;
    r[Common::LINKS][Common::CHASSIS] = json::Json::value_t::object;
    r[Common::LINKS][Common::OEM] = json::Json::value_t::object;

    r[Common::LOCATION] = json::Json::value_t::array;
    r[Common::IDENTIFIERS] = json::Json::value_t::array;

    r[Common::MANUFACTURER] = json::Json::value_t::null;
    r[Drive::MEDIA_TYPE] = json::Json::value_t::null;
    r[Common::MODEL] = json::Json::value_t::null;
    r[Drive::CAPABLE_SPEED] = json::Json::value_t::null;
    r[Drive::NEGOTIATED_SPEED] = json::Json::value_t::null;
    r[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.Drive";
    r[Common::OEM][Common::RACKSCALE][Drive::DRIVE_ERASED] = json::Json::value_t::null;
    r[Common::OEM][Common::RACKSCALE][Drive::FIRMWARE_VERSION] = json::Json::value_t::null;
    r[Common::OEM][Common::RACKSCALE][System::STORAGE] = json::Json::value_t::null;
    r[Common::OEM][Common::RACKSCALE][Drive::USED_BY] = json::Json::value_t::array;
    r[Common::OEM][Common::RACKSCALE][Drive::PCIE_FUNCTION] = json::Json::value_t::null;
    r[Common::OEM][Common::RACKSCALE][Common::METRICS] = json::Json::value_t::null;
    r[Common::PART_NUMBER] = json::Json::value_t::null;
    r[Drive::PROTOCOL] = json::Json::value_t::null;
    r[Common::SERIAL_NUMBER] = json::Json::value_t::null;
    r[Drive::CAPACITY_BYTES] = json::Json::value_t::null;

    r[Common::SKU] = json::Json::value_t::null;
    r[Drive::STATUS_INDICATOR] = json::Json::value_t::null;
    r[Drive::REVISION] = json::Json::value_t::null;
    r[Drive::FAILURE_PREDICTED] = json::Json::value_t::null;
    r[Common::ASSET_TAG] = json::Json::value_t::null;
    r[Drive::HOTSPARE_TYPE] = json::Json::value_t::null;
    r[Drive::ENCRYPTION_ABILITY] = json::Json::value_t::null;
    r[Drive::ENCRYPTION_STATUS] = json::Json::value_t::null;
    r[Drive::ROTATION_SPEED_RPM] = json::Json::value_t::null;
    r[Drive::BLOCK_SIZE_BYTES] = json::Json::value_t::null;
    r[Drive::PREDICTED_MEDIA_LIFE_LEFT] = json::Json::value_t::null;

    return r;
}


void add_links(const agent_framework::model::Drive& drive, json::Json& json) {

    // Relevant only for PNC Agent
    if (has_resource_capability(drive, Capability::PNC) &&
        drive.get_interface() == agent_framework::model::enums::TransportProtocol::NVMe) {

        // fill Storage link
        const auto storage_uuids = agent_framework::module::get_m2m_manager<agent_framework::model::StorageSubsystem,
            agent_framework::model::Drive>().get_parents(drive.get_uuid());
        if (storage_uuids.size() > 1) {
            log_warning("rest", "Drive " + drive.get_uuid() + " is assigned to more than one storage subsystem!");
        }
        else if (storage_uuids.size() == 0) {
            log_warning("rest", "Drive " + drive.get_uuid() + " is not assigned to any storage subsystems!");
        }
        else {
            const auto storage_uuid = storage_uuids.front();
            try {
                json[Common::OEM][Common::RACKSCALE][System::STORAGE][Common::ODATA_ID] = psme::rest::endpoint::utils::get_component_url(
                    agent_framework::model::enums::Component::StorageSubsystem, storage_uuid);
            }
            catch (agent_framework::exceptions::InvalidUuid&) {
                log_error("rest", "Drive " + drive.get_uuid() + " is assigned to a non existent storage subsystem!");
            }
        }

        // fill PCIeFunction link
        for (const auto& function_uuid : agent_framework::module::get_m2m_manager<agent_framework::model::Drive,
            agent_framework::model::PcieFunction>().get_children(
            drive.get_uuid())) {
            try {
                json[Common::OEM][Common::RACKSCALE][Drive::PCIE_FUNCTION][Common::ODATA_ID] =
                    psme::rest::endpoint::utils::get_component_url(
                        agent_framework::model::enums::Component::PcieFunction, function_uuid);
            }
            catch (agent_framework::exceptions::InvalidUuid&) {
                log_error("rest", "Drive " + drive.get_uuid() + " has non-existing PCIeFunctions!");
            }
        }
    }

    // fill endpoints links
    auto& manager = agent_framework::module::get_manager<agent_framework::model::Endpoint>();
    for (const auto& endpoint_uuid : manager.get_keys()) {
        if (agent_framework::model::Endpoint::has_entity(endpoint_uuid, drive.get_uuid())) {
            json::Json endpoint_link = json::Json();
            endpoint_link[Common::ODATA_ID] = psme::rest::endpoint::utils::get_component_url(
                agent_framework::model::enums::Component::Endpoint, endpoint_uuid);
            json[Common::LINKS][Drive::ENDPOINTS].push_back(std::move(endpoint_link));
        }
    }

    // fill UsedBy links
    for (const auto& pool : agent_framework::module::get_manager<agent_framework::model::StoragePool>().get_entries()) {
        for (const auto& source : pool.get_capacity_sources()) {
            for (const auto& drive_uuid : source.get_providing_drives()) {
                if (drive.get_uuid() == drive_uuid) {
                    json::Json used_link = json::Json();
                    try {
                        used_link[Common::ODATA_ID] = psme::rest::endpoint::utils::get_component_url(
                            agent_framework::model::enums::Component::StoragePool, pool.get_uuid());
                        json[Common::OEM][Common::RACKSCALE][Drive::USED_BY].push_back(std::move(used_link));
                    }
                    catch (agent_framework::exceptions::InvalidUuid&) {
                        log_error("rest", "Drive " + drive.get_uuid() + " is used by a non existent storage pool!");
                    }
                }
            }
        }
    }
}


static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::Drive::ASSET_TAG, constants::Common::ASSET_TAG},
    {agent_framework::model::literals::Drive::ERASED,    endpoint::PathBuilder(Common::OEM)
                                                             .append(Common::RACKSCALE)
                                                             .append(constants::Drive::DRIVE_ERASED)
                                                             .build()
    }
};

}


endpoint::Drive::Drive(const std::string& path) : EndpointBase(path) {}


endpoint::Drive::~Drive() {}


void endpoint::Drive::get(const server::Request& req, server::Response& res) {
    auto r = make_prototype();

    r[Common::ODATA_ID] = PathBuilder(req).build();

    auto drive = psme::rest::model::find<agent_framework::model::Chassis, agent_framework::model::Drive>(
        req.params).get();

    r[Common::ID] = req.params[PathParam::DRIVE_ID];

    if (drive.get_interface() == agent_framework::model::enums::TransportProtocol::NVMe) {
        r[Common::ACTIONS][constants::Drive::SECURE_ERASE][Common::TARGET] = endpoint::PathBuilder(req)
            .append(Common::ACTIONS)
            .append(constants::Drive::SECURE_ERASE_ENDPOINT)
            .build();
    }

    add_links(drive, r);
    fill_name_and_description(drive, r);

    const auto& fru = drive.get_fru_info();
    r[Common::MANUFACTURER] = fru.get_manufacturer();
    r[Common::MODEL] = fru.get_model_number();
    r[Common::PART_NUMBER] = fru.get_part_number();
    r[Common::SERIAL_NUMBER] = fru.get_serial_number();

    r[Common::ASSET_TAG] = drive.get_asset_tag();
    r[constants::Drive::MEDIA_TYPE] = drive.get_type();
    r[constants::Drive::CAPABLE_SPEED] = drive.get_capable_speed_gbs();
    r[constants::Drive::PROTOCOL] = drive.get_interface();
    r[constants::Drive::NEGOTIATED_SPEED] = drive.get_negotiated_speed_gbs();

    r[Common::SKU] = drive.get_sku();
    r[constants::Drive::STATUS_INDICATOR] = drive.get_status_indicator();
    r[constants::Drive::REVISION] = drive.get_revision();
    r[constants::Drive::FAILURE_PREDICTED] = drive.get_failure_predicted();
    r[constants::Drive::HOTSPARE_TYPE] = drive.get_hotspare_type();
    r[constants::Drive::ENCRYPTION_ABILITY] = drive.get_encryption_ability();
    r[constants::Drive::ENCRYPTION_STATUS] = drive.get_encryption_status();
    r[constants::Drive::ROTATION_SPEED_RPM] = drive.get_rpm();
    r[constants::Drive::BLOCK_SIZE_BYTES] = drive.get_block_size_bytes();
    r[constants::Drive::PREDICTED_MEDIA_LIFE_LEFT] = drive.get_predicted_media_life_left();

    r[Common::OEM][Common::RACKSCALE][constants::Drive::DRIVE_ERASED] = drive.get_erased();
    r[Common::OEM][Common::RACKSCALE][constants::Drive::FIRMWARE_VERSION] = drive.get_firmware_version();
    r[Common::OEM][Common::RACKSCALE][constants::Drive::LATENCY_TRACKING_ENABLED] = drive.get_latency_tracking_enabled();

    for (const auto& location : drive.get_locations()) {
        json::Json l = json::Json();
        l[Common::ODATA_TYPE] = "#Resource.v1_1_0.Location";
        l[constants::Drive::INFO] = location.get_info();
        l[constants::Drive::INFO_FORMAT] = location.get_info_format();
        r[Common::LOCATION].push_back(std::move(l));
    }

    for (const auto& identifier : drive.get_identifiers()) {
        json::Json id = json::Json();
        id[Common::ODATA_TYPE] = "#Resource.v1_1_0.Identifier";
        id[Common::DURABLE_NAME] = identifier.get_durable_name();
        id[Common::DURABLE_NAME_FORMAT] = identifier.get_durable_name_format();
        r[Common::IDENTIFIERS].push_back(std::move(id));
    }

    if (drive.get_capacity_gb().has_value()) {
        r[constants::Drive::CAPACITY_BYTES] = std::int64_t(endpoint::utils::gb_to_b(drive.get_capacity_gb().value()));
    }

    r[Common::LINKS][Common::CHASSIS][Common::ODATA_ID] = endpoint::PathBuilder(PathParam::BASE_URL)
        .append(constants::Common::CHASSIS)
        .append(req.params[PathParam::CHASSIS_ID])
        .build();

    endpoint::status_to_json(drive, r);
    r[Common::STATUS][Common::HEALTH_ROLLUP] = drive.get_status().get_health();
    r[Common::OEM][Common::RACKSCALE][Common::METRICS][Common::ODATA_ID] = PathBuilder(req).append(
        constants::Common::METRICS).build();

    set_response(res, r);
}


void endpoint::Drive::patch(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PatchDrive";

    auto drive = psme::rest::model::find<agent_framework::model::Chassis, agent_framework::model::Drive>(
        request.params).get();
    agent_framework::model::attribute::Attributes attributes{};

    const auto& json = JsonValidator::validate_request_body<schema::DrivePatchSchema>(request);

    if (json.value(Common::ASSET_TAG, json::Json()).is_string()) {
        attributes.set_value(agent_framework::model::literals::Drive::ASSET_TAG,
                             json[Common::ASSET_TAG].get<std::string>());
    }

    json::Json js_rackscale = json.value(Common::OEM, json::Json::object()).value(Common::RACKSCALE, json::Json::object());

    if (js_rackscale.value(constants::Drive::DRIVE_ERASED, json::Json()).is_boolean()) {
        bool drive_erased = json[Common::OEM][Common::RACKSCALE][constants::Drive::DRIVE_ERASED];
        attributes.set_value(agent_framework::model::literals::Drive::ERASED, drive_erased);
    }

    if (js_rackscale.value(constants::Drive::LATENCY_TRACKING_ENABLED, json::Json()).is_boolean()) {
        bool latency_tracking = json[Common::OEM][Common::RACKSCALE][constants::Drive::LATENCY_TRACKING_ENABLED];
        attributes.set_value(agent_framework::model::literals::Drive::LATENCY_TRACKING_ENABLED, latency_tracking);
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

        gami_agent->execute_in_transaction(TRANSACTION_NAME, set_drive_attributes);
    }
    get(request, response);
}


void endpoint::Drive::del(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "DeleteDrive";

    const auto drive = psme::rest::model::find<agent_framework::model::Chassis, agent_framework::model::Drive>(
        request.params).get();

    auto gami_req = agent_framework::model::requests::DeleteDrive(drive.get_uuid());

    const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(drive.get_agent_id());

    auto remove_drive = [&, gami_agent] {
        // try removing drive from agent's model
        gami_agent->execute<agent_framework::model::responses::DeleteDrive>(gami_req);

        // remove drive from REST model, DO NOT use drive reference after this line!
        psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
            agent_framework::model::enums::Component::Drive)->
            remove(drive.get_uuid());

        response.set_status(server::status_2XX::NO_CONTENT);
    };

    gami_agent->execute_in_transaction(TRANSACTION_NAME, remove_drive);

}
