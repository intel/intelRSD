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

#include "psme/rest/endpoints/task_service/task.hpp"
#include "psme/rest/endpoints/task_service/task_service_utils.hpp"
#include "psme/rest/endpoints/utils.hpp"

#include "json-wrapper/json-wrapper.hpp"

namespace psme {
namespace rest {
namespace endpoint {
namespace task_service_utils {

server::Response call_task_get(const std::string& task_uuid) {
    const std::string& task_url{utils::get_component_url(agent_framework::model::enums::Component::Task, task_uuid)};
    server::Request get_task_request{};
    server::Response get_task_response{};
    get_task_request.set_destination(task_url);
    get_task_request.params[constants::PathParam::TASK_ID] = std::to_string(
        agent_framework::module::get_manager<agent_framework::model::Task>()
            .get_entry(task_uuid).get_id()
    );

    psme::rest::endpoint::Task(task_url).get(get_task_request, get_task_response);
    return get_task_response;
}

agent_framework::model::Task::Messages build_created_message() {
    return agent_framework::model::Task::Messages{
        agent_framework::model::attribute::Message{
            "Base.1.0.Created",
            "The resource has been created successfully.",
            agent_framework::model::enums::Health::OK,
            "None",
            agent_framework::model::attribute::Oem{},
            agent_framework::model::attribute::Message::RelatedProperties{"#/Id"},
            agent_framework::model::attribute::Message::MessageArgs{}
        }
    };
}

agent_framework::model::Task::Messages build_success_message() {
    return agent_framework::model::Task::Messages{
        agent_framework::model::attribute::Message{
            "Base.1.0.Success",
            "Successfully completed request.",
            agent_framework::model::enums::Health::OK,
            "None",
            agent_framework::model::attribute::Oem{},
            agent_framework::model::attribute::Message::RelatedProperties{"#/Id"},
            agent_framework::model::attribute::Message::MessageArgs{}
        }
    };
}

}
}
}
}
