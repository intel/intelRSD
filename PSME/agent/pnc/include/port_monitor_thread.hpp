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
 * @file port_monitor_thread.hpp
 * @brief Port Monitor thread.
 * */

#pragma once

#include "tools/toolset.hpp"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {

/*! PortMonitorThread thread. */
class PortMonitorThread {
public:

    /*! Port Monitor Thread unique pointer */
    using PortMonitorThreadUniquePtr = std::unique_ptr<PortMonitorThread>;

    /*!
     * @brief Default constructor.
     * @param[in] switch_uuid Uuid of the switch to be handled by the monitor
     * @param[in] t Toolset used to perform operations
     * @param[in] interval Port monitor update interval in seconds
     * */
    PortMonitorThread(const std::string& switch_uuid, const tools::Toolset& t, std::size_t interval):
        m_switch_uuid(switch_uuid), m_tools(t), m_interval(interval) {}

    /*! Default destructor. */
    virtual ~PortMonitorThread();

    /*!
     * @brief Start state machine thread
     */
    void start();

    /*!
     * @brief Checks if initial discovery has been finished
     * @return True if initial disocovery was finished
     */
     bool is_discovery_finished() const {
         return m_is_discovery_finished;
     }

private:
    void task();

    std::string m_switch_uuid{};
    const tools::Toolset& m_tools;
    std::size_t m_interval{};

    std::thread m_thread{};
    std::condition_variable m_condition{};
    std::mutex m_mutex{};
    std::atomic<bool> m_is_running{false};
    std::atomic<bool> m_is_discovery_finished{false};

};

}
}
