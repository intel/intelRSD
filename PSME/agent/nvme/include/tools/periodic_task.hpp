/*!
 * @brief Wrapper class for a periodic task.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file periodic_task.hpp
 */

#pragma once

#include <cstdint>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <functional>

namespace agent {
namespace nvme {
namespace tools {

/*!
 * @brief A generic periodic task wrapper.
 */
class PeriodicTask final {
public:
    using worker = std::function<void(void)>;

    /*!
     * @brief Constructor.
     *
     * @param period The task's period in milliseconds.
     * @param task Work to be performed periodically.
     * @param pre_task Preparation to be done before the periodic task.
     * @param post_task Cleanup to be done after the periodic task.
     */
    PeriodicTask(const std::chrono::milliseconds& period, worker task, worker pre_task = []{}, worker post_task = []{});

    /*!
     * @brief Deleted copy constructor.
     */
    PeriodicTask(const PeriodicTask&) = delete;

    /*!
     * @brief Defaulted move constructor.
     */
    PeriodicTask(PeriodicTask&&) = default;

    /*!
     * @brief Deleted assignment operator.
     */
    PeriodicTask& operator=(const PeriodicTask&) = delete;

    /*!
     * @brief Defaulted move assignment operator.
     */
    PeriodicTask& operator=(PeriodicTask&&) = default;

    /*!
     * @brief Start the periodic task.
     */
    void start();

    /*!
     * @brief Stop the periodic task.
     */
    void stop();

    /*!
     * @brief Destructor.
     */
    ~PeriodicTask();

private:

    /*!
     * @brief The task implementation.
     */
    void task();

    std::chrono::milliseconds m_interval{};
    std::thread m_thread{};
    std::condition_variable m_condition{};
    std::mutex m_mutex{};
    std::atomic<bool> m_is_running{false};
    worker m_task{};
    worker m_pre_task{};
    worker m_post_task{};
};

}  // namespace tools
}  // namespace nvme
}  // namespace agent
