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
 *
 *
 * @file task_status_manager.hpp
 *
 * @brief Declaration of TaskStatusManager class
 * */

#ifndef AGENT_FRAMEWORK_ACTION_TASK_STATUS_MANAGER_HPP
#define AGENT_FRAMEWORK_ACTION_TASK_STATUS_MANAGER_HPP

#include <mutex>
#include <unordered_map>

namespace agent_framework {
namespace action {

/*! @brief TaskStatusManager represents status of task */
class TaskStatusManager {
public:
    /*!
     * @enum Status
     * @brief Defines the status of the task
     *
     * @var NotFound
     * Task not found
     *
     * @var Success
     * Task succeeded
     *
     * @var Fail
     * Task failed
     * */
    enum class Status {
        NotFound,
        Success,
        Fail
    };

    /*!
     * @brief Add task status
     *
     * @param uuid Task uuid
     * @param status Task status
     */
    void add_status(const std::string& uuid, const Status status);

    /*!
     * @brief Add task status
     *
     * @param uuid Task uuid
     * @param status Task status as bool value
     */
    void add_status(const std::string& uuid, const bool status);

    /*!
     * @brief Get task status
     *
     * @param uuid Task uuid
     * @return Task status
     */
    Status get_status(const std::string& uuid) const;

    /*!
     * @brief Remove task status
     * @param uuid Task uuid
     */
    void remove_status(const std::string& uuid);

    /*!
     * @brief Singleton pattern. Return global TaskStatusManager object
     * @return TaskStatusManager object
     */
    static TaskStatusManager& get_instance();

    /*! @brief Singleton pattern. Clear global TaskStatusManager object */
    static void cleanup();

private:
    TaskStatusManager() = default;
    /*! Disable copy & move */
    TaskStatusManager(const TaskStatusManager&) = delete;
    TaskStatusManager& operator=(TaskStatusManager&) = delete;
    TaskStatusManager(TaskStatusManager&&) = delete;
    TaskStatusManager& operator=(TaskStatusManager&&) = delete;

    /*! Initialization of global task runner object */
    static void init();
    static std::once_flag m_once_flag;

    mutable std::mutex m_mutex{};
    std::unordered_map<std::string, Status> m_status{};
};

}
}
#endif /* AGENT_FRAMEWORK_ACTIONS_TASK_STATUS_MANAGER_HPP */
