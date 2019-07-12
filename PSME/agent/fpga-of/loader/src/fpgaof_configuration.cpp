/*!
 * @brief FPGA-oF agent configuration.
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
 * @file fpgaof_configuration.cpp
 */


#include "loader/fpgaof_configuration.hpp"

#include <stdexcept>



using namespace agent::fpgaof::loader;




void FpgaofConfiguration::set_ipv4_address(const std::string& ip) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_ipv4_address = ip;
}


std::string FpgaofConfiguration::get_ipv4_address() {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_ipv4_address;
}


void FpgaofConfiguration::set_location_offset(uint32_t location_offset) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_location_offset = location_offset;
}


uint32_t FpgaofConfiguration::get_location_offset() {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_location_offset;
}


void FpgaofConfiguration::set_parent_id(const std::string& parent_id) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_parent_id = parent_id;
}


std::string FpgaofConfiguration::get_parent_id() {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_parent_id;
}


void FpgaofConfiguration::set_secure_erase_gbs_file_path(const std::string& secure_erase_gbs_file_path) {
	std::lock_guard<std::mutex> lock{m_mutex};
	m_secure_erase_gbs_file_path = secure_erase_gbs_file_path;
}


std::string FpgaofConfiguration::get_secure_erase_gbs_file_path() {
	std::lock_guard<std::mutex> lock{m_mutex};
	return m_secure_erase_gbs_file_path;
}


void FpgaofConfiguration::set_endpoint_fpgaof_port(const Uuid& endpoint, uint16_t fpgaof_port) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_endpoint_fpgaof_port_map[endpoint] = fpgaof_port;
}


uint16_t FpgaofConfiguration::get_endpoint_fpgaof_port(const Uuid& endpoint) {
    std::lock_guard<std::mutex> lock{m_mutex};
    if (m_endpoint_fpgaof_port_map.end() != m_endpoint_fpgaof_port_map.find(endpoint)) {
        return m_endpoint_fpgaof_port_map[endpoint];
    }
    throw std::runtime_error("Unknown FPGA-oF port for endpoint " + endpoint);
}

void FpgaofConfiguration::set_opae_proxy_transports(const std::vector<opaepp::TransportDetails>& opae_proxy_transports) {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_opae_proxy_transports = opae_proxy_transports;
}


const std::vector<opaepp::TransportDetails>& FpgaofConfiguration::get_opae_proxy_transports() {
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_opae_proxy_transports;
}
