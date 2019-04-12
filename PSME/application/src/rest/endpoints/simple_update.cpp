/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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


#include "agent-framework/module/responses/rmm.hpp"
#include "agent-framework/module/requests/rmm.hpp"
#include "agent-framework/action/task_creator.hpp"

#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/simple_update.hpp"
#include "psme/rest/endpoints/simple_update.hpp"
#include "psme/rest/endpoints/task_service/task_service_utils.hpp"
#include "psme/rest/endpoints/task_service/monitor_content_builder.hpp"
#include "psme/rest/server/multiplexer.hpp"

#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"

#include "curl/curl.h"

#include <thread>



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

namespace {
static constexpr char DESTINATION_SOFTWARE_PACKAGE_FILE_PATH[] = "/tmp/psme_rmm_package.deb";

static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::Manager::PACKAGE_URL, constants::UpdateService::IMAGE_URI}
};


void download_package(const std::string source, const std::string destination) {
    FILE* package_file = std::fopen(destination.c_str(), "w");

    if (package_file) {
        CURL* curl_handle = curl_easy_init();
        CURLcode res{CURLE_OK};

        curl_easy_setopt(curl_handle, CURLOPT_URL, source.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, reinterpret_cast<void*>(package_file));
        curl_easy_setopt(curl_handle, CURLOPT_PROTOCOLS,
                         CURLPROTO_HTTP | CURLPROTO_HTTPS | CURLPROTO_FTP | CURLPROTO_FTPS);

        res = curl_easy_perform(curl_handle);

        fclose(package_file);

        if (CURLE_OK != res) {
            log_debug("update-service", "Data transfer from " << source << " failed;: curl returned "
                                                              << static_cast<unsigned int>(res));
            throw std::runtime_error("Data transfer from " + source + " failed.");
        }
    }
    else {
        // No need to close package_file, as this code executes when it is not opened
        throw std::runtime_error("Cannot open package file for writing.");
    }
}


void install_package(const std::string package_path) {
    std::string command{"dpkg -i "};
    command += package_path;
    int update_result = system(command.c_str());
    if (0 != WEXITSTATUS(update_result)) {
        log_debug("update-service", "Rack manager update failed: dpkg returned " << update_result);
        throw std::runtime_error("Rack manager update failed");
    }
    else {
        log_info("update-service", "Rack manager software update finished.");
    }
}


void check_menagers_type(const std::vector<json::Json>& targets) {
    for (const auto& target : targets) {
        const auto& target_url = target.get<std::string>();
        auto params = server::Multiplexer::get_instance()->get_params(target_url, constants::Routes::MANAGER_PATH);
        auto manager_uuid = model::try_find<agent_framework::model::Manager>(params).get_uuid();
        if (manager_uuid.has_value()) {
            auto manager = agent_framework::module::get_manager<agent_framework::model::Manager>().get_entry(
                manager_uuid.value());
            if (agent_framework::model::enums::ManagerInfoType::RackManager != manager.get_manager_type()) {
                throw agent_framework::exceptions::InvalidValue("Cannot update managers other than RackManager");
            }
        }
        else {
            throw agent_framework::exceptions::InvalidValue("Cannot find resource: " + target_url);
        }
    }
}


