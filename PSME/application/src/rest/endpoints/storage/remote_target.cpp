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

#include "agent-framework/module/requests/storage.hpp"
#include "agent-framework/module/responses/storage.hpp"

#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/remote_target.hpp"

#include "psme/rest/endpoints/storage/remote_target.hpp"
#include "psme/rest/server/error/error_factory.hpp"

#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"



using namespace psme::rest;
using namespace psme::rest::error;
using namespace psme::rest::validators;
using namespace psme::rest::constants;
using namespace agent_framework::model;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Services/1/RemoteTargets/Links/Members/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#RemoteTarget.v1_1_0.RemoteTarget";
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


static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {literals::IscsiTarget::INITIATOR_IQN,
        endpoint::PathBuilder(constants::RemoteTarget::INITIATOR)
            .append(constants::RemoteTarget::ISCSI)
            .append(constants::RemoteTarget::INITIATOR_IQN).build()},
    {literals::IscsiTarget::AUTHENTICATION_METHOD,
            endpoint::PathBuilder(constants::RemoteTarget::ADDRESSES)
                    .append(constants::RemoteTarget::ISCSI)
                    .append(constants::RemoteTarget::CHAP)
                    .append(constants::RemoteTarget::TYPE).build()},
    {literals::IscsiTarget::CHAP_USERNAME,
            endpoint::PathBuilder(constants::RemoteTarget::ADDRESSES)
                    .append(constants::RemoteTarget::ISCSI)
                    .append(constants::RemoteTarget::CHAP)
                    .append(constants::RemoteTarget::USERNAME).build()},
    {literals::IscsiTarget::CHAP_SECRET,
            endpoint::PathBuilder(constants::RemoteTarget::ADDRESSES)
                    .append(constants::RemoteTarget::ISCSI)
                    .append(constants::RemoteTarget::CHAP)
                    .append(constants::RemoteTarget::SECRET).build()},
    {literals::IscsiTarget::MUTUAL_CHAP_USERNAME,
            endpoint::PathBuilder(constants::RemoteTarget::ADDRESSES)
                    .append(constants::RemoteTarget::ISCSI)
                    .append(constants::RemoteTarget::CHAP)
                    .append(constants::RemoteTarget::MUTUAL_USERNAME).build()},
    {literals::IscsiTarget::MUTUAL_CHAP_SECRET,
            endpoint::PathBuilder(constants::RemoteTarget::ADDRESSES)
                    .append(constants::RemoteTarget::ISCSI)
                    .append(constants::RemoteTarget::CHAP)
                    .append(constants::RemoteTarget::MUTUAL_SECRET).build()}
};

}


endpoint::RemoteTarget::RemoteTarget(const std::string& path) : EndpointBase(path) {}


endpoint::RemoteTarget::~RemoteTarget() {}


