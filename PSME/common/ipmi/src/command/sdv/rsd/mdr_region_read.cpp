/*!
 * @brief MdrRegionRead command implementation.
 *
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
 * @file mdr_region_read.cpp
 */


#include "ipmi/command/sdv/rsd/mdr_region_read.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv::rsd;
using namespace ipmi::command::sdv::constants;
using ipmi::command::sdv::NetFn;

request::MdrRegionRead::MdrRegionRead() : MdrRegionReadLegacyReq(NetFn::GROUP_EXTENSION) {}

request::MdrRegionRead::~MdrRegionRead() {}

void request::MdrRegionRead::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(static_cast<uint8_t>(GroupExtension::RACKSCALE));
    data.push_back(m_data_region_id);
    data.push_back(m_data_length);
    data.push_back(std::uint8_t(m_offset));
    data.push_back(std::uint8_t(m_offset >> 8));
}

response::MdrRegionRead::MdrRegionRead(const std::uint16_t data_length) :
    MdrRegionReadLegacyRes(NetFn::GROUP_EXTENSION, data_length) { }

response::MdrRegionRead::~MdrRegionRead() {}

void response::MdrRegionRead::unpack(const IpmiInterface::ByteBuffer& data) {
    if (static_cast<uint8_t>(GroupExtension::RACKSCALE) == data[constants::GROUP_EXTENSION_ID_OFFSET]) {
        m_is_rackscale_extension_byte_present = true;
    }
    m_length = data[LENGTH_OFFSET];
    m_update_count = data[UPDATE_COUNT_OFFSET];

    // Construct sub vector containing only SMBIOS data
    const auto& it = data.begin() + DATA_OFFSET;
    m_data = {it, it + m_length};
}

void response::MdrRegionRead::throw_error_on_completion_code(CompletionCode completion_code) const {
    switch (completion_code) {
        case InvalidMdrRegion::ERROR_CODE:
            throw InvalidMdrRegion(*this);
        case MdrRegionInUse::ERROR_CODE:
            throw MdrRegionInUse(*this);
        case MdrRegionNotLocked::ERROR_CODE:
            throw MdrRegionNotLocked(*this);
        case MdrDataLengthError::ERROR_CODE:
            throw MdrDataLengthError(*this);
        case MdrInvalidPayloadSize::ERROR_CODE:
            throw MdrInvalidPayloadSize(*this);
        default:
            // it's ok not to do anything, Response::throw_error_on_completion_code will handle it
            break;
        }
}
