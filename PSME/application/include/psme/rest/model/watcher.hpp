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
 * @brief Class handling events and executing periodic tasks.
 * @file watcher.hpp
 * */

#pragma once

#include <thread>
#include <chrono>
#include <vector>

#include "agent-framework/module/requests/psme/component_notification.hpp"



namespace psme {
namespace rest {
namespace model {

class WatcherTask;

/*! @brief Class handling events and tasks */
class Watcher final {
public:
    using WatcherTaskPtr = std::unique_ptr<WatcherTask>;

    Watcher();
    ~Watcher();

    void start();
    void stop();

    /*!
     * @brief Add new task
     *
     * Task is "grabbed" by Watcher (ownership is moved to).
     */
    void add_task(WatcherTaskPtr task);

private:
    using RunTime = std::chrono::time_point<std::chrono::steady_clock>;
    using ComponentNotification = agent_framework::model::requests::ComponentNotification;

    /*! @brief Registration held by the watcher */
    struct Registration {
        WatcherTaskPtr task;
        RunTime next_run;
        unsigned executed;
    };

    using Registrations = std::vector<Registration>;

    /*! @brief All tasks handled by the watcher */
    Registrations added_tasks{};

    /*!
     * @brief Insert registration in the appropriate place
     *
     * The vector is sorted on next_run time
     */
    void insert(Registration&& reg);

    /*!
     * @brief Thread loop
     *
     * Periodically checks if any event is available in event queue, and
     * handles when any found. Checks if any task should be executed.
     */
    void watch();

    std::thread m_thread{};

    volatile bool m_running{false};

    /*!
     * @brief handle notification from agent
     */
    void process_notification(const ComponentNotification& notification);
};

/*!
 * Task used by the watcher. First execute is done immediately when task was added.
 * Then the task is executed periodically with the interval.
 * Interval between executions might vary, execution time is not included in
 * the interval. Late tasks are executed as soon as possible.
 */
class WatcherTask {
public:
    WatcherTask(const std::string& name);

    virtual ~WatcherTask();

    /*!
     * @brief Get name of the task.
     *
     * Just for logging facility.
     */
    const std::string& get_name() const {
        return m_name;
    }

    /*!
     * @brief Get interval for next execution
     * @return Interval [s] to execute task
     */
    virtual std::chrono::seconds get_interval() const = 0;

    /*!
     * @brief Execute task
     * @throws StopWatching to remove task
     */
    virtual void execute() = 0;

    /*!
     * @brief Watcher stopped notification
     */
    virtual void stopped() { }

    /*!
     * @brief Watcher started notification
     */
    virtual void started() { }

    /*! @brief Simple "exception" to stop task execution. */
    class StopWatching {
    public:
        StopWatching() = default;
        StopWatching(const StopWatching&) = default;

        ~StopWatching();
    };

private:
    WatcherTask(const WatcherTask&) = delete;
    WatcherTask() = delete;

    const std::string m_name{};
};

}
}
}

