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
#include "agent-framework/module-ref/constants/storage.hpp"
#include "agent-framework/module-ref/model/iscsi_target.hpp"

#include "psme/rest-ref/utils/json_validator.hpp"
#include "psme/rest-ref/endpoints/remote_target.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/rest-ref/model/handlers/generic_handler_deps.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"
#include "psme/core/agent/agent_manager.hpp"

using namespace psme::rest;
using namespace psme::rest::error;
using namespace psme::rest::utils;
using namespace app::rest::constants;
using namespace app::rest;
using namespace agent_framework::model;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Services/1/RemoteTargets/Links/Members/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#RemoteTarget.1.0.0.RemoteTarget";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "iSCSI Remote Target";
    r[Common::DESCRIPTION] = "iSCSI Remote Target description";
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;
    r[RemoteTarget::TYPE] = json::Value::Type::NIL;

    r[RemoteTarget::ADDRESSES] = json::Value::Type::ARRAY;
    r[RemoteTarget::INITIATOR] = json::Value::Type::ARRAY;
    r[Common::LINKS] = json::Value::Type::OBJECT;
    r[Common::OEM] = json::Value::Type::OBJECT;
    return r;
}

json::Value validate_patch_request(const server::Request& request) {
    using json::Pair;

    json::Value schema{
        Pair(RemoteTarget::ADDRESSES,
            JsonValidator::type_of(JsonValidator::ARRAY_TYPE, false)),
        Pair(RemoteTarget::INITIATOR,
            JsonValidator::type_of(JsonValidator::ARRAY_TYPE, false)),
        Pair(Common::OEM,
            JsonValidator::type_of(JsonValidator::OBJECT_TYPE, false))
    };

    return JsonValidator::validate_request_body(request, schema);
}

}

endpoint::RemoteTarget::RemoteTarget(const std::string& path) : Endpoint(path) {}
endpoint::RemoteTarget::~RemoteTarget() {}

void endpoint::RemoteTarget::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();

    r[constants::Common::ODATA_ID] = PathBuilder(req).build();
    r[constants::Common::ID] = req.params[PathParam::REMOTE_TARGET_ID];

    auto rt =
        psme::rest::model::Find<agent_framework::model::IscsiTarget>(req.params[PathParam::REMOTE_TARGET_ID]).
        via<agent_framework::model::StorageServices>(req.params[PathParam::SERVICE_ID]).get();

    endpoint::utils::status_to_json(rt, r);
    r[Common::STATUS][Common::HEALTH_ROLLUP] = endpoint::utils::rollup_remote_target_health(rt.get_uuid()).to_string();


    // iSCSI field in Addresses object
    json::Value iscsi;
    iscsi[constants::RemoteTarget::TARGET_PORTAL_PORT] = std::stoi(rt.get_target_port());
    iscsi[constants::RemoteTarget::TARGET_PORTAL_IP] = rt.get_target_address();
    iscsi[constants::RemoteTarget::TARGET_IQN] = rt.get_target_iqn();

    iscsi[constants::RemoteTarget::TARGET_LUN] = json::Value::Type::ARRAY;
    for(const auto& target_lun: rt.get_target_lun()){
        json::Value lun;
        lun[constants::RemoteTarget::LUN] = target_lun.get_lun();
        try {
            // Find drive by UUID and get it's id to build link path.
            auto drive_id = agent_framework::module::StorageManager::get_instance()
                ->get_logical_drive_manager()
                .get_entry(target_lun.get_logical_drive()).get_id();

            // fill the link
            lun[constants::RemoteTarget::LOGICAL_DRIVE]
                [constants::Common::ODATA_ID] =
                    endpoint::PathBuilder(PathParam::BASE_URL)
                                    .append(Root::SERVICES)
                                    .append(req.params[PathParam::SERVICE_ID])
                                    .append(StorageService::LOGICAL_DRIVES)
                                    .append(drive_id).build();
            iscsi[constants::RemoteTarget::TARGET_LUN].push_back(std::move(lun));
        }
        catch (agent_framework::exceptions::InvalidUuid&) {
            log_error(GET_LOGGER("rest"), "Model iSCSITarget/LogicalDrive link error");
            log_error(GET_LOGGER("rest"), "Logical Drive "
                << target_lun.get_logical_drive() << ", used by "
                << " an iSCSITarget (" << rt.get_uuid() << "), does not exist");
        }
    }

    // Addresses array
    json::Value address;
    address[constants::RemoteTarget::ISCSI] = std::move(iscsi);
    r[constants::RemoteTarget::ADDRESSES].push_back(std::move(address));

    // Initiator array
    json::Value initiator;
    initiator[constants::RemoteTarget::ISCSI][constants::RemoteTarget::INITIATOR_IQN] =
        rt.get_initiator_iqn();
    r[constants::RemoteTarget::INITIATOR].push_back(initiator);

    set_response(res, r);
}

