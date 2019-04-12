/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file master_write_read.cpp
 *
 * @brief Master Write Read Command implementation
 * */

#include "ipmi/command/generic/master_write_read.hpp"
#include "ipmi/command/generic/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

request::MasterWriteRead::MasterWriteRead(std::uint8_t bus, std::uint8_t address,
    std::uint8_t read_count, std::vector<std::uint8_t> data) :
    Request(generic::NetFn::APP, generic::Cmd::MASTER_WRITE_READ),
        m_bus(bus), m_address(address), m_read_count(read_count), m_data(data)
    {}

request::MasterWriteRead::~MasterWriteRead() {}


void request::MasterWriteRead::pack(std::vector<std::uint8_t>& data) const {

    // Only private bus is used, so bus ID is written on 7 bits
    uint8_t first_byte = uint8_t(((m_bus & 0x7f) << 1) | 0x01);
    data.push_back(first_byte);
    data.push_back(m_address);
    data.push_back(m_read_count);
    if (m_data.empty()) {
        data.push_back(0);
    }
    else {
        for (const auto& elem : m_data) {
            data.push_back(elem);
        }
    }
}


// one additional byte for the status code
response::MasterWriteRead::MasterWriteRead(uint8_t data_size) :
    Response(generic::NetFn::APP, generic::Cmd::MASTER_WRITE_READ, data_size + 1) {}


response::MasterWriteRead::~MasterWriteRead() {}

void response::MasterWriteRead::unpack(const std::vector<std::uint8_t>& data) {
    for (unsigned i = 1; i < data.size(); ++i) {
        m_response.push_back(data[i]);
    }
}
