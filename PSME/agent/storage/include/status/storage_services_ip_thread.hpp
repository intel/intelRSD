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
 * @file storage_services_ip_update_thread.hpp
 *
 * @brief Declaration of Storage Services Ip Update thread class
 * */

#ifndef AGENT_STORAGE_SERVICES_IP_UPDATE_THREAD_HPP
#define AGENT_STORAGE_SERVICES_IP_UPDATE_THREAD_HPP
#include "agent-framework/logger_ext.hpp"
#include "json/json.hpp"
#include <thread>
#include <memory>

/*! Agent namespace */
namespace agent {
/*! Storage namespace */
namespace storage {
/*! Status namespace */
namespace status {

/*! @brief StorageServicesIpUpdateThread: Thread updates storage serivces ip address
 */
class StorageServicesIpUpdateThread final {
public:
    using Seconds = std::chrono::seconds;

    /*! Default Storage services ip update intarval */
    static constexpr const Seconds DEFAULT_STORAGE_SERVICES_IP_UPDATE_INTERVAL{10};
    /*!
     * @brief Constructor
     * */
    StorageServicesIpUpdateThread() = default;

    /*!
     * @brief Constructor
     * @param[in] interval Interval between two consequtive Storage Serivce Ip
     * updates
     * */
    StorageServicesIpUpdateThread(const Seconds interval):
        m_interval(interval) {
    }

    /*! @brief Destructor */
    ~StorageServicesIpUpdateThread();

    /*! @brief Start thread */
    void start();

    /*! @brief Stop thread */
    void stop();

private:
    Seconds m_interval{DEFAULT_STORAGE_SERVICES_IP_UPDATE_INTERVAL};
    std::thread m_thread{};
    volatile bool m_running{false};
    void m_task();
};

/*! Storage Service Ip Update thread unique pointer */
using StorageServicesIpUpdateThreadUniquePtr = std::unique_ptr<StorageServicesIpUpdateThread>;

}
}
}
#endif /* AGENT_STORAGE_SERVICES_IP_UPDATE_THREAD_HPP */
