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
 * @file get_device_guid.cpp
 *
 * @brief Implementation of GetDeviceGuid Request/Response.
 * */

#include "ipmi/command/generic/get_device_guid.hpp"
#include "ipmi/command/generic/enums.hpp"

#include <sstream>
#include <iomanip>

using namespace ipmi;
using namespace ipmi::command::generic;

request::GetDeviceGuid::GetDeviceGuid(): Request(generic::NetFn::APP, generic::Cmd::GET_DEVICE_GUID) {}
request::GetDeviceGuid::~GetDeviceGuid() {}

void request::GetDeviceGuid::pack(std::vector<std::uint8_t>& data) const {
    (void)data;
}

response::GetDeviceGuid::GetDeviceGuid(): Response(generic::NetFn::APP, generic::Cmd::GET_DEVICE_GUID, RESPONSE_SIZE) {}
response::GetDeviceGuid::~GetDeviceGuid() {}


void response::GetDeviceGuid::unpack(const std::vector<std::uint8_t>& data) {

    char guid[GUID_LENGTH];

    int result = std::snprintf(&guid[0], GUID_LENGTH,
                               "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                               static_cast<unsigned int>(data[OFFSET_TIME_LOW + 3]),
                               static_cast<unsigned int>(data[OFFSET_TIME_LOW + 2]),
                               static_cast<unsigned int>(data[OFFSET_TIME_LOW + 1]),
                               static_cast<unsigned int>(data[OFFSET_TIME_LOW]),
                               static_cast<unsigned int>(data[OFFSET_TIME_MID + 1]),
                               static_cast<unsigned int>(data[OFFSET_TIME_MID]),
                               static_cast<unsigned int>(data[OFFSET_TIME_HIGH_AND_VER + 1]),
                               static_cast<unsigned int>(data[OFFSET_TIME_HIGH_AND_VER]),
                               static_cast<unsigned int>(data[OFFSET_CLK_SEQ_AND_RESERVED]),
                               static_cast<unsigned int>(data[OFFSET_CLK_SEQ_AND_RESERVED + 1]),
                               static_cast<unsigned int>(data[OFFSET_NODE]),
                               static_cast<unsigned int>(data[OFFSET_NODE + 1]),
                               static_cast<unsigned int>(data[OFFSET_NODE + 2]),
                               static_cast<unsigned int>(data[OFFSET_NODE + 3]),
                               static_cast<unsigned int>(data[OFFSET_NODE + 4]),
                               static_cast<unsigned int>(data[OFFSET_NODE + 5]));

    guid[GUID_LENGTH - 1] = '\0';
    if (result == GUID_LENGTH - 1) {
        m_guid = std::string(guid);
    }
}

