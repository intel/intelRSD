/*!
 * @brief Implementation of MdrRegionWrite Request/Response.
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
 * @file command/sdv/mdr_region_write.cpp
 */


#include "ipmi/command/sdv/rsd/mdr_region_write.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"

#include <algorithm>
#include <iterator>

using namespace ipmi::command::sdv::rsd;

request::MdrRegionWrite::MdrRegionWrite() :
        Request(sdv::NetFn::GROUP_EXTENSION, sdv::Cmd::MDR_REGION_WRITE) { }

request::MdrRegionWrite::~MdrRegionWrite() { }

void request::MdrRegionWrite::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(static_cast<uint8_t>(GroupExtension::RACKSCALE));
    data.push_back(m_session_lock_handle);
    data.push_back(m_data_region_id);
    data.push_back(m_data_length);
    data.push_back(std::uint8_t(m_data_offset));
    data.push_back(std::uint8_t(m_data_offset >> 8));
    std::copy(m_data.cbegin(), m_data.cend(), std::back_inserter(data));
}

response::MdrRegionWrite::MdrRegionWrite():
        Response(sdv::NetFn::GROUP_EXTENSION + 1, sdv::Cmd::MDR_REGION_WRITE, RESPONSE_SIZE) { }

response::MdrRegionWrite::~MdrRegionWrite() { }

void response::MdrRegionWrite::unpack(const IpmiInterface::ByteBuffer& data) {
    if (static_cast<uint8_t>(GroupExtension::RACKSCALE) == data[constants::GROUP_EXTENSION_ID_OFFSET]) {
        m_is_rackscale_extension_byte_present = true;
    }
    m_data_region_id = DataRegionId(data[DATA_REGION_ID_OFFSET]);
    m_data_validation = DataValidation(data[DATA_VALIDATION_OFFSET]);
    m_lock_status = LockStatus(data[LOCK_STATUS_OFFSET]);

    m_max_region_length = uint16_t((data[MAX_REGION_LENGTH_OFFSET + 1] << 8)
                           | (data[MAX_REGION_LENGTH_OFFSET]));

    m_size_of_region_used = uint16_t((data[SIZE_OF_REGION_USED_OFFSET + 1] << 8)
                                | (data[SIZE_OF_REGION_USED_OFFSET]));
}

void response::MdrRegionWrite::throw_error_on_completion_code(CompletionCode completion_code) const {
    switch (completion_code) {
        case InvalidMdrRegion::ERROR_CODE:
            throw InvalidMdrRegion(*this);
        case MdrRegionInUse::ERROR_CODE:
            throw MdrRegionInUse(*this);
        case MdrRegionNotLocked::ERROR_CODE:
            throw MdrRegionNotLocked(*this);
        case MdrDataLengthError::ERROR_CODE:
            throw MdrDataLengthError(*this);
        default:
            // it's ok not to do anything, Response::throw_error_on_completion_code will handle it
            break;
        }
}