void endpoint::RemoteTarget::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();

    r[constants::Common::ODATA_ID] = PathBuilder(req).build();
    r[constants::Common::ID] = req.params[PathParam::REMOTE_TARGET_ID];

    auto rt =
        psme::rest::model::Find<agent_framework::model::IscsiTarget>(req.params[PathParam::REMOTE_TARGET_ID])
            .via<agent_framework::model::StorageServices>(req.params[PathParam::SERVICE_ID]).get();

    endpoint::status_to_json(rt, r);
    r[Common::STATUS][Common::HEALTH_ROLLUP] = endpoint::HealthRollup<agent_framework::model::IscsiTarget>().get(
        rt.get_uuid());


    // iSCSI field in Addresses object
    json::Value iscsi;
    iscsi[Common::ODATA_TYPE] = "#RemoteTarget.v1_1_0.ISCSIAddress";
    iscsi[constants::RemoteTarget::TARGET_PORTAL_PORT] = std::stoi(rt.get_target_port());
    iscsi[constants::RemoteTarget::TARGET_PORTAL_IP] = rt.get_target_address();
    iscsi[constants::RemoteTarget::TARGET_IQN] = rt.get_target_iqn();

    iscsi[constants::RemoteTarget::TARGET_LUN] = json::Value::Type::ARRAY;
    for (const auto& target_lun: rt.get_target_lun()) {
        json::Value lun;
        lun[constants::RemoteTarget::LUN] = target_lun.get_lun();
        try {
            // convert ID into UUID
            auto drive_id = agent_framework::module::StorageComponents::get_instance()->
                get_logical_drive_manager().get_entry(target_lun.get_logical_drive()).get_id();

            // fill the link
            lun[constants::RemoteTarget::LOGICAL_DRIVE][constants::Common::ODATA_ID] =
                endpoint::PathBuilder(PathParam::BASE_URL)
                    .append(Root::SERVICES)
                    .append(req.params[PathParam::SERVICE_ID])
                    .append(StorageService::LOGICAL_DRIVES)
                    .append(drive_id)
                    .build();

            iscsi[constants::RemoteTarget::TARGET_LUN].push_back(std::move(lun));
        }
        catch (const agent_framework::exceptions::InvalidUuid&) {
            log_error(GET_LOGGER("rest"), "Model iSCSITarget/LogicalDrive link error");
            log_error(GET_LOGGER("rest"), "Logical Drive " << target_lun.get_logical_drive() <<
                                                           ", used by an iSCSITarget (" << rt.get_uuid()
                                                           << "), does not exist");
        }
    }

    // CHAP field in iSCSI object
    json::Value chap;
    chap[constants::RemoteTarget::TYPE] = rt.get_authentication_method();
    chap[constants::RemoteTarget::USERNAME] = rt.get_chap_username();
    chap[constants::RemoteTarget::SECRET] = rt.get_chap_secret();
    chap[constants::RemoteTarget::MUTUAL_USERNAME] = rt.get_mutual_chap_username();
    chap[constants::RemoteTarget::MUTUAL_SECRET] = rt.get_mutual_chap_secret();

    iscsi[constants::RemoteTarget::CHAP] = std::move(chap);

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
    auto remote_target =
        psme::rest::model::Find<agent_framework::model::IscsiTarget>(request.params[PathParam::REMOTE_TARGET_ID])
            .via<agent_framework::model::StorageServices>(request.params[PathParam::SERVICE_ID]).get();

    std::string agent_id{remote_target.get_agent_id()};
    std::string target_uuid{remote_target.get_uuid()};
    std::string initiator_iqn{};

    auto json = JsonValidator::validate_request_body<schema::RemoteTargetPatchSchema>(request);
    agent_framework::model::attribute::Attributes attributes{};

    if (json.is_member(constants::RemoteTarget::INITIATOR)) {
        const auto& initiators = json[constants::RemoteTarget::INITIATOR].as_array();
        for (const auto& initiator : initiators) {
            if (initiator[constants::RemoteTarget::ISCSI].is_object()) {
                const auto& iscsi = initiator[constants::RemoteTarget::ISCSI];
                if (iscsi[constants::RemoteTarget::INITIATOR_IQN].is_string()) {
                    initiator_iqn = iscsi[constants::RemoteTarget::INITIATOR_IQN].as_string();
                }
            }
        }
        attributes.set_value(literals::IscsiTarget::INITIATOR_IQN, initiator_iqn);
    }

    if (json.is_member(constants::RemoteTarget::ADDRESSES)) {
        const auto& addresses = json[constants::RemoteTarget::ADDRESSES].as_array();
        for (const auto& address : addresses) {
            if (address[constants::RemoteTarget::ISCSI].is_object()) {
                const auto& iscsi = address[constants::RemoteTarget::ISCSI];
                if (iscsi.is_member(constants::RemoteTarget::CHAP)) {
                    if (iscsi[constants::RemoteTarget::CHAP].is_null()) {
                        attributes.set_value(literals::IscsiTarget::AUTHENTICATION_METHOD, Json::Value::null);
                        attributes.set_value(literals::IscsiTarget::CHAP_USERNAME, Json::Value::null);
                        attributes.set_value(literals::IscsiTarget::CHAP_SECRET, Json::Value::null);
                        attributes.set_value(literals::IscsiTarget::MUTUAL_CHAP_USERNAME, Json::Value::null);
                        attributes.set_value(literals::IscsiTarget::MUTUAL_CHAP_SECRET, Json::Value::null);
                    }
                    else {
                        const auto& chap = iscsi[constants::RemoteTarget::CHAP];

                        attributes.set_value(literals::IscsiTarget::AUTHENTICATION_METHOD,
                                             chap[constants::RemoteTarget::TYPE].is_null() ? Json::Value::null :
                                             chap[constants::RemoteTarget::TYPE].as_string());

                        auto set_attribute = [&](const char* gami_literal, const char* rest_literal) {
                            if (chap.is_member(rest_literal)) {
                                attributes.set_value(gami_literal,
                                    chap[rest_literal].is_null() ? Json::Value::null : chap[rest_literal].as_string());
                            }
                        };
                        set_attribute(literals::IscsiTarget::CHAP_USERNAME, constants::RemoteTarget::USERNAME);
                        set_attribute(literals::IscsiTarget::CHAP_SECRET, constants::RemoteTarget::SECRET);
                        set_attribute(literals::IscsiTarget::MUTUAL_CHAP_USERNAME,
                                      constants::RemoteTarget::MUTUAL_USERNAME);
                        set_attribute(literals::IscsiTarget::MUTUAL_CHAP_SECRET,
                                      constants::RemoteTarget::MUTUAL_SECRET);
                    }
                }
            }
        }
    }

    if (!attributes.empty()) {
        requests::SetComponentAttributes set_component_attributes_request{target_uuid, attributes};
        const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(agent_id);

        auto set_target_attributes = [&, gami_agent] {
            // Call set component attribute method
            const auto& set_component_attributes_response = gami_agent->
                execute<agent_framework::model::responses::SetComponentAttributes>(set_component_attributes_request);

            const auto& result_statuses = set_component_attributes_response.get_statuses();
            if (!result_statuses.empty()) {
                const auto& error =
                    ErrorFactory::create_error_from_set_component_attributes_results(result_statuses,
                                                                                     gami_to_rest_attributes);
                throw ServerException(error);
            }

            auto parent_uuid = remote_target.get_parent_uuid();
            psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                agent_framework::model::enums::Component::IscsiTarget)->
                load(gami_agent, parent_uuid,
                     agent_framework::model::enums::Component::StorageServices, remote_target.get_uuid(), false);
        };

        gami_agent->execute_in_transaction(set_target_attributes);

    }

    get(request, response);
}


void endpoint::RemoteTarget::del(const server::Request& req, server::Response& res) {

    using HandlerManager = psme::rest::model::handler::HandlerManager;

    auto target = psme::rest::model::Find<agent_framework::model::IscsiTarget>(req.params[PathParam::REMOTE_TARGET_ID])
        .via<agent_framework::model::StorageServices>(req.params[PathParam::SERVICE_ID])
        .get();

    auto gami_req = agent_framework::model::requests::DeleteIscsiTarget(
        target.get_uuid());

    const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(target.get_agent_id());

    auto remove_target = [&, gami_agent] {
        // try removing target from agent's model
        gami_agent->execute<agent_framework::model::responses::DeleteIscsiTarget>(gami_req);

        // remove target from REST model, DO NOT use target reference after this line!
        HandlerManager::get_instance()->get_handler(
            agent_framework::model::enums::Component::IscsiTarget)->
            remove(target.get_uuid());

        res.set_status(server::status_2XX::NO_CONTENT);
    };

    gami_agent->execute_in_transaction(remove_target);
}
