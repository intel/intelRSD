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
#include "agent-framework/module-ref/enum/storage.hpp"

#include "psme/rest-ref/endpoints/utils.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/endpoints/logical_drive_collection.hpp"
#include "psme/rest-ref/utils/json_validator.hpp"
#include "psme/rest-ref/utils/mapper.hpp"
#include "psme/rest-ref/model/handlers/handler_manager.hpp"
#include "psme/rest-ref/model/handlers/generic_handler_deps.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"
#include "psme/core/agent/agent_manager.hpp"

#include <vector>

using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace app::rest;
using namespace app::rest::constants;
using namespace agent_framework::module;
using namespace psme::rest::utils;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#LogicalDrives";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#LogicalDriveCollection.LogicalDriveCollection";
    r[Common::NAME] = "Logical Drives Collection";
    r[Common::DESCRIPTION] = "Collection of Logical Drives";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}

std::string get_master_drive_uuid(const std::string& link){
    try {
        auto params = psme::rest::model::Mapper::get_params(link);
        auto drive =  psme::rest::model::Find<agent_framework::model::LogicalDrive>(params[PathParam::LOGICAL_DRIVE_ID])
            .via<agent_framework::model::StorageServices>(params[PathParam::SERVICE_ID])
            .get_one();
        if (drive->get_mode() == agent_framework::model::enums::LogicalDriveMode::LV) {
            return drive->get_uuid();
        } else {
            throw(agent_framework::exceptions::InvalidParameters("Logical drive " + link + " is not of LV mode and cannot be a Master Drive"));
        }
    } catch (agent_framework::exceptions::NotFound& ex) {
        log_error(GET_LOGGER("rest"), "Can't find logical drive :" + link);
        throw(agent_framework::exceptions::InvalidParameters(ex.what()));
    } catch (agent_framework::exceptions::InvalidParameters& ex) {
        log_error(GET_LOGGER("rest"), "Logical drive " + link + " is not of LV mode and cannot be a Master Drive");
        throw(ex);
    }
}

std::string get_logical_volume_group_uuid(const std::string& link){
    try {
        auto params = psme::rest::model::Mapper::get_params(link);
        auto drive =  psme::rest::model::Find<agent_framework::model::LogicalDrive>(params[PathParam::LOGICAL_DRIVE_ID])
            .via<agent_framework::model::StorageServices>(params[PathParam::SERVICE_ID])
            .get_one();
        if (drive->get_mode() == agent_framework::model::enums::LogicalDriveMode::LVG) {
            return drive->get_uuid();
        } else {
            throw(agent_framework::exceptions::InvalidParameters("Logical drive " + link + " is not of LVG mode!"));
        }
    } catch (agent_framework::exceptions::NotFound& ex) {
        log_error(GET_LOGGER("rest"), "Can't find logical drive :" + link);
        throw(agent_framework::exceptions::InvalidParameters(ex.what()));
    } catch (agent_framework::exceptions::InvalidParameters& ex) {
        log_error(GET_LOGGER("rest"), "Logical drive " + link + " is not of LVG mode!");
        throw(ex);
    }
}

json::Value validate_post_request(const server::Request& request) {
    using namespace agent_framework::model::enums;
    using json::Pair;

    json::Value allowable_types{json::Value::Type::ARRAY};
    for(const auto& value : LogicalDriveType::get_values()) {
        allowable_types.push_back(value);
    }

    json::Value allowable_modes{json::Value::Type::ARRAY};
    for(const auto& value : LogicalDriveMode::get_values()) {
        allowable_modes.push_back(value);
    }

    json::Value schema({
        Pair(Common::NAME,
            JsonValidator::type_of(JsonValidator::STRING_TYPE, true)),
        Pair(constants::LogicalDrive::TYPE,
            JsonValidator::any_of(JsonValidator::STRING_TYPE,
                allowable_types, true)),
        Pair(constants::LogicalDrive::MODE,
            JsonValidator::any_of(JsonValidator::STRING_TYPE,
                allowable_modes, true)),
        Pair(constants::LogicalDrive::PROTECTED,
            JsonValidator::type_of(JsonValidator::BOOL_TYPE, true)),
        Pair(constants::LogicalDrive::CAPACITY,
            JsonValidator::type_of(JsonValidator::NUMBER_TYPE, true)),
        Pair(constants::LogicalDrive::IMAGE,
            JsonValidator::type_of(JsonValidator::STRING_TYPE)),
        Pair(constants::LogicalDrive::BOOTABLE,
            JsonValidator::type_of(JsonValidator::BOOL_TYPE, true)),
        Pair(constants::LogicalDrive::SNAPSHOT,
            JsonValidator::type_of(JsonValidator::BOOL_TYPE, true)),
        Pair(constants::LogicalDrive::CAPACITY,
            JsonValidator::more_than(0., true)),
        Pair(Common::LINKS, json::Value({
            Pair(constants::LogicalDrive::LOGICAL_DRIVES,
                JsonValidator::type_of(JsonValidator::ARRAY_TYPE, true)),
            Pair(constants::LogicalDrive::MASTER_DRIVE,
                JsonValidator::type_of(JsonValidator::OBJECT_TYPE, true))
        }))
    });

    auto json =  JsonValidator::validate_request_body(request, schema);

    // validate links:
    if ((!json.is_member(Common::LINKS)) ||
         (!json[Common::LINKS].is_member(constants::LogicalDrive::MASTER_DRIVE)) ||
         (!json[Common::LINKS].is_member(constants::LogicalDrive::LOGICAL_DRIVES))) {
        log_error(GET_LOGGER("rest"), "Missing elements in LINKS section in json");
        throw(agent_framework::exceptions::InvalidParameters("Missing elements in LINKS section in json"));
    }

    return json;
}

}

