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



#include "agent-framework/action/task.hpp"
#include "agent-framework/module/model/task.hpp"



namespace agent_framework {
namespace action {

/*!
 * A class for creating physical tasks which are automatically associated with
 * a model task class (i.e. the class representng a Task resource).
 * The advantage of using this class instead of creating objects manually is
 * that it does behind the scenes it registers certain callbacks necessary for
 * proper action of tasks feature. Doing it by hand each time a task is created
 * would be extremely error prone and difficult to maintain in the future.
 * */
class TaskCreator {
// @TODO: improve move semantics by overloading methods taking rvalue references
public:
    using TaskResource = agent_framework::model::Task;
    using PromisedResponseType = std::function<json::Json(void)>;
    using PromisedErrorThrowerType = std::function<agent_framework::exceptions::GamiException(agent_framework::exceptions::GamiException)>;


    /*!
     * Prepare task by associating it with a model task and presetting callbacks.
     * The purpose of this function is to link a physical task with a class
     * representing a task in the model. It presets the prerun actions to set the
     * resource task state to 'pending' and registers callbacks setting the model
     * task state on completion or exception.
     * */
    TaskCreator& prepare_task();


    /*!
     * Register the associated model task in the task manager. After calling this function
     * the associated task resource can be obtained via agent commands such as getTasksCollection
     * or getTaskInfo.
     * */
    void register_task();


    /*!
     * Add prerun action to the task. This acts as a proxy for the created
     * physical task, i.e. the same behavior can be obtained by calling an
     * appropriate function on the task object received with get_task method.
     *
     * @param[in] prerun_action Prerun action to be added to the constructed task
     * */
    TaskCreator& add_prerun_action(const Task::PrerunActionType& prerun_action);


    /*!
     * Add subtask to the task. This acts as a proxy for the created
     * physical task, i.e. the same behavior can be obtained by calling an
     * appropriate function on the task object received with get_task method.
     *
     * @param[in] subtask Subtask to be added to the constructed task
     * */
    TaskCreator& add_subtask(const Task::SubtaskType& subtask);


    /*!
     * Add postrun action to the task. This acts as a proxy for the created
     * physical task, i.e. the same behavior can be obtained by calling an
     * appropriate function on the task object received with get_task method.
     *
     * @param[in] postrun_action Postrun action to be added to the constructed task
     * */
    TaskCreator& add_postrun_action(const Task::PostrunActionType& postrun_action);


    /*!
     * Add completion callback to the task. This acts as a proxy for the created
     * physical task, i.e. the same behavior can be obtained by calling an
     * appropriate function on the task object received with get_task method.
     *
     * @param[in] callback Callback to be added to the constructed task
     * */
    TaskCreator& add_completion_callback(const Task::CallbackFunctionType& callback);


    /*!
     * Add exception callback to the task. This acts as a proxy for the created
     * physical task, i.e. the same behavior can be obtained by calling an
     * appropriate function on the task object received with get_task method.
     *
     * @param[in] callback Callback to be added to the constructed task
     * */
    TaskCreator& add_exception_callback(const Task::ExceptionCallbackFunctionType& callback);


    /*!
     * Get constructed physical task.
     *
     * @return Constructed task
     * */
    const Task& get_task() const;


    /*!
     * Get associated task resource.
     *
     * @return Associated task resource
     * */
    const TaskResource& get_task_resource() const;


    /*!
     * Register a stateless functional object that will be used to pass the result
     * to the task result manager after task completion. The functional object must
     * have signature json::Json(void) in order to work properly. Statelessness is
     * required as it may be called long after state variables have been destroyed.
     *
     * @param[in] promised_response Functional object that is used to render task response
     * */
    void set_promised_response(PromisedResponseType promised_response);


    /*!
     * Set a stateless functional object that is used to construct an exception object
     * in case an exception is thrown during task execution. The functional object should
     * return an exception object, i.e. it should have a signature of type
     * agent_framework::exceptions::ExceptionBase(void).
     *
     * @param[in] promised_error_thrower Functional object that returns an exception object
     * */
    void set_promised_error_thrower(PromisedErrorThrowerType promised_error_thrower);


private:
    bool m_is_task_prepared{false};
    bool m_is_task_registered{false};
    Task m_task{};
    TaskResource m_task_resource{};
};

}
}


