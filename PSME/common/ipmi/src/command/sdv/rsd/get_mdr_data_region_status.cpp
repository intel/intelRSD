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
 * @brief Get mdr data region status command implementation for 2.2 platform
 *
 * @file ipmi/command/sdv/rsd/get_mdr_data_region_status.cpp
 */

#include "ipmi/command/sdv/rsd/get_mdr_data_region_status.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"

using namespace ipmi::command::sdv::rsd;

using ipmi::command::sdv::NetFn;

request::GetMdrDataRegionStatus::GetMdrDataRegionStatus() : GetMdrStatusLegacyReq(NetFn::GROUP_EXTENSION) {}

request::GetMdrDataRegionStatus::~GetMdrDataRegionStatus() {}

void request::GetMdrDataRegionStatus::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(static_cast<uint8_t>(GroupExtension::RACKSCALE));
    data.push_back(m_data_region_id);
}

response::GetMdrDataRegionStatus::GetMdrDataRegionStatus():
    GetMdrStatusLegacyRes(NetFn::GROUP_EXTENSION) {}

response::GetMdrDataRegionStatus::~GetMdrDataRegionStatus() { }

void response::GetMdrDataRegionStatus::unpack(const IpmiInterface::ByteBuffer& data) {
    if (static_cast<uint8_t>(GroupExtension::RACKSCALE) == data[constants::GROUP_EXTENSION_ID_OFFSET]) {
        m_is_rackscale_extension_byte_present = true;
    }
    m_mdr_version = data[MDR_VERSION_OFFSET];
    m_data_region_id = DataRegionId(data[DATA_REGION_ID_OFFSET]);
    m_data_validation = DataValidation(data[DATA_VALIDATION_OFFSET]);
    m_data_update_count = data[DATA_UPDATE_COUNT_OFFSET];
    m_lock_status = LockStatus(data[LOCK_STATUS_OFFSET]);

    m_region_size = uint16_t((data[REGION_SIZE_OFFSET + 1] << 8)
                             | (data[REGION_SIZE_OFFSET]));

    m_region_size_used = uint16_t((data[REGION_SIZE_USED_OFFSET + 1] << 8)
                                  | (data[REGION_SIZE_USED_OFFSET]));

    m_region_checksum = data[REGION_CHECKSUM_OFFSET];
}
