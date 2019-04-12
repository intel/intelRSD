/*!
 * @brief Implementation of GetMdrDataRegionStatus Request/Response.
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
 * @file command/sdv/get_mdr_data_region_status.cpp
 */

#include "ipmi/command/sdv/get_mdr_data_region_status.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetMdrDataRegionStatus::GetMdrDataRegionStatus() :
        Request(sdv::NetFn::RACKSCALE, sdv::Cmd::GET_MDR_DATA_REGION_STATUS) { }

request::GetMdrDataRegionStatus::GetMdrDataRegionStatus(NetFn net_fn) :
    Request(net_fn, sdv::Cmd::GET_MDR_DATA_REGION_STATUS) { }

request::GetMdrDataRegionStatus::~GetMdrDataRegionStatus() { }

void request::GetMdrDataRegionStatus::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(m_data_region_id);
}

response::GetMdrDataRegionStatus::GetMdrDataRegionStatus():
        Response(sdv::NetFn::RACKSCALE + 1, sdv::Cmd::GET_MDR_DATA_REGION_STATUS, RESPONSE_SIZE) { }

response::GetMdrDataRegionStatus::GetMdrDataRegionStatus(NetFn net_fn) :
    Response(sdv::NetFn(net_fn+1), sdv::Cmd::GET_MDR_DATA_REGION_STATUS, RESPONSE_SIZE) { }

response::GetMdrDataRegionStatus::~GetMdrDataRegionStatus() { }

void response::GetMdrDataRegionStatus::unpack(const IpmiInterface::ByteBuffer& data) {
    m_mdr_version = data[OFFSET_MDR_VERSION];
    m_data_region_id = DataRegionId(data[OFFSET_DATA_REGION_ID]);
    m_data_validation = DataValidation(data[OFFSET_DATA_VALIDATION]);
    m_data_update_count = data[OFFSET_DATA_UPDATE_COUNT];
    m_lock_status = LockStatus(data[OFFSET_LOCK_STATUS]);

    m_region_size = uint16_t((data[OFFSET_REGION_SIZE + 1] << 8)
                           | (data[OFFSET_REGION_SIZE]));

    m_region_size_used = uint16_t((data[OFFSET_REGION_SIZE_USED + 1] << 8)
                                | (data[OFFSET_REGION_SIZE_USED]));

    m_region_checksum = data[OFFSET_REGION_CHECKSUM];
}

response::MdrDataRegionLockedError::MdrDataRegionLockedError(const Response& response)
        : ResponseError(response, "MDR data region is locked", COMPLETION_CODE_NORMAL) {}

response::MdrDataRegionLockedError::~MdrDataRegionLockedError() {}

response::MdrDataRegionInvalidError::MdrDataRegionInvalidError(const Response& response)
        : ResponseError(response, "MDR data region is not valid", COMPLETION_CODE_NORMAL) {}

response::MdrDataRegionInvalidError::~MdrDataRegionInvalidError() {}

response::MdrDataRegionEmptyError::MdrDataRegionEmptyError(const Response& response)
        : ResponseError(response, "MDR data region is empty", COMPLETION_CODE_NORMAL) {}

response::MdrDataRegionEmptyError::~MdrDataRegionEmptyError() {}
