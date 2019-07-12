/*!
 * @brief Configuration for FPGA-oF agent
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 *
 * @file fpgaof_configuration.hpp
 */

#pragma once



#include "agent-framework/module/utils/uuid.hpp"
#include "opaepp/opae-proxy/opae_proxy_transport_detail.hpp"

#include <vector>
#include <string>
#include <mutex>
#include <map>



namespace agent {
namespace fpgaof {
namespace loader {

class FpgaofConfiguration final {
public:


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
     * @brief Set FPGAOF port for given endpoint UUID
     *
     * @param endpoint Endpoint UUID
     * @param fpgaof_port FPGAOF port
     */
    void set_endpoint_fpgaof_port(const Uuid& endpoint, uint16_t fpgaof_port);


    /*!
     * @brief Get FPGAOF port for given endpoint UUID
     *
     * @param endpoint Endpoint UUID
     * @return FPGAOF port
     */
    uint16_t get_endpoint_fpgaof_port(const Uuid& endpoint);

	/*!
	 * @brief Set OPAE-Proxy transports
	 * @param[in] opae_proxy_transports Opae proxy transports collection
	 * */
	void set_opae_proxy_transports(const std::vector<opaepp::TransportDetails>& opae_proxy_transports);


	/*!
	 * @brief Get OPAE-Proxy transports
	 * @return Opae proxy transports collection
	 * */
	const std::vector<opaepp::TransportDetails>& get_opae_proxy_transports();


    /*!
     * @brief Set secure erase .gbs default file path
     *
     * @param[in] secure_erase_gbs_file_path path to .gbs default file for secure erase
     */
    void set_secure_erase_gbs_file_path(const std::string& secure_erase_gbs_file_path);


    /*!
     * @brief Get secure erase .gbs default file path
     *
     * @return secure erase .gbs default file path
     */
    std::string get_secure_erase_gbs_file_path();


private:
    std::mutex m_mutex{};
    std::string m_ipv4_address{};
    uint32_t m_location_offset{};
    std::string m_parent_id{};
    std::map<Uuid, uint16_t> m_endpoint_fpgaof_port_map{};
    std::string m_secure_erase_gbs_file_path{};
    std::vector<opaepp::TransportDetails> m_opae_proxy_transports{};
};

}
}
}
