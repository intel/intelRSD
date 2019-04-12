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
 * @brief
 *
 * @file grantley_mdr_region_accessor.cpp
 */

#include "ipmi/utils/sdv/grantley_mdr_region_accessor.hpp"
#include "ipmi/command/sdv/mdr_region_read.hpp"

using namespace ipmi::sdv;
using namespace ipmi;

IpmiInterface::ByteBuffer GrantleyMdrRegionAccessor::get_mdr_region() {
    const auto bytes_to_read = get_mdr_region_size_to_read();
    return read_mdr_region<ipmi::command::sdv::request::MdrRegionRead, ipmi::command::sdv::response::MdrRegionRead>(
        bytes_to_read);
}

std::uint16_t GrantleyMdrRegionAccessor::get_mdr_region_size_to_read() {
    auto status = get_mdr_region_status<ipmi::command::sdv::request::GetMdrDataRegionStatus, ipmi::command::sdv::response::GetMdrDataRegionStatus>();
    check_mdr_region_unlocked_and_valid(status);
    return status.get_region_size_used();
}

std::uint8_t GrantleyMdrRegionAccessor::get_mdr_region_checksum() {
    auto status = get_mdr_region_status<ipmi::command::sdv::request::GetMdrDataRegionStatus, ipmi::command::sdv::response::GetMdrDataRegionStatus>();
    check_mdr_region_unlocked_and_valid(status);
    return status.get_region_checksum();
}

std::uint8_t GrantleyMdrRegionAccessor::get_mdr_region_update_count() {
    throw std::runtime_error("GrantleyMdrRegionAccessor::get_mdr_region_update_count not implemented");
}

void GrantleyMdrRegionAccessor::write_mdr_region(const IpmiInterface::ByteBuffer&) {
    throw std::runtime_error("GrantleyMdrRegionAccessor::write_mdr_region not implemented");
}
