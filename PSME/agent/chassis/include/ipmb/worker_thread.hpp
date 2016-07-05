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
 * @file ipmb/worker_thread.hpp
 * @brief IPMI IPMP worker thread.
 * */

#ifndef AGENT_CHASSIS_IPMB_WORKER_THREAD_HPP
#define AGENT_CHASSIS_IPMB_WORKER_THREAD_HPP

#include <mutex>
#include <thread>
#include <vector>

/*! Agent namespace */
namespace agent {
/*! Chassis namespace */
namespace chassis {
/*! IPMB namespace */
namespace ipmb {

/*! IPMI Message worker thread */
class WorkerThread {
public:

    /*! Start thread */
    void start();

    virtual ~WorkerThread();
private:
    std::thread m_thread{};
    volatile bool m_is_running {false};

    void task();

};

/*! Worker Thread Unique Pointer */
using WorkerThreadUniquePtr = std::unique_ptr<WorkerThread>;

}
}
}

#endif /* AGENT_CHASSIS_IPMB_WORKER_THREAD_HPP */
