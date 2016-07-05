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
 * @file generic_client_thread.hpp
 * @brief Declaration of GenericClientThread class
 * */

#ifndef AGENT_FRAMEWORK_CLIENT_GENERIC_CLIENT_THREAD_HPP
#define AGENT_FRAMEWORK_CLIENT_GENERIC_CLIENT_THREAD_HPP

#include "agent-framework/logger_ext.hpp"
#include "agent-framework/client/generic_client.hpp"

#include <jsonrpccpp/client/connectors/httpclient.h>

#include <condition_variable>
#include <thread>
#include <mutex>

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace generic {

/*! @brief Generic Client Thread */
class GenericClientThread {
private:
    std::thread m_thread;
    const std::string m_ip;
    const int m_port;
    std::condition_variable m_condition;
    std::mutex m_mutex;

    volatile bool m_is_running;

    void m_task();

    GenericClientThread(GenericClientThread const & gct);
    GenericClientThread& operator=(GenericClientThread const & gct);
public:
    /*!
     * @brief Generic Client Thread class constructor.
     *
     * @param[in] ip Server IP address
     * @param[in] port Server port
     * */
    GenericClientThread(const std::string& ip, const int& port) :
        m_thread(&GenericClientThread::m_task, this),
        m_ip(ip),
        m_port(port),
        m_condition(),
        m_mutex(),
        m_is_running(true) {}

    /*! @brief Generic Client Thread destructor */
    ~GenericClientThread();

};

/*! Generic Client Thread unique pointer */
typedef std::unique_ptr<GenericClientThread> GenericClientThreadUniquePtr;

}
}

#endif /* AGENT_FRAMEWORK_CLIENT_GENERIC_CLIENT_THREAD_HPP */
