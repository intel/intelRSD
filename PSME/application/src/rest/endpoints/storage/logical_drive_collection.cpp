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
#include "agent-framework/module/enum/storage.hpp"

#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/storage/logical_drive_collection.hpp"
#include "psme/rest/endpoints/storage/logical_drive.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/logical_drive.hpp"
#include "psme/rest/server/multiplexer.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "psme/rest/endpoints/task_service/monitor_content_builder.hpp"
#include "psme/rest/endpoints/task_service/task_service_utils.hpp"


#include <vector>



using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::validators;
using namespace psme::rest::constants;

using namespace agent_framework::module;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#LogicalDriveCollection.LogicalDriveCollection";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#LogicalDriveCollection.LogicalDriveCollection";
    r[Common::NAME] = "Logical Drives Collection";
    r[Common::DESCRIPTION] = "Collection of Logical Drives";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}


std::string get_master_drive_uuid(const std::string& link) {
    try {
        auto params = server::Multiplexer::get_instance()->get_params(link, constants::Routes::LOGICAL_DRIVE_PATH);
        auto drive = psme::rest::model::Find<agent_framework::model::LogicalDrive>(params[PathParam::LOGICAL_DRIVE_ID])
            .via<agent_framework::model::StorageService>(params[PathParam::SERVICE_ID])
            .get_one();

        if (drive->get_mode() == agent_framework::model::enums::LogicalDriveMode::LV) {
            return drive->get_uuid();
        }

        THROW(agent_framework::exceptions::InvalidValue, "rest",
              "Logical drive " + link + " is not of LV mode and cannot be a Master Drive.");
    }
    catch (agent_framework::exceptions::NotFound& ex) {
        log_error(GET_LOGGER("rest"), "Can't find logical drive: " + link);
        throw agent_framework::exceptions::InvalidValue(ex.get_message());
    }
}


std::string get_logical_volume_group_uuid(const std::string& link) {
    try {
        auto params = server::Multiplexer::get_instance()->get_params(link, constants::Routes::LOGICAL_DRIVE_PATH);
        auto drive = psme::rest::model::Find<agent_framework::model::LogicalDrive>(params[PathParam::LOGICAL_DRIVE_ID])
            .via<agent_framework::model::StorageService>(params[PathParam::SERVICE_ID])
            .get_one();

        if (drive->get_mode() == agent_framework::model::enums::LogicalDriveMode::LVG) {
            return drive->get_uuid();
        }

        THROW(agent_framework::exceptions::InvalidValue, "rest",
              "Logical drive " + link + " is not of LVG mode!");
    }
    catch (agent_framework::exceptions::NotFound& ex) {
        log_error(GET_LOGGER("rest"), "Can't find logical drive: " + link);
        throw agent_framework::exceptions::InvalidValue(ex.get_message());
    }
}

}


LogicalDriveCollection::LogicalDriveCollection(const std::string& path) : EndpointBase(path) {}


LogicalDriveCollection::~LogicalDriveCollection() {}


void LogicalDriveCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto service_uuid =
        psme::rest::model::Find<agent_framework::model::StorageService>(req.params[PathParam::SERVICE_ID]).get_uuid();

    auto keys = StorageComponents::get_instance()->
        get_logical_drive_manager().get_ids(service_uuid);

    json[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(keys.size());
    for (const auto& key : keys) {
        json::Value link_elem(json::Value::Type::OBJECT);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(key).build();
        json[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(res, json);
}


void LogicalDriveCollection::post(const server::Request& req, server::Response& res) {
    auto json = JsonValidator::validate_request_body<schema::LogicalDrivePostSchema>(req);

    const auto& master_drive_uuid = get_master_drive_uuid(
        json[Common::LINKS][constants::LogicalDrive::MASTER_DRIVE][Common::ODATA_ID].as_string());

    if (json[Common::LINKS][constants::LogicalDrive::LOGICAL_DRIVES].size() != 1) {
        auto error = error::ErrorFactory::create_invalid_payload_error(
            "Array should have precisely one element.", {
                PathBuilder(Common::LINKS)
                    .append(constants::LogicalDrive::LOGICAL_DRIVES)
                    .build()
            });
        throw error::ServerException(error);
    }

    std::string logical_volume_group_uuid = get_logical_volume_group_uuid(
        json[Common::LINKS][constants::LogicalDrive::LOGICAL_DRIVES][0][Common::ODATA_ID].as_string());

    std::string image = (json[constants::LogicalDrive::IMAGE] == json::Value::Type::NIL)
                        ? "" : json[constants::LogicalDrive::IMAGE].as_string();

    agent_framework::model::requests::AddLogicalDrive add_drive_request{
        json[constants::LogicalDrive::TYPE].as_string(),
        endpoint::utils::gib_to_gb(json[constants::LogicalDrive::CAPACITY].as_double()),
        json[constants::LogicalDrive::MODE].as_string(),
        master_drive_uuid,
        image,
        json[constants::LogicalDrive::SNAPSHOT].as_bool(),
        json[constants::LogicalDrive::BOOTABLE].as_bool(),
        json[constants::LogicalDrive::PROTECTED].as_bool(),
        std::vector<std::string>{logical_volume_group_uuid},
        agent_framework::model::attribute::Oem()
    };

    auto parent =
        psme::rest::model::Find<agent_framework::model::StorageService>(req.params[PathParam::SERVICE_ID]).get();

    auto agent_id = parent.get_agent_id();
    auto parent_uuid = parent.get_uuid();

    auto grandparent_uuid = StorageComponents::get_instance()->
        get_storage_service_manager().get_entry_reference(parent_uuid)->
        get_parent_uuid();

    // prepare response which will be presented on task completion
    auto response_renderer = [req](json::Json in_json) -> server::Response {
        auto command_response = agent_framework::model::responses::AddLogicalDrive::from_json(in_json);
        auto created_drive_id = StorageComponents::get_instance()->
            get_logical_drive_manager().get_entry(
            command_response.get_drive()).get_id();

        Response response{};
        response.set_status(server::status_2XX::CREATED);
        psme::rest::endpoint::utils::set_location_header(response, PathBuilder(req).append(created_drive_id).build());

        return response;
    };

    // reload Storage Service resource (on task completion)
    auto completion_notifier = [parent_uuid, grandparent_uuid, agent_id](const std::string& task_uuid) {
        auto lambda_agent_manager = psme::core::agent::AgentManager::get_instance();

        auto task = agent_framework::module::get_manager<agent_framework::model::Task>().get_entry_reference(
            task_uuid);
        if (task->get_state() == agent_framework::model::enums::TaskState::Completed) {
            agent_framework::model::Task::Messages messages{
                agent_framework::model::attribute::Message{"Base.1.0.0.Created", "The resource has been created successfully",
                                                           agent_framework::model::enums::Health::OK,
                                                           "None", agent_framework::model::attribute::Oem{},
                                                           agent_framework::model::attribute::Message::RelatedProperties{
                                                               "#/Id"},
                                                           agent_framework::model::attribute::Message::MessageArgs{}}};
            task->set_messages(messages);
        }

        psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
            agent_framework::model::enums::Component::StorageService)->
            load(lambda_agent_manager->get_agent(agent_id),
                 grandparent_uuid, agent_framework::model::enums::Component::Manager, parent_uuid, true);
    };

    auto gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(agent_id);

    auto add_drive = [&, gami_agent] {
        auto add_drive_response = gami_agent->
            execute<agent_framework::model::responses::AddLogicalDrive>(add_drive_request);

        // If the response object does not contain "task" field, then the request resulted in an action,
        // otherwise a task needs to be created
        if (add_drive_response.get_task().empty()) {
            psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                agent_framework::model::enums::Component::StorageService)->
                load(gami_agent,
                     grandparent_uuid, agent_framework::model::enums::Component::Manager, parent_uuid, true);

            auto created_drive_id = StorageComponents::get_instance()->
                get_logical_drive_manager().get_entry_reference(
                add_drive_response.get_drive())->get_id();

            psme::rest::endpoint::utils::set_location_header(res, PathBuilder(req).append(created_drive_id).build());
            res.set_status(server::status_2XX::CREATED);
        }
        else {
            const auto task_uuid = add_drive_response.get_task();

            auto task_handler = psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                agent_framework::model::enums::Component::Task);
            task_handler->load(gami_agent,
                               "",
                               agent_framework::model::enums::Component::Task,
                               task_uuid,
                               false);

            MonitorContentBuilder::get_instance()->add_builder(task_uuid, response_renderer);
            CommonComponents::get_instance()->get_task_manager().get_entry_reference(
                task_uuid)->add_completion_notifier(std::bind(completion_notifier, task_uuid));

            std::string task_monitor_url = PathBuilder(
                utils::get_component_url(agent_framework::model::enums::Component::Task, task_uuid)).append(
                Monitor::MONITOR).build();
            psme::rest::endpoint::utils::set_location_header(res, task_monitor_url);
            res.set_body(psme::rest::endpoint::task_service_utils::call_task_get(task_uuid).get_body());
            res.set_status(server::status_2XX::ACCEPTED);
        }
    };

    gami_agent->execute_in_transaction(add_drive);
}
