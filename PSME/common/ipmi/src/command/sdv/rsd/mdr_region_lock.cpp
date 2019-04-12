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
 * @brief MDR region lock command implementation
 *
 * @file ipmi/command/sdv/rsd/mdr_region_lock.cpp
 */

#include "ipmi/command/sdv/rsd/mdr_region_lock.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"

#include <sstream>

using namespace ipmi;
using namespace ipmi::command::sdv::rsd;

request::MdrRegionLock::MdrRegionLock() :
    Request(NetFn::GROUP_EXTENSION, Cmd::MDR_REGION_LOCK) {}


request::MdrRegionLock::~MdrRegionLock() {}


void request::MdrRegionLock::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(static_cast<uint8_t>(GroupExtension::RACKSCALE));
    data.push_back(m_session_lock_handle);
    data.push_back(m_data_region_id);
    data.push_back(static_cast<std::uint8_t>(m_lock_type));
    data.push_back(std::uint8_t(m_timeout));
    data.push_back(std::uint8_t(m_timeout >> 8));
}


response::MdrRegionLock::MdrRegionLock() :
    Response(NetFn::GROUP_EXTENSION + 1, Cmd::MDR_REGION_LOCK, RESPONSE_SIZE) {}


response::MdrRegionLock::~MdrRegionLock() {}


void response::MdrRegionLock::unpack(const IpmiInterface::ByteBuffer& data) {
    if (static_cast<uint8_t>(GroupExtension::RACKSCALE) == data[constants::GROUP_EXTENSION_ID_OFFSET]) {
        m_is_rackscale_extension_byte_present = true;
    }

    m_session_lock_handle = data[SESSION_LOCK_HANDLE_OFFSET];
}

void response::MdrRegionLock::throw_error_on_completion_code(CompletionCode completion_code) const {
    switch (completion_code) {
        case InvalidMdrRegion::ERROR_CODE:
            throw InvalidMdrRegion(*this);
        case MdrRegionInUse::ERROR_CODE:
            throw InvalidMdrRegion(*this);
        case MdrRegionLocked::ERROR_CODE:
            throw InvalidMdrRegion(*this);
        default:
            // it's ok not to do anything, Response::throw_error_on_completion_code will handle it
            break;
        }
}