void endpoint::RemoteTarget::patch(const server::Request& request, server::Response& response) {
    auto rt =
        psme::rest::model::Find<agent_framework::model::IscsiTarget>(request.params[PathParam::REMOTE_TARGET_ID]).
        via<agent_framework::model::StorageServices>(request.params[PathParam::SERVICE_ID]).get_one();

    std::string agent_id{rt->get_agent_id()};
    std::string target_uuid{rt->get_uuid()};

    auto json = validate_patch_request(request);

    agent_framework::model::attribute::Attributes attributes{};

    if (json.is_member(constants::RemoteTarget::ADDRESSES)) {
        const auto& addresses = json[constants::RemoteTarget::ADDRESSES].as_array();
        if (addresses.size() > 1) {
            THROW(agent_framework::exceptions::InvalidParameters, "rest",
                  "Unable to patch the target's IQN more than once in a single request");
        } else if (addresses.size() == 1) {
            const auto& address = addresses.front();
            if (address[constants::RemoteTarget::ISCSI].is_object()) {
                const auto& iscsi = address[constants::RemoteTarget::ISCSI];
                if (iscsi[constants::RemoteTarget::TARGET_IQN].is_string()) {
                    attributes.set_value(
                        agent_framework::model::literals::IscsiTarget::TARGET_IQN,
                        iscsi[constants::RemoteTarget::TARGET_IQN].as_string()
                    );
                }
            }
        }
    }

    if (json.is_member(constants::RemoteTarget::INITIATOR)) {
        const auto& initiators = json[constants::RemoteTarget::INITIATOR].as_array();
        if (initiators.size() > 1) {
            THROW(agent_framework::exceptions::InvalidParameters, "rest",
                  "Unable to patch the target's Initiator IQN more than once in a single request");
        } else if (initiators.size() == 1) {
            const auto& initiator = initiators.front();
            if (initiator[constants::RemoteTarget::ISCSI].is_object()) {
                const auto& iscsi = initiator[constants::RemoteTarget::ISCSI];
                if (iscsi[constants::RemoteTarget::INITIATOR_IQN].is_string()) {
                    attributes.set_value(
                        agent_framework::model::literals::IscsiTarget::INITIATOR_IQN,
                        iscsi[constants::RemoteTarget::INITIATOR_IQN].as_string()
                    );
                }
            }
        }
    }

    agent_framework::model::requests::SetComponentAttributes
    set_component_attributes_request{target_uuid, attributes};

    // Call set component attribute method
    const auto& manager = psme::core::agent::AgentManager::get_instance();
    manager->call_method
        <agent_framework::model::responses::SetComponentAttributes>(
            agent_id, set_component_attributes_request);

    auto parent_uuid = rt->get_parent_uuid();

    psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::IscsiTarget)->
        load(manager->get_agent(rt->get_agent_id()), parent_uuid,
             rt->get_uuid(), false);

    response.set_status(server::status_2XX::NO_CONTENT);
}


void endpoint::RemoteTarget::del(const server::Request& req, server::Response& res) {

    using AgentManager = psme::core::agent::AgentManager;
    using HandlerManager = psme::rest::model::handler::HandlerManager;

    auto target = psme::rest::model::Find
        <agent_framework::model::IscsiTarget>(req.params[PathParam::REMOTE_TARGET_ID]).via
        <agent_framework::model::StorageServices>(req.params[PathParam::SERVICE_ID]).
        get_one();

    auto gami_req = agent_framework::model::requests::DeleteIscsiTarget(
        target->get_uuid());

    // try removing target from agent's model
    AgentManager::get_instance()->call_method
        <agent_framework::model::responses::DeleteIscsiTarget>
        (target->get_agent_id(), gami_req);

    // remove target from REST model, DO NOT use target reference after this line!
    HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::IscsiTarget)->
        remove(target->get_uuid());

    res.set_status(server::status_2XX::NO_CONTENT);
}
