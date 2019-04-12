/*!
 * @brief Implementation of MdrRegionRead Request/Response.
 *
 * @copyright Copyright (c) 2015-2019 Intel Corporation.
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
 * @file command/sdv/mdr_region_read.cpp
 */

#include "ipmi/command/sdv/mdr_region_read.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::MdrRegionRead::MdrRegionRead() :
        Request(sdv::NetFn::RACKSCALE, sdv::Cmd::MDR_REGION_READ) { }

request::MdrRegionRead::MdrRegionRead(sdv::NetFn net_fn) :
    Request(net_fn, sdv::Cmd::MDR_REGION_READ) { }

request::MdrRegionRead::~MdrRegionRead() { }

void request::MdrRegionRead::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(m_data_region_id);
    data.push_back(m_data_length);
    data.push_back(std::uint8_t(m_offset));
    data.push_back(std::uint8_t(m_offset >> 8));
}

response::MdrRegionRead::MdrRegionRead(const std::uint16_t data_length) :
        Response(sdv::NetFn::RACKSCALE + 1, sdv::Cmd::MDR_REGION_READ, data_length + OFFSET_DATA) { }

response::MdrRegionRead::MdrRegionRead(sdv::NetFn net_fn, const std::uint16_t data_length) :
    Response(sdv::NetFn(net_fn+1), sdv::Cmd::MDR_REGION_READ, data_length + OFFSET_DATA) { }

response::MdrRegionRead::~MdrRegionRead() { }

void response::MdrRegionRead::unpack(const IpmiInterface::ByteBuffer& data) {
    m_length = data[OFFSET_LENGTH];
    m_update_count = data[OFFSET_UPDATE_COUNT];

    // Construct sub vector containing only SMBIOS data
    const auto& it = data.begin() + OFFSET_DATA;
    m_data = {it, it + m_length};
}