std::string invoke_update_on_targets(const std::vector<json::Json>& targets,
                                     const std::string& image_uri) {
    check_menagers_type(targets);

    auto add_task_exception_message = [](const std::string& task_uuid, const std::string&) {
        auto task = agent_framework::module::get_manager<agent_framework::model::Task>()
            .get_entry_reference(task_uuid);
        agent_framework::model::Task::Messages messages{
            agent_framework::model::attribute::Message{"Base.1.0.0.GeneralError", "RackManager update failed.",
                                                       agent_framework::model::enums::Health::OK,
                                                       "None", agent_framework::model::attribute::Oem{},
                                                       agent_framework::model::attribute::Message::RelatedProperties{},
                                                       agent_framework::model::attribute::Message::MessageArgs{}}};
        task->set_messages(messages);
    };

    auto add_task_completion_message = [](const std::string& task_uuid) {
        auto task = agent_framework::module::get_manager<agent_framework::model::Task>()
            .get_entry_reference(task_uuid);
        agent_framework::model::Task::Messages messages{
            agent_framework::model::attribute::Message{"Base.1.0.0.Success", "Successfully Completed Request",
                                                       agent_framework::model::enums::Health::OK,
                                                       "None", agent_framework::model::attribute::Oem{},
                                                       agent_framework::model::attribute::Message::RelatedProperties{},
                                                       agent_framework::model::attribute::Message::MessageArgs{}}};
        task->set_messages(messages);
    };

    auto response_renderer = [](json::Json) {
        server::Response response{};
        response.set_status(server::status_2XX::NO_CONTENT);

        return response;
    };

    agent_framework::action::TaskCreator task_creator{};
    task_creator.prepare_task();
    task_creator.add_subtask(std::bind(download_package, image_uri, DESTINATION_SOFTWARE_PACKAGE_FILE_PATH));
    task_creator.add_subtask(std::bind(install_package, DESTINATION_SOFTWARE_PACKAGE_FILE_PATH));

    agent_framework::model::Task task_resource = task_creator.get_task_resource();
    auto& task_manager = agent_framework::module::get_manager<agent_framework::model::Task>();
    auto tasks_count = task_manager.get_entry_count();
    task_resource.set_id(static_cast<std::uint64_t>(tasks_count + 1));
    task_manager.add_entry(task_resource);

    task_creator.add_exception_callback(std::bind(add_task_exception_message, task_resource.get_uuid(), image_uri));
    task_creator.add_completion_callback(std::bind(add_task_completion_message, task_resource.get_uuid()));

    task_creator.set_promised_response([]() { return json::Json{}; });
    task_creator.set_promised_error_thrower([](const agent_framework::exceptions::GamiException& exception) {
        return agent_framework::exceptions::InvalidValue("RackManager update failed. " + exception.get_message());
    });

    psme::rest::endpoint::MonitorContentBuilder::get_instance()->
        add_builder(task_resource.get_uuid(), response_renderer);

    auto update_task = task_creator.get_task();

    // TaskRunner cannot be used as ARM gcc does not implement std::future correctly
    std::thread(update_task).detach();

    return task_resource.get_uuid();
}

}


endpoint::SimpleUpdate::SimpleUpdate(const std::string& path) : EndpointBase(path) {}


endpoint::SimpleUpdate::~SimpleUpdate() {}


const std::vector<agent_framework::model::enums::TransferProtocol>& endpoint::SimpleUpdate::get_allowed_protocols() {
    static std::vector<agent_framework::model::enums::TransferProtocol> allowed_protocols{
        agent_framework::model::enums::TransferProtocol::HTTP,
        agent_framework::model::enums::TransferProtocol::HTTPS,
        agent_framework::model::enums::TransferProtocol::FTP,
        agent_framework::model::enums::TransferProtocol::SFTP
    };
    return allowed_protocols;
}


void endpoint::SimpleUpdate::post(const server::Request& request, server::Response& response) {
    const auto& json = JsonValidator::validate_request_body<schema::SimpleUpdatePostSchema>(request);
    const auto& image_uri = json.value(constants::UpdateService::IMAGE_URI, std::string{});
    std::string task_uuid{};

    if (json.count(constants::UpdateService::TRANSFER_PROTOCOL)) {
        const auto& protocol = json.value(constants::UpdateService::TRANSFER_PROTOCOL, std::string{});
        const auto protocol_enum = agent_framework::model::enums::TransferProtocol::from_string(protocol);
        if (!JsonValidator::validate_allowable_values(SimpleUpdate::get_allowed_protocols(), protocol_enum)) {
            throw error::ServerException(
                error::ErrorFactory::create_error_for_not_allowable_value(
                    "Simple update does not support the requested transfer protocol.",
                    UpdateService::TRANSFER_PROTOCOL, protocol
                )
            );
        }
    }

    if (json.count(constants::UpdateService::TARGETS)) {
        const auto& targets = json[constants::UpdateService::TARGETS];
        task_uuid = ::invoke_update_on_targets(targets, image_uri);
    }

    auto response_renderer = [](json::Json /*in_json*/) -> server::Response {
        Response r{};
        r.set_status(server::status_2XX::NO_CONTENT);
        return r;
    };

    MonitorContentBuilder::get_instance()->add_builder(task_uuid, response_renderer);

    std::string task_monitor_url =
        PathBuilder(utils::get_component_url(agent_framework::model::enums::Component::Task, task_uuid))
            .append(Monitor::MONITOR)
            .build();
    psme::rest::endpoint::utils::set_location_header(request, response, task_monitor_url);
    response.set_body(psme::rest::endpoint::task_service_utils::call_task_get(task_uuid).get_body());
    response.set_status(server::status_2XX::ACCEPTED);
}
