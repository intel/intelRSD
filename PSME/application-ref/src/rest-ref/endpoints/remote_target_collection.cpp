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

#include "agent-framework/module-ref/requests/storage.hpp"
#include "agent-framework/module-ref/responses/storage.hpp"

#include "psme/rest-ref/utils/mapper.hpp"
#include "psme/rest-ref/utils/json_validator.hpp"
#include "psme/rest-ref/model/handlers/handler_manager.hpp"
#include "psme/rest-ref/model/handlers/generic_handler_deps.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"
#include "psme/rest-ref/endpoints/utils.hpp"
#include "psme/rest-ref/endpoints/remote_target_collection.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/core/agent/agent_manager.hpp"

using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::error;
using namespace psme::rest::utils;
using namespace app::rest::constants;
using namespace agent_framework::module;



namespace {
using TargetLuns = attribute::Array<attribute::TargetLun>;

json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Services/1/RemoteTargets";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#RemoteTargetCollection.RemoteTargetCollection";
    r[Common::NAME] = "Remote Targets Collection";
    r[Common::DESCRIPTION] = "Collection of Remote Targets";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}

json::Value validate_post_request(const server::Request& request) {
    using json::Pair;
    json::Value schema({
        Pair(Common::NAME,
            JsonValidator::type_of(JsonValidator::STRING_TYPE)),
        Pair(RemoteTarget::TYPE,
            JsonValidator::type_of(JsonValidator::STRING_TYPE)),
        Pair(RemoteTarget::ADDRESSES,
            JsonValidator::type_of(JsonValidator::ARRAY_TYPE)),
        Pair(RemoteTarget::INITIATOR,
            JsonValidator::type_of(JsonValidator::ARRAY_TYPE))
    });

    return JsonValidator::validate_request_body(request, schema);
}

TargetLuns process_luns(const json::Value& json) {
    TargetLuns target_luns{};
    std::string invalid_drive{};

    for (const auto& lun : json) {
        const auto& logical_drive_url =
            lun[RemoteTarget::LOGICAL_DRIVE][Common::ODATA_ID].as_string();

        try {
            const auto& params =
                    psme::rest::model::Mapper::get_params(logical_drive_url);
            auto drive = psme::rest::model::Find
                <agent_framework::model::LogicalDrive>(params[PathParam::LOGICAL_DRIVE_ID])
                .via<agent_framework::model::StorageServices>(params[PathParam::SERVICE_ID])
                .get_one();

            if(drive->get_protected()) {
                invalid_drive = logical_drive_url;
                break;
            }

            target_luns.add_entry({lun[RemoteTarget::LUN].as_uint(),
                    drive->get_uuid()});
        }
        catch (agent_framework::exceptions::NotFound&) {
            throw(agent_framework::exceptions::InvalidParameters("Invalid drive ID: " + logical_drive_url));

        }
    }
    if (!invalid_drive.empty()) {
        throw(agent_framework::exceptions::InvalidParameters("Cannot create target on a protected "
                                                                 "logical drive: " + invalid_drive));

    }
    return target_luns;
}

agent_framework::model::requests::AddIscsiTarget
create_add_iscsi_target_request(const json::Value& json) {
    std::string target_iqn{};
    std::string initiator_iqn{};
    TargetLuns target_luns{};

    try {
        for (const auto& address : json[RemoteTarget::ADDRESSES].as_array()) {
            if (address[RemoteTarget::ISCSI].is_object()) {
                target_iqn = address[RemoteTarget::ISCSI][RemoteTarget::TARGET_IQN].as_string();
                target_luns = process_luns(address[RemoteTarget::ISCSI][RemoteTarget::TARGET_LUN]);
            }
        }

        for (const auto& initiator : json[RemoteTarget::INITIATOR].as_array()) {
            if (initiator[RemoteTarget::ISCSI].is_object()) {
                initiator_iqn = initiator[RemoteTarget::ISCSI]
                                [RemoteTarget::INITIATOR_IQN].as_string();
            }
        }
    }
    catch (const json::Value::Exception&) {
        throw(agent_framework::exceptions::InvalidParameters("Missing or invalid fields in json POST request"));
    }

    agent_framework::model::requests::AddIscsiTarget request{
        initiator_iqn, target_iqn, attribute::Oem{}};
        request.set_target_luns(target_luns);
    return request;
}

bool validate_if_lun_is_lv(const json::Value& json) {
    for(const auto& address : json[RemoteTarget::ADDRESSES].as_array()) {
        if (address[RemoteTarget::ISCSI].is_object()) {
            for(const auto& lun : address[RemoteTarget::ISCSI]
                [RemoteTarget::TARGET_LUN].as_array()) {
                auto params = psme::rest::model::Mapper::get_params(
                    lun[RemoteTarget::LOGICAL_DRIVE]
                        [Common::ODATA_ID].as_string());
                auto drive = psme::rest::model::Find
                    <agent_framework::model::LogicalDrive>
                    (params[PathParam::LOGICAL_DRIVE_ID]).via
                    <agent_framework::model::StorageServices>
                    (params[PathParam::SERVICE_ID]).get_one();
                if(drive->get_mode() != enums::LogicalDriveMode::LV) {
                    return false;
                }
            }
        }
    }
    return true;
}

}

RemoteTargetCollection::RemoteTargetCollection(const std::string& path) : Endpoint(path) {}
RemoteTargetCollection::~RemoteTargetCollection() {}

void RemoteTargetCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto service_uuid = psme::rest::model::Find<agent_framework::model::StorageServices>(req.params[PathParam::SERVICE_ID]).get_one()->get_uuid();

    auto keys = StorageManager::get_instance()->
                        get_iscsi_target_manager().get_ids(service_uuid);

    json[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(keys.size());
    for (const auto& key : keys) {
        json::Value link_elem(json::Value::Type::OBJECT);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(key).build();
        json[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(res, json);
}

void RemoteTargetCollection::post(const server::Request& request, server::Response& response) {
    using HandlerManager = psme::rest::model::handler::HandlerManager;

    auto service = psme::rest::model::Find
                            <agent_framework::model::StorageServices>(
                                request.params[PathParam::SERVICE_ID]).get_one();

    // Gets agent ID from parent resource
    std::string agent_id{service->get_agent_id()};

    const auto& json = validate_post_request(request);
    const auto& add_request = create_add_iscsi_target_request(json);

    if(!validate_if_lun_is_lv(json)) {
        THROW(agent_framework::exceptions::InvalidParameters, "rest",
            "Cannot create a target on a LogicalDrive with a mode other than "
            "LogicalVolume");
    }

    // Call set component attribute method
    const auto& manager = psme::core::agent::AgentManager::get_instance();
    const auto& add_response = manager->call_method
        <agent_framework::model::responses::AddIscsiTarget>(
            agent_id, add_request);

    HandlerManager::get_instance()->
        get_handler(agent_framework::model::enums::Component::IscsiTarget)->
        load(manager->get_agent(service->get_agent_id()),
            service->get_uuid(), add_response.get_target(), true);

    auto created_target =
        agent_framework::module::StorageManager::get_instance()->
            get_iscsi_target_manager().get_entry_reference(
                    add_response.get_target());

    ::psme::rest::endpoint::utils::set_location_header(
        response, request, request.get_url() + "/" +
            std::to_string(created_target->get_id()));

    response.set_status(server::status_2XX::CREATED);
}
