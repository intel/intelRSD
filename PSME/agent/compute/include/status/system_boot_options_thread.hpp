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
 * @file system_boot_options_thread.hpp
 *
 * @brief Declaration of System boot options thread class
 * */

#ifndef AGENT_COMPUTE_STATUS_SYSTEM_BOOT_OPTIONS_THREAD_HPP
#define AGENT_COMPUTE_STATUS_SYSTEM_BOOT_OPTIONS_THREAD_HPP

#include <thread>
#include <memory>

/*! Agent namespace */
namespace agent {
/*! Compute namespace */
namespace compute {
/*! Status namespace */
namespace status {

/*! @brief System Boot Options thread: thread reading systems' BootOptions
 *  i.e. active boot overrides using the IPMI getSystemBootOptions command.
 *  In case of any change, this thread updates the model of the agent.
 */
class SystemBootOptionsThread final {
public:
    using Seconds = std::chrono::seconds;

    /*! Default power state update intarval */
    static constexpr const Seconds DEFAULT_BOOT_OPTIONS_UPDATE_INTERVAL{10};
    /*!
     * @brief Constructor
     * */
    SystemBootOptionsThread() = default;

    /*!
     * @brief Constructor
     * @param[in] interval Interval between two consequtive boot options reads
     * */
    SystemBootOptionsThread(const Seconds interval):
        m_interval(interval) {
    }

    /*! @brief Destructor */
    ~SystemBootOptionsThread();

    /*! @brief Start thread */
    void start();

    /*! @brief Stop thread */
    void stop();

private:
    Seconds m_interval{DEFAULT_BOOT_OPTIONS_UPDATE_INTERVAL};
    std::thread m_thread{};
    volatile bool m_running{false};
    void m_task();
};

/*! State Machine Thread unique pointer */
using SystemBootOptionsThreadUniquePtr = std::unique_ptr<SystemBootOptionsThread>;

}
}
}
#endif /* AGENT_COMPUTE_STATUS_SYSTEM_BOOT_OPTIONS_THREAD_HPP
*/
