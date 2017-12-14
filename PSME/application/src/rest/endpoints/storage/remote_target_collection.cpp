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

#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/requests/storage.hpp"
#include "agent-framework/module/responses/storage.hpp"

#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/server/multiplexer.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/remote_targets_collection.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/endpoints/endpoints.hpp"


using namespace psme::rest;
using namespace psme::rest::error;
using namespace psme::rest::validators;
using namespace psme::rest::constants;
using namespace agent_framework::module;
using namespace agent_framework::model;

namespace {
using TargetLuns = attribute::Array<attribute::TargetLun>;


json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#RemoteTargetCollection.RemoteTargetCollection";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#RemoteTargetCollection.RemoteTargetCollection";
    r[Common::NAME] = "Remote Targets Collection";
    r[Common::DESCRIPTION] = "Collection of Remote Targets";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}


TargetLuns process_luns(const json::Value& json) {
    TargetLuns target_luns{};

    for (const auto& lun : json) {
        const auto& logical_drive_url = lun[RemoteTarget::LOGICAL_DRIVE][Common::ODATA_ID].as_string();

        try {
            auto params = server::Multiplexer::get_instance()->
                get_params(logical_drive_url, constants::Routes::LOGICAL_DRIVE_PATH);
            auto drive = psme::rest::model::Find<agent_framework::model::LogicalDrive>(
                params[PathParam::LOGICAL_DRIVE_ID])
                .via<agent_framework::model::StorageService>(params[PathParam::SERVICE_ID])
                .get_one();

            if (drive->get_mode() != enums::LogicalDriveMode::LV) {
                THROW(agent_framework::exceptions::InvalidValue, "rest",
                      "Cannot create a target on a LogicalDrive with a mode other than LogicalVolume.");
            }

            if (drive->get_protected()) {
                throw agent_framework::exceptions::InvalidValue(
                    "Cannot create target on a protected logical drive: " + logical_drive_url);
            }

            target_luns.add_entry({lun[RemoteTarget::LUN].as_uint(), drive->get_uuid()});
        }
        catch (agent_framework::exceptions::NotFound&) {
            throw agent_framework::exceptions::InvalidValue("Invalid logical drive URL: " + logical_drive_url);
        }
    }

    return target_luns;
}


agent_framework::model::requests::AddIscsiTarget create_add_iscsi_target_request(const json::Value& json) {
    using agent_framework::model::requests::AddIscsiTarget;
    AddIscsiTarget request{};

    const auto& addresses = json[RemoteTarget::ADDRESSES].as_array();

    constexpr const size_t EXPECTED_TARGET_IQNS = 1;
    if (addresses.size() != EXPECTED_TARGET_IQNS) {
        throw agent_framework::exceptions::InvalidValue(
            "Exactly one TargetIQN must be specified.");
    }
    const auto& iscsi = addresses.front()[RemoteTarget::ISCSI];

    request.set_target_iqn(iscsi[RemoteTarget::TARGET_IQN].as_string());

    request.set_target_luns(process_luns(iscsi[RemoteTarget::TARGET_LUN]));

    if (iscsi.is_member(RemoteTarget::CHAP) && !iscsi[RemoteTarget::CHAP].is_null()) {
        const auto& chap = iscsi[RemoteTarget::CHAP];
        if (chap.is_member(RemoteTarget::TYPE) && !chap[RemoteTarget::TYPE].is_null()) {
            request.set_authentication_method(
                enums::TargetAuthenticationMethod::from_string(chap[RemoteTarget::TYPE].as_string()));
        }

        request.set_chap_username(chap[RemoteTarget::USERNAME]);
        request.set_chap_secret(chap[RemoteTarget::SECRET]);
        request.set_mutual_chap_username(chap[RemoteTarget::MUTUAL_USERNAME]);
        request.set_mutual_chap_secret(chap[RemoteTarget::MUTUAL_SECRET]);
    }

    const auto& initiators = json[RemoteTarget::INITIATOR].as_array();

    constexpr const size_t MAX_INITIATOR_IQNS = 1;
    if (initiators.size() > MAX_INITIATOR_IQNS) {
        throw agent_framework::exceptions::InvalidValue(
            "Creating remote targets with more than one InitiatorIQN is not supported");
    }
    for (const auto& initiator : initiators) {
        // empty InitiatorIQN is represented as null, which means the same on tgt level, but is compliant with metadata
        auto initiator_iqn = initiator[RemoteTarget::ISCSI][RemoteTarget::INITIATOR_IQN].as_string();
        if (!initiator_iqn.empty()) {
            request.set_initiator_iqn(initiator_iqn);
        }
    }

    return request;
}
}


endpoint::RemoteTargetCollection::RemoteTargetCollection(const std::string& path) : EndpointBase(path) {}


endpoint::RemoteTargetCollection::~RemoteTargetCollection() {}


void endpoint::RemoteTargetCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto service_uuid =
        psme::rest::model::Find<agent_framework::model::StorageService>(req.params[PathParam::SERVICE_ID]).get_uuid();

    auto keys = StorageComponents::get_instance()->
        get_iscsi_target_manager().get_ids(service_uuid);

    json[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(keys.size());
    for (const auto& key : keys) {
        json::Value link_elem(json::Value::Type::OBJECT);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(key).build();
        json[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(res, json);
}


void endpoint::RemoteTargetCollection::post(const server::Request& request, server::Response& response) {
    using HandlerManager = psme::rest::model::handler::HandlerManager;

    auto service =
        psme::rest::model::Find<agent_framework::model::StorageService>(request.params[PathParam::SERVICE_ID]).get();

    // Gets agent ID from parent resource
    std::string agent_id{service.get_agent_id()};

    auto json = JsonValidator::validate_request_body<schema::RemoteTargetsCollectionPostSchema>(request);
    const auto& add_request = create_add_iscsi_target_request(json);

    const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(agent_id);

    auto add_target = [&, gami_agent] {
        // Call set add target method
        const auto& add_response = gami_agent->execute<agent_framework::model::responses::AddIscsiTarget>(
            add_request);

        HandlerManager::get_instance()->
            get_handler(agent_framework::model::enums::Component::IscsiTarget)->
            load(gami_agent,
                 service.get_uuid(), agent_framework::model::enums::Component::StorageService,
                 add_response.get_target(), true);

        auto created_target = agent_framework::module::StorageComponents::get_instance()->
            get_iscsi_target_manager().get_entry_reference(add_response.get_target());

        psme::rest::endpoint::utils::set_location_header(
            response, request.get_url() + "/" + std::to_string(created_target->get_id()));

        response.set_status(server::status_2XX::CREATED);
    };

    gami_agent->execute_in_transaction(add_target);
}
