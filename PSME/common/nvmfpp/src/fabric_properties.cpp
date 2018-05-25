/*!
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file fabric_properties.cpp
 */

#include "nvmfpp/fabric_properties.hpp"

nvmfpp::FabricProperties::~FabricProperties() {}

nvmfpp::FabricProperties::FabricProperties(
    const std::string& nqn,
    const std::string& address,
    const std::uint16_t port,
    const nvmfpp::Transport transport):
    m_nqn(nqn),
    m_address(address),
    m_port(uint16_to_string(port)),
    m_transport(enum_to_string(transport)) {}

std::string nvmfpp::FabricProperties::enum_to_string(nvmfpp::Transport transport) const {
    std::string transport_string{};

    switch (transport) {
        case Transport::RDMA:
            transport_string = "rdma";
            break;
        case Transport::FC:
            transport_string = "fc";
            break;
        case Transport::LOOP:
            transport_string = "loop";
            break;
        default:
            transport_string = "rdma";
    }
    return transport_string;
}

std::string nvmfpp::FabricProperties::uint16_to_string(uint16_t property_uint16) const {
    std::stringstream property_ss{};
    property_ss << property_uint16;
    return property_ss.str();
}

bool nvmfpp::FabricProperties::check_mandatory_connect_fields() const {
    if (m_nqn.empty() || m_transport.empty() || m_address.empty() || m_port.empty()) {
        return false;
    }
    return true;
}

bool nvmfpp::FabricProperties::check_mandatory_disconnect_fields() const {
    if (m_nqn.empty() == m_device.empty()) {
        return false;
    }
    return true;
}

std::string nvmfpp::FabricProperties::pack() const {

    std::stringstream content{};

    if (!m_nqn.empty()) {
        content << "nqn=" << m_nqn << ",";
    }
    if (!m_transport.empty()) {
        content << "transport=" << m_transport << ",";
    }
    if (!m_address.empty()) {
        content << "traddr=" << m_address << ",";
    }
    if (!m_port.empty()) {
        content << "trsvcid=" << m_port << ",";
    }
    if (!m_host_traddr.empty()) {
        content << "host_traddr=" << m_host_traddr << ",";
    }
    if (!m_hostnqn.empty()) {
        content << "hostnqn=" << m_hostnqn << ",";
    }
    if (!m_nr_io_queues.empty()) {
        content << "nr_io_queues=" << m_nr_io_queues << ",";
    }
    if (!m_queue_size.empty()) {
        content << "queue_size=" << m_queue_size << ",";
    }
    if (!m_keep_alive_tmo.empty()) {
        content << "keep_alive_tmo=" << m_keep_alive_tmo << ",";
    }
    if (!m_reconnect_delay.empty()) {
        content << "reconnect_delay=" << m_reconnect_delay << ",";
    }

    return content.str();
}
