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
 * @file get_cable_id.cpp
 *
 * @brief Get Cable Id request and response.
 * */
#include "ipmi/command/sdv/get_cable_id.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include <iostream>

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetCableId::GetCableId() : Request(sdv::NetFn::RACKSCALE, sdv::Cmd::GET_CABLE_ID) {}
request::GetCableId::~GetCableId() {}

void request::GetCableId::pack(std::vector<std::uint8_t>& data) const {
    data.push_back(uint8_t(CHANNEL));
    data.push_back(uint8_t(OFFSET_CABLE_ID));
    data.push_back(uint8_t(CABLE_ID_LENGTH));
}

void request::GetCableId::set_channel(size_t channel) {
    CHANNEL = channel;
}

response::GetCableId::GetCableId(): Response(sdv::NetFn::RACKSCALE, sdv::Cmd::GET_CABLE_ID, RESPONSE_SIZE) {}
response::GetCableId::~GetCableId() {}

void response::GetCableId::unpack(const std::vector<std::uint8_t>& data) {
    m_cable_id = extract_cable_id(data);
}

std::string response::GetCableId::extract_cable_id(const std::vector<std::uint8_t>& data) {
    char cable_id_array[] = {
            char(data[OFFSET_CABLE_ID + 0]),
            char(data[OFFSET_CABLE_ID + 1]),
            char(data[OFFSET_CABLE_ID + 2]),
            char(data[OFFSET_CABLE_ID + 3]),
            char(data[OFFSET_CABLE_ID + 4]),
            char(data[OFFSET_CABLE_ID + 5]),
            char(data[OFFSET_CABLE_ID + 6]),
            char(data[OFFSET_CABLE_ID + 7]),
            char(data[OFFSET_CABLE_ID + 8]),
            char(data[OFFSET_CABLE_ID + 9]),
            char(data[OFFSET_CABLE_ID + 10]),
            char(data[OFFSET_CABLE_ID + 11]),
            char(data[OFFSET_CABLE_ID + 12]),
            char(data[OFFSET_CABLE_ID + 13]),
            char(data[OFFSET_CABLE_ID + 14]),
            char(data[OFFSET_CABLE_ID + 15]),
            '\0'
    };

    std::string cable_id(cable_id_array);

    // trim spaces from the right
    cable_id.erase(cable_id.find_last_not_of(' ') + 1);
    return cable_id;
}
