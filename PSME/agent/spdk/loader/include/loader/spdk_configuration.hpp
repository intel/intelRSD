/*!
 * @brief Configuration for SPDK agent
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 *
 * @file spdk_configuration.hpp
 */

#pragma once



#include <vector>
#include <string>
#include <mutex>



namespace agent {
namespace spdk {
namespace loader {

class SpdkConfiguration {
public:

    /*!
     * @brief Set NIC drivers
     * @param[in] drivers List of drivers
     * */
    void set_nic_drivers(const std::vector<std::string>& drivers) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_nic_drivers = drivers;
    }


    /*!
     * @brief Get NIC drivers from configuration file
     * @return NIC drivers collection
     */
    const std::vector<std::string>& get_nic_drivers() const {
        std::lock_guard<std::mutex> lock{m_mutex};
        return m_nic_drivers;
    }


    /*!
     * @brief Set SPDK socket.
     * @param spdk_socket SPDK socket.
     */
    void set_spdk_socket(const std::string& spdk_socket) {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_spdk_socket = spdk_socket;
    }


    /*!
     * @brief Get SPDK socket from configuration file
     * @return Value of SPDK socket.
     */
    const std::string& get_spdk_socket() const {
        std::lock_guard<std::mutex> lock{m_mutex};
        return m_spdk_socket;
    }


    /*!
     * @brief Get SPDK polling interval in seconds.
     * @return SPDK polling interval.
     */
    const std::chrono::seconds& get_spdk_polling_interval() const {
        return m_spdk_polling_interval;
    }


    /*!
     * @brief Set SPDK polling interval in seconds.
     * @param spdk_polling_interval Interval in seconds.
     */
    void set_spdk_polling_interval(const std::chrono::seconds& spdk_polling_interval) {
        m_spdk_polling_interval = spdk_polling_interval;
    }


    /*!
     * @brief Get SPDK port.
     * @return port.
     */
    uint32_t get_service_port() {
        std::lock_guard<std::mutex> lock{m_mutex};
        return m_port;
    }


    /*!
     * @brief Set SPDK port.
     * @param port.
     */
    void set_service_port(const uint32_t port) {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_port = port;
    }


private:
    mutable std::mutex m_mutex{};
    std::string m_spdk_socket{};
    std::vector<std::string> m_nic_drivers{};
    std::chrono::seconds m_spdk_polling_interval{};
    uint32_t m_port{};

};

}
}
}
