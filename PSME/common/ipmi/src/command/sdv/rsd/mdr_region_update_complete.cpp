/*!
 * @brief Implementation of MdrRegionUpdateComplete Request/Response.
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
 * @file command/sdv/rsd/mdr_region_update_complete.cpp
 */

#include "ipmi/command/sdv/rsd/mdr_region_update_complete.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;
using namespace ipmi::command::sdv::rsd;

request::MdrRegionUpdateComplete::MdrRegionUpdateComplete() :
    Request(NetFn::GROUP_EXTENSION, Cmd::MDR_REGION_UPDATE_COMPLETE) { }

request::MdrRegionUpdateComplete::~MdrRegionUpdateComplete() { }

void request::MdrRegionUpdateComplete::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(static_cast<uint8_t>(GroupExtension::RACKSCALE));
    data.push_back(m_session_lock_handle);
    data.push_back(m_data_region_id);
}

response::MdrRegionUpdateComplete::MdrRegionUpdateComplete():
    Response(NetFn::GROUP_EXTENSION + 1, Cmd::MDR_REGION_UPDATE_COMPLETE, RESPONSE_SIZE) { }

response::MdrRegionUpdateComplete::~MdrRegionUpdateComplete() { }

void response::MdrRegionUpdateComplete::unpack(const IpmiInterface::ByteBuffer& data) {
    if (static_cast<uint8_t>(GroupExtension::RACKSCALE) == data[constants::GROUP_EXTENSION_ID_OFFSET]) {
        m_is_rackscale_extension_byte_present = true;
    }
}

void response::MdrRegionUpdateComplete::throw_error_on_completion_code(CompletionCode completion_code) const {
    switch (completion_code) {
        case InvalidMdrRegion::ERROR_CODE:
            throw InvalidMdrRegion(*this);
        case MdrRegionInUse::ERROR_CODE:
            throw MdrRegionInUse(*this);
        case MdrRegionNotLocked::ERROR_CODE:
            throw MdrRegionNotLocked(*this);
        default:
            // it's ok not to do anything, Response::throw_error_on_completion_code will handle it
            break;
        }
}
