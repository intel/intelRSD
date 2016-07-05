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
 * @file port_desc_tlv.cpp
 *
 * @brief LLDP Port Description TLV information
 *
 * */

#include "api/lldp/port_desc_tlv.hpp"

#include <sstream>

using std::string;
using namespace agent::network::api::lldp;

PortDescTlv::PortDescTlv(const TlvPayload& payload) {
    std::stringstream parser{};
    m_port_desc.assign(reinterpret_cast<const char*>(payload.get()),
                                                    payload.get_size());
    /* parse interface index and name
     * format: "Interface   9 as sw0p1" */
    string word{};
    parser << m_port_desc;
    parser >> word >> m_ifindex >> word >> m_ifname;
}
