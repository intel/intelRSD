/*!
 * @brief Implementation of NvmeConfig class.
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
 * @file config.cpp
 */

#include "loader/config.hpp"

#include <stdexcept>

using namespace std;
using namespace agent::nvme::loader;

void NvmeConfig::set_nic_drivers(const string& name) {
    lock_guard<mutex> lock{m_mutex};
    m_nic_drivers = name;
}

string NvmeConfig::get_nic_drivers() {
    lock_guard<mutex> lock{m_mutex};
    return m_nic_drivers;
}

void NvmeConfig::set_ipv4_address(const string& ip) {
    lock_guard<mutex> lock{m_mutex};
    m_ipv4_address = ip;
}

string NvmeConfig::get_ipv4_address() {
    lock_guard<mutex> lock{m_mutex};
    return m_ipv4_address;
}

void NvmeConfig::set_location_offset(uint32_t location_offset) {
    lock_guard<mutex> lock{m_mutex};
    m_location_offset = location_offset;
}

uint32_t NvmeConfig::get_location_offset() {
    lock_guard<mutex> lock{m_mutex};
    return m_location_offset;
}

void NvmeConfig::set_parent_id(const string& parent_id) {
    lock_guard<mutex> lock{m_mutex};
    m_parent_id = parent_id;
}

string NvmeConfig::get_parent_id() {
    lock_guard<mutex> lock{m_mutex};
    return m_parent_id;
}

void NvmeConfig::set_rdma_port(uint32_t rdma_port) {
    lock_guard<mutex> lock{m_mutex};
    m_rdma_port = rdma_port;
}

uint32_t NvmeConfig::get_rdma_port() {
    lock_guard<mutex> lock{m_mutex};
    return m_rdma_port;
}

NvmeConfig::~NvmeConfig() { }

void NvmeConfig::set_endpoint_nvme_port(const Uuid& endpoint, uint16_t nvme_port) {
    lock_guard<mutex> lock{m_mutex};
    m_endpoint_nvme_port_map[endpoint] = nvme_port;
}

uint16_t NvmeConfig::get_endpoint_nvme_port(const Uuid& endpoint) {
    lock_guard<mutex> lock{m_mutex};
    if (m_endpoint_nvme_port_map.end() != m_endpoint_nvme_port_map.find(endpoint)) {
        return m_endpoint_nvme_port_map[endpoint];
    }
    throw runtime_error("Unknown NVMe port for endpoint " + endpoint);
}

