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
 * @file task_runner.hpp
 *
 * @brief Declaration of TaskRunner class
 * */

#ifndef AGENT_FRAMEWORK_ACTION_TASK_RUNNER_HPP
#define AGENT_FRAMEWORK_ACTION_TASK_RUNNER_HPP

#include "agent-framework/threading/threadpool.hpp"
#include <mutex>
#include <future>

namespace agent_framework {
namespace action {

/*! @brief TaskRunner class run task in separated thread */
class TaskRunner {
public:
    /*!
     * @brief Execute function in separated thread
     *
     * @tparam F Function object
     * @tparam Args Function arguments
     *
     * @param f Function of F type
     * @param args Function arguments
     *
     * @return Return future
     */
    template <typename F, typename... Args>
    auto run(F&& f, Args&&... args) ->
                    std::future<typename std::result_of<F(Args...)>::type> {
        return m_threadpool.run(std::forward<F>(f), std::forward<Args>(args)...);
    }

    /*!
     * @brief Singleton pattern. Return global TaskRunner object
     * @return TaskRunner object
     */
    static TaskRunner& get_instance();

    /*! @brief Singleton pattern. Clear global TaskRunner object */
    static void cleanup();

private:
    TaskRunner() = default;
    /*! Disable copy & move */
    TaskRunner(const TaskRunner&) = delete;
    TaskRunner& operator=(TaskRunner&) = delete;
    TaskRunner(TaskRunner&&) = delete;
    TaskRunner& operator=(TaskRunner&&) = delete;

    /*! Initialization of global task runner object */
    static void init();
    static std::once_flag m_once_flag;

    threading::Threadpool m_threadpool{};
};

}
}
#endif /* AGENT_FRAMEWORK_ACTIONS_TASK_RUNNER_HPP */
