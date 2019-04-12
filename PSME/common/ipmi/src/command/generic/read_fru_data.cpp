/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file command/generic/read_fru_data.cpp
 */

#include "ipmi/command/generic/read_fru_data.hpp"
#include "ipmi/command/generic/enums.hpp"

#include <sstream>
#include <iomanip>

using namespace ipmi;
using namespace ipmi::command::generic;

request::ReadFruData::ReadFruData() : Request(generic::NetFn::STORAGE, generic::Cmd::READ_FRU_DATA) {}
request::ReadFruData::~ReadFruData() {}

void request::ReadFruData::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(m_device_id);
    data.push_back(uint8_t(m_offset & 0xFF));
    data.push_back(uint8_t(m_offset >> 8));
    data.push_back(m_count);
}

response::ReadFruData::ReadFruData() : Response(generic::NetFn::STORAGE, generic::Cmd::READ_FRU_DATA, MIN_RESPONSE_SIZE) {}
response::ReadFruData::~ReadFruData() {}

void response::ReadFruData::unpack(const IpmiInterface::ByteBuffer& data) {
    uint8_t count = data[OFFSET_COUNT];
    m_data.clear();
    for (std::size_t i = 0; i < count; ++i) {
        m_data.push_back(data[OFFSET_DATA + i]);
    }

}
