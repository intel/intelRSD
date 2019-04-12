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
 *
 * @file thread.hpp
 *
 * @brief Thread declaration
 * */

#pragma once
#include <future>
#include <thread>
#include <atomic>

/*! Psme namespace */
namespace agent_framework {
/*! Threading namespace */
namespace threading {

/*!
 * @brief Thread class
 */
class Thread {
public:

    /*! @brief Default constructor */
    Thread() = default;

    /* delete copy & assign operators */
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    /*! @brief Execute function */
    virtual void execute() { }

    /*! @brief Start thread */
    void start();

    /*! @brief Stop thread */
    void stop();

    /*! @brief Destructor */
    virtual ~Thread();

protected:
    /*!
     * @brief Check if thread is still running
     * @return true if thread is running false otherwise
     */
    bool is_running() const { return m_running; }

    /*!
     * @brief Set thread running state
     * @param[in] enable Thread running state
     */
    void set_running(bool enable) { m_running = enable; }

private:
    std::atomic_bool m_running{false};
    std::thread m_thread{};
};

}
}
