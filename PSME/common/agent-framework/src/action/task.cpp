/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file task.cpp
 * @brief Asynchronous task model class
 * */

#include "agent-framework/action/task.hpp"

#include <algorithm>
#include <exception>



using namespace agent_framework::action;
using namespace agent_framework::exceptions;


Task::~Task() {}


void Task::operator()() {
    try {
        prerun();
        run();
        run_completion_callbacks();
    }
    catch (const GamiException& e) {
        run_exception_callbacks(e);
        run_exception_handlers(e);
    }
    catch (const std::exception& e) {
        auto gami_exception = GamiException(ErrorCode::UNKNOWN_ERROR, e.what());
        run_exception_callbacks(gami_exception);
        run_exception_handlers(gami_exception);
    }
    catch (...) {
        auto unknown_exception = GamiException(ErrorCode::UNKNOWN_ERROR, "An unknown error occurred during task execution.");
        run_exception_callbacks(unknown_exception);
        run_exception_handlers(unknown_exception);
    }
    postrun();
}


void Task::run() {
    for (const auto& subtask : m_subtasks) {
        subtask();
    }
}


void Task::prerun() {
    for (const auto& prerun_action : m_prerun_actions) {
        prerun_action();
    }
}


void Task::postrun() {
    for (const auto& postrun_action : m_postrun_actions) {
        postrun_action();
    }
}


Task& Task::add_subtask(const SubtaskType& subtask) {
    m_subtasks.push_back(subtask);
    return *this;
}


Task& Task::add_subtask(SubtaskType&& subtask) {
    m_subtasks.push_back(std::move(subtask));
    return *this;
}


Task& Task::move_subtask(std::size_t old_position, std::size_t new_position) {
    if (old_position >= m_subtasks.size() || new_position >= m_subtasks.size()) {
        throw std::out_of_range("Cannot move subtask: index out of range");
    }

    const SubtaskType subtask = m_subtasks[old_position];
    m_subtasks.erase(m_subtasks.begin() + old_position);
    m_subtasks.insert(m_subtasks.begin() + new_position, subtask);

    return *this;
}


Task& Task::move_callback(CallbackType callback_type, std::size_t old_position, std::size_t new_position) {
    switch (callback_type) {
        case CallbackType::Completion:
            move_completion_callback(old_position, new_position);
            break;
        case CallbackType::Exception:
            move_exception_callback(old_position, new_position);
            break;
        default:
            break;
    }

    return *this;
}


Task& Task::move_prerun_action(std::size_t old_position, std::size_t new_position) {
    if (old_position >= m_prerun_actions.size() || new_position >= m_prerun_actions.size()) {
        throw std::out_of_range("Cannot move prerun action: index out of range");
    }

    const PrerunActionType prerun_action = m_prerun_actions[old_position];
    m_prerun_actions.erase(m_prerun_actions.begin() + old_position);
    m_prerun_actions.insert(m_prerun_actions.begin() + new_position, prerun_action);

    return *this;
}


Task& Task::move_postrun_action(std::size_t old_position, std::size_t new_position) {
    if (old_position >= m_postrun_actions.size() || new_position >= m_postrun_actions.size()) {
        throw std::out_of_range("Cannot move postrun action: index out of range");
    }

    const PostrunActionType postrun_action = m_postrun_actions[old_position];
    m_postrun_actions.erase(m_postrun_actions.begin() + old_position);
    m_postrun_actions.insert(m_postrun_actions.begin() + new_position, postrun_action);

    return *this;
}


Task& Task::remove_subtask(std::size_t position) {
    if (position >= m_subtasks.size()) {
        throw std::out_of_range("Cannot delete subtask: index out of range");
    }

    m_subtasks.erase(m_subtasks.begin() + position);

    return *this;
}


Task& Task::remove_callback(CallbackType callback_type, std::size_t position) {
    switch (callback_type) {
        case CallbackType::Completion:
            remove_completion_callback(position);
            break;
        case CallbackType::Exception:
            remove_exception_callback(position);
            break;
        default:
            break;
    }

    return *this;
}


Task& Task::remove_prerun_action(std::size_t position) {
    if (position >= m_prerun_actions.size()) {
        throw std::out_of_range("Cannot delte prerun action: index out of rande");
    }

    m_prerun_actions.erase(m_subtasks.begin() + position);

    return *this;
}


Task& Task::remove_postrun_action(std::size_t position) {
    if (position >= m_postrun_actions.size()) {
        throw std::out_of_range("Cannot delete postrun action: index out of range");
    }

    m_postrun_actions.erase(m_postrun_actions.begin() + position);

    return *this;
}


Task& Task::add_exception_handler(const action::Task::ExceptionHandlerFunctionType& exception_handler) {
    m_exception_handlers.push_back(exception_handler);
    return *this;
}


Task& Task::add_prerun_action(const PrerunActionType& prerun_action) {
    m_prerun_actions.push_back(prerun_action);
    return *this;
}


Task& Task::add_postrun_action(const PostrunActionType& postrun_action) {
    m_postrun_actions.push_back(postrun_action);
    return *this;
}

// Protected functions implementation

Task& Task::add_completion_callback(const CallbackFunctionType& callback) {
    m_completion_callbacks.push_back(callback);
    return *this;
}


Task& Task::add_exception_callback(const ExceptionCallbackFunctionType& callback) {
    m_exception_callbacks.push_back(callback);
    return *this;
}


void Task::run_completion_callbacks() {
    for (const auto& completion_callback : m_completion_callbacks) {
        completion_callback();
    }
}


void Task::run_exception_callbacks(const GamiException& e) {
    for (const auto& exception_callback : m_exception_callbacks) {
        exception_callback(e);
    }
}


void Task::run_exception_handlers(const GamiException& e) {
    for (const auto& exception_handler : m_exception_handlers) {
        exception_handler(e);
    }
}


Task& Task::move_completion_callback(std::size_t old_position, std::size_t new_position) {
    if (old_position >= m_completion_callbacks.size() || new_position >= m_completion_callbacks.size()) {
        throw std::out_of_range("Cannot delete callback: index out of range");
    }

    CallbackFunctionType completion_callback = m_completion_callbacks[old_position];
    m_completion_callbacks.erase(m_completion_callbacks.begin() + old_position);
    m_completion_callbacks.insert(m_completion_callbacks.begin() + new_position, completion_callback);

    return *this;
}


Task& Task::move_exception_callback(std::size_t old_position, std::size_t new_position) {
    if (old_position >= m_exception_callbacks.size() || new_position >= m_exception_callbacks.size()) {
        throw std::out_of_range("Cannot delete callback: index out of range");
    }

    ExceptionCallbackFunctionType exception_callback = m_exception_callbacks[old_position];
    m_exception_callbacks.erase(m_exception_callbacks.begin() + old_position);
    m_exception_callbacks.insert(m_exception_callbacks.begin() + new_position, exception_callback);

    return *this;
}


Task& Task::remove_completion_callback(std::size_t position) {
    if (position >= m_completion_callbacks.size()) {
        throw std::out_of_range("Cannot delete completion callback: index out of range");
    }

    m_completion_callbacks.erase(m_completion_callbacks.begin() + position);

    return *this;
}


Task& Task::remove_exception_callback(std::size_t position) {
    if (position >= m_exception_callbacks.size()) {
        throw std::out_of_range("Cannot delete exception callback: index out of range");
    }

    m_exception_callbacks.erase(m_exception_callbacks.begin() + position);

    return *this;
}



