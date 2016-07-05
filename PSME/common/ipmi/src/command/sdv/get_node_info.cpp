/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file get_node_info.cpp
 *
 * @brief ...
 * */

#include "ipmi/command/sdv/get_node_info.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetNodeInfo::GetNodeInfo() : Request(uint8_t(NetFn::INTEL), uint8_t(Cmd::GET_NODE_INFO)) {
}

request::GetNodeInfo::~GetNodeInfo() {
}

response::GetNodeInfo::GetNodeInfo() : Response(uint8_t(NetFn::INTEL), uint8_t(Cmd::GET_NODE_INFO), RESPONSE_SIZE) {
}

response::GetNodeInfo::~GetNodeInfo() {
}

void response::GetNodeInfo::unpack(const vector<uint8_t>& data) {
    if(!is_response_correct(data)) {
        return; // received only completion code, do not unpack.
    }

    m_nodes_no = data[OFFSET_NODES_NUMBER];

    m_nodes_present.reserve(NODE_NUMBER);
    for (unsigned int index = 0; index < NODE_NUMBER; index++) {
        m_nodes_present[index] = NODE_PRESENT_MASK & data[OFFSET_NODES_PRESENT + index];
    }
}

bool response::GetNodeInfo::is_present(size_t node_nr) const {
    if (node_nr < m_nodes_present.size()) {
        throw runtime_error("Node number too big: " + to_string(node_nr));
    }
    return m_nodes_present[node_nr];

}

size_t response::GetNodeInfo::get_nodes_number() const {
    return m_nodes_no;
}
