/*!
 * @brief Definition of FpgaConfig class.
 *
 * @copyright Copyright (c) 2019 Intel Corporation
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

#include <mutex>
#include <string>
#include <map>
#include <memory>

namespace agent {
namespace fpga_discovery {
namespace loader {

class FpgaDiscoveryConfig final : public agent_framework::generic::Singleton<FpgaDiscoveryConfig> {
public:

    /*! @TODO cleanup config after incorporating jsonrpc server */

    /*!
     * @brief Set discovery service interfaces
     *
     * @param[in] interfaces Discovery service interface collection
     */
//    void set_discovery_service_interfaces(const nvmf_discovery::Interfaces& interfaces);

    /*!
     * @brief Get discovery service interfaces
     *
     * @return Discovery service interface collection
     */
//    const nvmf_discovery::Interfaces& get_discovery_service_interfaces() const;

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
    std::string get_ipv4_address() const;

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
    uint32_t get_location_offset() const;

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
    std::string get_parent_id() const;

    /*!
     * @brief Default destructor
     * */
    virtual ~FpgaDiscoveryConfig();

private:
    mutable std::mutex m_mutex{};
    std::string m_ipv4_address{};
    uint32_t m_location_offset{};
    std::string m_parent_id{};
//    nvmf_discovery::Interfaces m_interfaces{};
};

}
}
}

