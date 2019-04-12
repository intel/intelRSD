/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file rmm/discovery_thread.hpp
 */

#pragma once

#include "discovery/helpers/common.hpp"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>

/*! Agent namespace */
namespace agent {
/*! Rmm namespace */
namespace rmm {

/*! DiscoveryThread thread. */
class DiscoveryThread {
public:

    /*! DiscoveryThread unique pointer */
    using DiscoveryThreadUniquePtr = std::unique_ptr<DiscoveryThread>;

    /*!
     * @brief Default constructor.
     * @param[in] interval Discoveryupdate interval in seconds
     * */
    DiscoveryThread(discovery::helpers::DiscoveryContext& dc, std::size_t interval):
        m_dc(dc), m_interval(interval) {}

    /*! Default destructor. */
    virtual ~DiscoveryThread();

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

    discovery::helpers::DiscoveryContext& m_dc;
    const std::size_t m_interval;

    std::atomic<bool> m_is_running{false};
    std::atomic<bool> m_is_discovery_finished{false};
    std::thread m_thread{};
    std::condition_variable m_condition{};
    std::mutex m_mutex{};
};

}
}
