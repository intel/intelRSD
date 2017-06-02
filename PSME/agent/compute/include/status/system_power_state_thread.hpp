/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file system_power_state_thread.hpp
 *
 * @brief Declaration of System power state thread class
 * */

#pragma once
#include "agent-framework/logger_ext.hpp"
#include <thread>
#include <memory>

/*! Agent namespace */
namespace agent {
/*! Compute namespace */
namespace compute {
/*! Status namespace */
namespace status {

/*! @brief System Power State thread: thread reading systems' Power status
 *  In case of any change, this thread updates the model and generates events
 */
class SystemPowerStateThread final {
public:
    using Seconds = std::chrono::seconds;

    /*! Default power state update intarval */
    static constexpr const Seconds DEFAULT_POWER_STATE_UPDATE_INTERVAL{10};
    /*!
     * @brief Constructor
     * */
    SystemPowerStateThread() = default;

    /*!
     * @brief Constructor
     * @param[in] interval Interval between two consequtive power status reads
     * */
    SystemPowerStateThread(const Seconds interval):
        m_interval(interval) {
    }

    /*! @brief Destructor */
    ~SystemPowerStateThread();

    /*! @brief Start thread */
    void start();

    /*! @brief Stop thread */
    void stop();

private:
    Seconds m_interval{DEFAULT_POWER_STATE_UPDATE_INTERVAL};
    std::thread m_thread{};
    volatile bool m_running{false};
    void m_task();
};

/*! State Machine Thread unique pointer */
using SystemPowerStateThreadUniquePtr = std::unique_ptr<SystemPowerStateThread>;

}
}
}
