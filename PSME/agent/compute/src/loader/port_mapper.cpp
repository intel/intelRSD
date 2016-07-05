/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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
 * */

#include "port_mapper.hpp"

using namespace agent::compute;
using namespace std;

PortMapper::~PortMapper() {}

void PortMapper::set_switch_port_identifier(const string& manager_uuid,
                                            const string& port_identifier) {
    m_manager_to_port_map[manager_uuid] = port_identifier;
}

const string& PortMapper::get_switch_port_identifier(const string& manager_uuid) const {
    auto item = m_manager_to_port_map.find(manager_uuid);
    if (item == m_manager_to_port_map.end()) {
        throw std::runtime_error("No switch port for manager " + manager_uuid);
    }
    return item->second;
}
