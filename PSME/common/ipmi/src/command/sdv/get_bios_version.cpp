/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @section DESCRIPTION
 *
 * @file get_bios_version.cpp
 *
 * @brief Get Bios Version request and response.
 * */
#include "ipmi/command/sdv/get_bios_version.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetBiosVersion::GetBiosVersion() :
        Request(sdv::NetFn::QUANTA, sdv::Cmd::GET_BIOS_VERSION) {}
request::GetBiosVersion::~GetBiosVersion() {}

response::GetBiosVersion::GetBiosVersion() :
        Response(sdv::NetFn::QUANTA, sdv::Cmd::GET_BIOS_VERSION, RESPONSE_SIZE) {}
response::GetBiosVersion::~GetBiosVersion() {}

void response::GetBiosVersion::unpack(const std::vector<std::uint8_t>& data) {
    m_full_version = extract_bios_version(data);
}

std::string response::GetBiosVersion::extract_bios_version(const std::vector<std::uint8_t>& data) {
    char version_array[] = {
        char(data[OFFSET_ASCII_CODE + 0]),
        char(data[OFFSET_ASCII_CODE + 1]),
        char(data[OFFSET_ASCII_CODE + 2]),
        char(data[OFFSET_ASCII_CODE + 3]),
        char(data[OFFSET_VERSION + 0]),
        char(data[OFFSET_VERSION + 1]),
        char(data[OFFSET_VERSION + 2]),
        char(data[OFFSET_VERSION + 3]),
        char(data[OFFSET_VERSION + 4]),
        char(data[OFFSET_VERSION + 5]),
        char(data[OFFSET_VERSION + 6]),
        char(data[OFFSET_VERSION + 7]),
        char(data[OFFSET_VERSION + 8]),
        char(data[OFFSET_VERSION + 9]),
        char(data[OFFSET_VERSION + 10]),
        char(data[OFFSET_VERSION + 11]),
        '\0'
    };

    std::string version(version_array);
    return version;
}

