/*!
 * @brief Implementation of Get SDR Repository Info Request/Response.
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
 * @file command/generic/get_sdr_repository_info.cpp
 */

#include "ipmi/command/generic/get_sdr_repository_info.hpp"
#include "ipmi/command/generic/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

request::GetSdrRepositoryInfo::GetSdrRepositoryInfo() :
        Request(generic::NetFn::STORAGE, generic::Cmd::GET_SDR_REPOSITORY_INFO) {}

request::GetSdrRepositoryInfo::~GetSdrRepositoryInfo() {}

constexpr const std::uint8_t  response::GetSdrRepositoryInfo::IPMI2_SDR_VERSION;
constexpr const std::uint16_t response::GetSdrRepositoryInfo::AVAILABLE_64KB_MINUS_2;
constexpr const std::uint16_t response::GetSdrRepositoryInfo::UNSPECIFIED_FREE_SPACE;

response::GetSdrRepositoryInfo::GetSdrRepositoryInfo() :
        Response(generic::NetFn::STORAGE + 1, generic::Cmd::GET_SDR_REPOSITORY_INFO, RESPONSE_SIZE) {}

response::GetSdrRepositoryInfo::~GetSdrRepositoryInfo() {}


namespace {
constexpr const std::uint8_t OFFSET_RECORD_COUNT = 2;
constexpr const std::uint8_t OFFSET_FREE_SPACE = 4;
constexpr const std::uint8_t OFFSET_LAST_ADDITION_TS = 6;
constexpr const std::uint8_t OFFSET_LAST_DELETE_TS = 10;
constexpr const std::uint8_t OFFSET_SUPPORTED_OPERATIONS = 14;
}

void response::GetSdrRepositoryInfo::unpack(const IpmiInterface::ByteBuffer& data) {
    m_sdr_version = data[OFFSET_DATA];
    m_record_count = static_cast<std::uint16_t>(data[OFFSET_RECORD_COUNT] | data[OFFSET_RECORD_COUNT + 1] << 8);
    m_free_space_in_bytes = static_cast<std::uint16_t>(data[OFFSET_FREE_SPACE] | data[OFFSET_FREE_SPACE + 1] << 8);
    m_last_addition_timestamp = static_cast<std::uint32_t>(data[OFFSET_LAST_ADDITION_TS]
                                                           | data[OFFSET_LAST_ADDITION_TS + 1] << 8
                                                           | data[OFFSET_LAST_ADDITION_TS + 2] << 16
                                                           | data[OFFSET_LAST_ADDITION_TS + 3] << 24);
    m_last_delete_timestamp = static_cast<std::uint32_t>(data[OFFSET_LAST_DELETE_TS]
                                                           | data[OFFSET_LAST_DELETE_TS + 1] << 8
                                                           | data[OFFSET_LAST_DELETE_TS + 2] << 16
                                                           | data[OFFSET_LAST_DELETE_TS + 3] << 24);
    m_supported_operations = data[OFFSET_SUPPORTED_OPERATIONS];
}
