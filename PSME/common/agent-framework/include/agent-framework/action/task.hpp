/*!
 * @brief
 *
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
 * @file
 * */

#pragma once



#include "agent-framework/exceptions/gami_exception.hpp"

#include <functional>
#include <vector>
#include <utility>



namespace agent_framework {
namespace action {

class Task {
// @TODO: improve move semantics by overloading methods taking rvalue references
// @TODO: allow registration of callbacks using std::bind
public:
    using CallbackFunctionType = std::function<void(void)>;
    using ExceptionHandlerFunctionType = std::function<void(const agent_framework::exceptions::GamiException&)>;
    using ExceptionCallbackFunctionType = std::function<void(const agent_framework::exceptions::GamiException&)>;
    using PrerunActionType = std::function<void(void)>;
    using PostrunActionType = std::function<void(void)>;
    using SubtaskType = std::function<void(void)>;

    using Callbacks = std::vector<CallbackFunctionType>;
    using ExceptionHandlers = std::vector<ExceptionCallbackFunctionType>;
    using ExceptionCallbacks = std::vector<ExceptionCallbackFunctionType>;
    using PrerunActions = std::vector<PrerunActionType>;
    using PostrunActions = std::vector<PostrunActionType>;
    using Subtasks = std::vector<SubtaskType>;


    Task() = default;


    virtual ~Task();


    /* Enable copy and move semantics */
    Task(const Task&) = default;


    Task(Task&&) = default;


    Task& operator=(const Task&) = default;


    Task& operator=(Task&&) = default;


    /*!
     * Enum representing allowable callback types
     * */
    enum class CallbackType {
        Completion,
        Exception
    };


    /*!
     * Start the task
     * */
    virtual void operator()();


    /*!
     * Run all subtasks
     * */
    virtual void run();


    /*!
     * Run all prerun actions
     * */
    virtual void prerun();


    /*!
     * Run all postrun actions
     * */
    virtual void postrun();


    /*!
     * Add subtask to the task
     *
     * @param[in] subtask Callable subtask object
     *
     * @return *this
     * */
    Task& add_subtask(const SubtaskType& subtask);


    /*!
     * Add subtask to the task
     *
     * @param[in] subtask Callable subtask object
     *
     * @return *this
     * */
    Task& add_subtask(SubtaskType&& subtask);


    /*!
     * Add completion callback.
     *
     * @param[in] callback Completion callback to be added.
     *
     * @return *this
     * */
    Task& add_completion_callback(const CallbackFunctionType& callback);


    /*!
     * Add exception callback.
     *
     * @param[in] callback Exception callback to be added.
     *
     * @return *this
     * */
    Task& add_exception_callback(const ExceptionCallbackFunctionType& callback);


    /*!
     * Register exception handler for the task
     *
     * @param[in] exception_handler Exception handler to be called when an exception is thrown
     * during task execution.
     *
     * @return *this
     * */
    Task& add_exception_handler(const ExceptionCallbackFunctionType& exception_handler);


    /*!
     * Add prerun action
     *
     * @param[in] prerun_action Callable prerun object
     *
     * @return *this
     * */
    Task& add_prerun_action(const PrerunActionType& prerun_action);


    /*!
     * Add postrun action
     *
     * @param[in] postrun_action Callable postrun object
     *
     * @return *this
     * */
    Task& add_postrun_action(const PostrunActionType& postrun_action);


    /*!
     * Move subtask
     *
     * @param[in] old_position Position of subtask to be moved
     * @param[in] new_position Position of subtask to be moved to
     *
     * @return *this
     * */
    Task& move_subtask(std::size_t old_position, std::size_t new_position);


    /*!
     * Move registered callback
     *
     * @param[in] callback_type Type of callback to be moved
     * @param[in] old_position Position of callback to be moved
     * @param[in] new_position Position of callback to be moved to
     *
     * @return *this
     * */
    Task& move_callback(CallbackType callback_type, std::size_t old_position, std::size_t new_position);


    /*!
     * Move prerun action
     *
     * @param[in] old_position Position of prerun action to be moved
     * @param[in] new_position Position of prerun action to be moved to
     *
     * @return *this
     * */
    Task& move_prerun_action(std::size_t old_position, std::size_t new_position);


    /*!
     * Move postrun action
     *
     * @param[in] old_position Position of postrun action to be moved
     * @param[in] new_position Position of postrun action to be moved to
     *
     * @return *this
     * */
    Task& move_postrun_action(std::size_t old_position, std::size_t new_position);


    /*!
     * Remove subtask
     *
     * @param[in] position Position of subtask to be removed
     *
     * @return *this
     * */
    Task& remove_subtask(std::size_t position);


    /*!
     * Remove callback
     *
     * @param[in] callback_type Type of callback
     * @param[in] position Position of subtask to be removed
     *
     * @return *this
     * */
    Task& remove_callback(CallbackType callback_type, std::size_t position);


    /*!
     * Remove prerun action
     *
     * @param[in] position Position of prerun action to be removed
     *
     * @return *this
     * */
    Task& remove_prerun_action(std::size_t position);


    /*!
     * Remove postrun action
     *
     * @param[in] position Position of postrun action to be removed
     *
     * @return *this
     * */
    Task& remove_postrun_action(std::size_t position);


protected:
    /*!
     * Run all completion callbacks.
     * */
    void run_completion_callbacks();


    /*!
     * Run all exception callbacks.
     * */
    void run_exception_callbacks(const agent_framework::exceptions::GamiException& e);


    /*!
     * Run all exception callbacks.
     * */
    void run_exception_handlers(const agent_framework::exceptions::GamiException& e);


    /*!
     * Move registered completion callback
     *
     * @param[in] old_position Position of callback to be moved
     * @param[in] new_position Position of callback to be moved to
     *
     * @return *this
     * */
    Task& move_completion_callback(std::size_t old_position, std::size_t new_position);


    /*!
     * Move registered completion callback
     *
     * @param[in] old_position Position of callback to be moved
     * @param[in] new_position Position of callback to be moved to
     *
     * @return *this
     * */
    Task& move_exception_callback(std::size_t old_position, std::size_t new_position);


    /*!
     * Remove completion callback
     *
     * @param[in] position Position of subtask to be removed
     *
     * @return *this
     * */
    Task& remove_completion_callback(std::size_t position);


    /*!
     * Remove exception callback
     *
     * @param[in] position Position of subtask to be removed
     *
     * @return *this
     * */
    Task& remove_exception_callback(std::size_t position);


private:
    Subtasks m_subtasks{};
    Callbacks m_completion_callbacks{};
    ExceptionCallbacks m_exception_callbacks{};
    ExceptionHandlers m_exception_handlers{};
    PrerunActions m_prerun_actions{};
    PostrunActions m_postrun_actions{};
};

}
}
