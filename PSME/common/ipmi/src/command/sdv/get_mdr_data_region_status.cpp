/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file get_mdr_data_regions_status.cpp
 *
 * @brief Implementation of GetMdrDataRegionsStatus Request/Response.
 * */

#include "ipmi/command/sdv/get_mdr_data_region_status.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetMdrDataRegionStatus::GetMdrDataRegionStatus() :
        Request(sdv::NetFn::RACKSCALE, sdv::Cmd::GET_MDR_DATA_REGIONS_STATUS) { }

request::GetMdrDataRegionStatus::~GetMdrDataRegionStatus() { }

void request::GetMdrDataRegionStatus::pack(std::vector<std::uint8_t>& data) const {
    data.push_back(m_data_region_id);
}

response::GetMdrDataRegionStatus::GetMdrDataRegionStatus():
        Response(sdv::NetFn::RACKSCALE, sdv::Cmd::GET_MDR_DATA_REGIONS_STATUS, RESPONSE_SIZE) { }

response::GetMdrDataRegionStatus::~GetMdrDataRegionStatus() { }

void response::GetMdrDataRegionStatus::unpack(const std::vector<std::uint8_t>& data) {
    if (!is_response_correct(data)) {
        return; // received only completion code, do not unpack.
    }

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
