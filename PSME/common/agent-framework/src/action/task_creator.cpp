/*!
 * @brief
 *
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @file
 * */

#include "agent-framework/action/task_creator.hpp"
#include "agent-framework/action/task_result_manager.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/eventing/event_data.hpp"
#include "agent-framework/eventing/events_queue.hpp"

#include <exception>
#include <utility>



using TaskState = agent_framework::model::enums::TaskState;

namespace agent_framework {
namespace action {

TaskCreator& TaskCreator::prepare_task() {
    if (m_is_task_prepared) {
        throw std::logic_error("Cannot prepare an already prepared task");
    }
    if (m_is_task_registered) {
        throw std::logic_error("Cannot prepare an already registered task");
    }

    auto prerun_action_unbound = [](const std::string& task_uuid) {
        auto task = module::get_manager<TaskResource>().get_entry_reference(task_uuid);
        task->set_state(TaskState::Running);
        task->start();
    };

    auto completion_callback_unbound = [](const std::string& task_uuid) {
        auto task = module::get_manager<TaskResource>().get_entry_reference(task_uuid);
        task->set_state(TaskState::Completed);
        task->stop();
    };

    auto exception_callback_unbound = [](const std::string& task_uuid) {
        auto task = module::get_manager<TaskResource>().get_entry_reference(task_uuid);
        task->set_state(TaskState::Exception);
        task->stop();
    };

    m_task.add_prerun_action(std::bind(prerun_action_unbound, m_task_resource.get_uuid()));
    m_task.add_callback(Task::CallbackType::Completion,
                        std::bind(completion_callback_unbound, m_task_resource.get_uuid()));
    m_task.add_callback(Task::CallbackType::Exception,
                        std::bind(exception_callback_unbound, m_task_resource.get_uuid()));
    m_is_task_prepared = true;

    return *this;
}


void TaskCreator::register_task() {
    if (!m_is_task_prepared) {
        throw std::logic_error("Cannot register an unprepared task");
    }
    if (m_is_task_registered) {
        throw std::logic_error("Cannot register an already registered task");
    }

    auto send_task_update_event = [](const std::string& uuid) {
        agent_framework::eventing::EventData edat;
        edat.set_component(uuid);
        edat.set_type(agent_framework::model::enums::Component::Task);
        edat.set_notification(agent_framework::eventing::Notification::Update);
        agent_framework::eventing::EventsQueue::get_instance()->push_back(edat);
    };

    auto handle_task_exception = [](const std::string& uuid, const exceptions::GamiException& e) {
        TaskResultManager::get_instance()->set_exception(uuid, e);
    };

    m_task.add_postrun_action(std::bind(send_task_update_event, m_task_resource.get_uuid()));

    m_task.add_exception_handler(std::bind(handle_task_exception, m_task_resource.get_uuid(), std::placeholders::_1));

    m_task_resource.set_state(TaskState::Pending);
    module::get_manager<TaskResource>().add_entry(m_task_resource);
    m_is_task_registered = true;
}


TaskCreator& TaskCreator::add_prerun_action(const Task::PrerunActionType& prerun_action) {
    m_task.add_prerun_action(prerun_action);
    return *this;
}


TaskCreator& TaskCreator::add_subtask(const Task::SubtaskType& subtask) {
    m_task.add_subtask(subtask);
    return *this;
}


TaskCreator& TaskCreator::add_callback(Task::CallbackType callback_type, const Task::CallbackFunctionType& callback) {
    m_task.add_callback(callback_type, callback);
    return *this;
}


TaskCreator& TaskCreator::add_postrun_action(const Task::PostrunActionType& postrun_action) {
    m_task.add_postrun_action(postrun_action);
    return *this;
}


const Task& TaskCreator::get_task() const {
    return m_task;
}


const TaskCreator::TaskResource& TaskCreator::get_task_resource() const {
    return m_task_resource;
}


void TaskCreator::set_promised_response(PromisedResponseType promised_response) {
    std::string task_uuid = m_task_resource.get_uuid();

    auto response_result_callback = [promised_response, task_uuid]() {
        json::Json response = promised_response();
        TaskResultManager::get_instance()->set_result(task_uuid, response);
    };

    m_task.add_callback(Task::CallbackType::Completion, response_result_callback);

}


void TaskCreator::set_promised_error_thrower(PromisedErrorThrowerType promised_error_thrower) {
    std::string task_uuid = m_task_resource.get_uuid();

    auto exception_thrower_callback = [task_uuid, promised_error_thrower]() {
        auto promised_error = promised_error_thrower();
        TaskResultManager::get_instance()->set_exception(task_uuid, promised_error);
    };

    m_task.add_callback(Task::CallbackType::Exception, exception_thrower_callback);
}

}
}
