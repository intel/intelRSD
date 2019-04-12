/*!
 * @brief Definition of NvmeConfig class.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file config.hpp
 */

#pragma once

#include "agent-framework/generic/singleton.hpp"
#include "agent-framework/module/utils/uuid.hpp"
#include "tools/base_endpoint_creator.hpp"

#include <mutex>
#include <string>
#include <map>
#include <memory>

namespace agent {
namespace nvme {
namespace loader {

class NvmeConfig final : public agent_framework::generic::Singleton<NvmeConfig> {
public:
    using EndpointCreatorPtr = std::shared_ptr<tools::BaseEndpointCreator>;

    /*!
     * @brief Set NIC drivers
     *
     * @param[in] drivers Space-separated list of drivers
     */
    void set_nic_drivers(const std::string& drivers);

    /*!
     * @brief Get NIC drivers
     *
     * @return NIC drivers separated by space
     */
    std::string get_nic_drivers();

    /*!
     * @brief Set IPv4 address
     *
     * @param[in] ip IPv4
     */
    void set_ipv4_address(const std::string& ip);

    /*!
     * @brief Get IPv4 address
     *
     * @return IPv4 address
     */
    std::string get_ipv4_address();

    /*!
     * @brief Set location offset
     *
     * @param[in] location_offset Location offset
     */
    void set_location_offset(uint32_t location_offset);

    /*!
     * @brief Get location offset
     *
     * @return location offset
     */
    uint32_t get_location_offset();

    /*!
     * @brief Set parent id
     *
     * @param[in] parent_id Parent id
     */
    void set_parent_id(const std::string& parent_id);

    /*!
     * @brief Get parent id
     *
     * @return parent id
     */
    std::string get_parent_id();

    /*!
     * @brief Set rdma port
     * @param rdma_port
     */
    void set_rdma_port(uint32_t rdma_port);

    /*!
     * @brief Get rdma-port from configuration file
     * @return rdma_port
     */
    uint32_t get_rdma_port();

    /*!
     * @brief Default destructor
     * */
    virtual ~NvmeConfig();

    /*!
     * @brief Set EndpointCreator object
     *
     * @param endpoint_creator EndpointCreator pointer
     */
    void set_endpoint_creator(const EndpointCreatorPtr& endpoint_creator) {
        m_endpoint_creator = endpoint_creator;
    }

    /*!
     * @brief Get reference to EndpointCreator object
     *
     * @return EndpointCreator
     */
    tools::BaseEndpointCreator& get_endpoint_creator() { return *(m_endpoint_creator.get()); }

    /*!
     * @brief Set NVME port for given endpoint UUID
     *
     * @param endpoint Endpoint UUID
     * @param nvme_port NVME port
     */
    void set_endpoint_nvme_port(const Uuid& endpoint, uint16_t nvme_port);

    /*!
     * @brief Get NVME port for given endpoint UUID
     *
     * @param endpoint Endpoint UUID
     * @return NVME port
     */
    uint16_t get_endpoint_nvme_port(const Uuid& endpoint);

private:
    std::mutex m_mutex{};
    std::string m_nic_drivers{};
    std::string m_ipv4_address{};
    uint32_t m_location_offset{};
    uint32_t m_rdma_port{};
    std::string m_parent_id{};
    EndpointCreatorPtr m_endpoint_creator{};
    std::map<Uuid, uint16_t> m_endpoint_nvme_port_map{};
};

}
}
}

