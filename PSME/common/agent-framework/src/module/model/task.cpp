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
 *
 * */

#include "agent-framework/module/model/task.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/utils/time.hpp"
#include "agent-framework/exceptions/exception.hpp"



using namespace agent_framework::model;

const enums::Component Task::component = enums::Component::Task;
const enums::CollectionName Task::collection_name = enums::CollectionName::Tasks;


Task::Task(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {
    // Status is unused for Tasks. Its value is always "Enabled".
    // The default Health for Task is "OK"
    set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
}


Task::~Task() { }

Task Task::from_json(const json::Json& json) {
    Task task{};

    task.set_state(json[literals::Task::STATE]);
    task.set_start_time(json[literals::Task::START_TIME]);
    task.set_end_time(json[literals::Task::END_TIME]);
    task.set_status(attribute::Status::from_json(json[literals::Task::STATUS]));
    task.set_messages(Messages::from_json(json[literals::Task::MESSAGES]));
    task.set_name(json[literals::Task::NAME]);
    task.set_oem(attribute::Oem::from_json(json.value(literals::Task::OEM, json::Json())));

    return task;
}


json::Json Task::to_json() const {
    json::Json json = json::Json();

    json[literals::Task::STATE] = get_state();
    json[literals::Task::START_TIME] = get_start_time();
    json[literals::Task::END_TIME] = get_end_time();
    json[literals::Task::STATUS] = get_status().to_json();
    json[literals::Task::MESSAGES] = get_messages().to_json();
    json[literals::Task::NAME] = get_name();
    json[literals::Task::OEM] = get_oem().to_json();

    return json;
}


void Task::start() {
    std::string message{};

    if (m_start_time.has_value()) {
        message = "Cannot start task " + get_uuid() + ", task has already started";
    }
    if (m_end_time.has_value()) {
        message = "Cannot start task " + get_uuid() + ", task already finished";
    }

    if (!message.empty()) {
        log_error(LOGUSR, message);
        throw std::logic_error(message);
    }

    m_start_time = agent_framework::utils::make_iso_8601_timestamp();
}


void Task::stop() {
    std::string message{};

    if (!m_start_time.has_value()) {
        message = "Cannot end task " + get_uuid() + ", task not started yet";
    }
    if (m_end_time.has_value()) {
        message = "Cannot end task " + get_uuid() + ", task already finished";
    }

    if (!message.empty()) {
        log_error(LOGUSR, message);
        throw std::logic_error(message);
    }

    m_end_time = agent_framework::utils::make_iso_8601_timestamp();
}
