/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file nvme_configuration.hpp
 */

#pragma once



#include <vector>
#include <string>
#include <mutex>


namespace agent {
namespace storage {
namespace loader {

class NvmeConfiguration {
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
     * @brief Get NIC drivers
     * @return NIC drivers collection
     */
    const std::vector<std::string>& get_nic_drivers() const {
        std::lock_guard<std::mutex> lock{m_mutex};
        return m_nic_drivers;
    }

    /*!
     * @brief Set RDMA port.
     * @param rdma_port RDMA port.
     */
    void set_rdma_port(std::uint16_t rdma_port) {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_rdma_port = rdma_port;
    }

    /*!
     * @brief Get rdma-port from configuration file
     * @return Value of RDMA port.
     */
    std::uint16_t get_rdma_port() const {
        std::lock_guard<std::mutex> lock{m_mutex};
        return m_rdma_port;
    }

private:
    mutable std::mutex m_mutex{};

    std::uint16_t m_rdma_port{};
    std::vector<std::string> m_nic_drivers{};
};

}
}
}
