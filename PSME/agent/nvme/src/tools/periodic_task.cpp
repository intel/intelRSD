/*!
 * @brief Implementation of a periodic task.
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
 * @file periodic_task.cpp
 */

#include "tools/periodic_task.hpp"
#include "logger/logger_factory.hpp"

namespace agent {
namespace nvme {
namespace tools {

PeriodicTask::PeriodicTask(const std::chrono::milliseconds& period, PeriodicTask::worker task,
    PeriodicTask::worker pre_task, PeriodicTask::worker post_task) :
    m_interval{period}, m_task{task}, m_pre_task{pre_task}, m_post_task{post_task} {

}

PeriodicTask::~PeriodicTask() {
    if (m_is_running) {
        stop();
    }
}

void PeriodicTask::start() {
    m_is_running = true;
    m_thread = std::thread(&PeriodicTask::task, this);
}

void PeriodicTask::stop() {
    m_is_running = false;
    m_condition.notify_one();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void PeriodicTask::task() {
    try {
        // run user defined preparation
        m_pre_task();

        while (m_is_running) {
            try {
                std::unique_lock<std::mutex> lk(m_mutex);
                if (m_condition.wait_for(lk, m_interval) == std::cv_status::timeout) {
                    // user defined task
                    m_task();
                }
            }
            catch (std::exception& e) {
                log_error("nvme-agent", "PeriodicTask: loop FAILED: " << e.what());
            }
        }

        // user defined cleanup
        m_post_task();

    }
    catch (std::exception& e) {
                log_error("nvme-agent", "PeriodicTask: FAILED: " << e.what());
    }
    log_debug("nvme-agent", "PeriodicTask stopped");
}

}  // namespace tools
}  // namespace nvme
}  // namespace agent