LogicalDriveCollection::LogicalDriveCollection(const std::string& path) : Endpoint(path) {}
LogicalDriveCollection::~LogicalDriveCollection() {}

void LogicalDriveCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto service_uuid = psme::rest::model::Find<agent_framework::model::StorageServices>(req.params[PathParam::SERVICE_ID]).get_one()->get_uuid();

    auto keys = StorageManager::get_instance()->
                        get_logical_drive_manager().get_ids(service_uuid);

    json[Collection::ODATA_COUNT] =
                                    static_cast<std::uint32_t>(keys.size());
    for (const auto& key : keys) {
        json::Value link_elem(json::Value::Type::OBJECT);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(key).build();
        json[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(res, json);
}

void LogicalDriveCollection::post(const server::Request& req, server::Response& res) {

    auto json = validate_post_request(req);

    std::string master_drive_uuid = get_master_drive_uuid(
        json[Common::LINKS][constants::LogicalDrive::MASTER_DRIVE][Common::ODATA_ID].as_string());

    std::string image_type =  "";
    if ( json::Value::Type::NIL != json[constants::LogicalDrive::IMAGE]) {
        image_type = json[constants::LogicalDrive::IMAGE].as_string();
    }


    if(json[Common::LINKS][constants::LogicalDrive::LOGICAL_DRIVES].size() != 1) {
        throw(agent_framework::exceptions::InvalidParameters("LogicalDrives array should have precisely one element."));
    }

    std::string logical_volume_group_uuid = get_logical_volume_group_uuid(
        json[Common::LINKS][constants::LogicalDrive::LOGICAL_DRIVES][0][Common::ODATA_ID].as_string());

    agent_framework::model::requests::AddLogicalDrive add_drive_request{
        json[constants::LogicalDrive::TYPE].as_string(),
        endpoint::utils::gib_to_gb(json[constants::LogicalDrive::CAPACITY].as_double()),
        json[constants::LogicalDrive::MODE].as_string(),
        master_drive_uuid,
        image_type,
        json[constants::LogicalDrive::SNAPSHOT].as_bool(),
        json[constants::LogicalDrive::PROTECTED].as_bool(),
        std::vector<std::string>{logical_volume_group_uuid},
        agent_framework::model::attribute::Oem()
    };

    auto parent =  psme::rest::model::Find
        <agent_framework::model::StorageServices>
        (req.params[PathParam::SERVICE_ID]).get_one();

    auto agent_id = parent->get_agent_id();
    auto parent_uuid = parent->get_uuid();

    auto grandparent_uuid = StorageManager::get_instance()->
        get_storage_services_manager().get_entry_reference(parent_uuid)->
        get_parent_uuid();

    auto agent_manager = psme::core::agent::AgentManager::get_instance();
    auto add_drive_response = agent_manager->
        call_method <agent_framework::model::responses::AddLogicalDrive>(
            agent_id, add_drive_request);

    psme::rest::model::handler::HandlerManager::get_instance()->
        get_handler(
        agent_framework::model::enums::Component::StorageServices)->
        load(agent_manager->get_agent(agent_id),
        grandparent_uuid, parent_uuid, true);

    auto created_drive_id = StorageManager::get_instance()->
        get_logical_drive_manager().get_entry_reference(
        add_drive_response.get_drive())->get_id();

    ::psme::rest::endpoint::utils::set_location_header(
            res, req, PathBuilder(req).append(created_drive_id).build());
    res.set_status(server::status_2XX::CREATED);

}
