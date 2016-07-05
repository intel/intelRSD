/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file get_dimm_info.cpp
 *
 * @brief ...
 * */
#include "ipmi/command/sdv/get_dimm_info.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetDimmInfo::GetDimmInfo(): Request(uint8_t(NetFn::QUANTA), uint8_t(Cmd::GET_DIMM_INFO)) {

}
request::GetDimmInfo::~GetDimmInfo() {}

void request::GetDimmInfo::pack(vector<uint8_t>& data) const {
    data.push_back(m_dimm_index);
}

response::GetDimmInfo::GetDimmInfo(): Response(uint8_t(NetFn::QUANTA),
                                               uint8_t(Cmd::GET_DIMM_INFO), ATOM_RESPONSE_SIZE) {}
response::GetDimmInfo::~GetDimmInfo() {
}

void response::GetDimmInfo::unpack(const vector<uint8_t>& data) {
    if (is_xeon(data)) {
        xeon_unpack(data);
    }
    else if (is_atom(data)) {
        atom_unpack(data);
    }
    else if (is_error(data)) {
        m_completion_code = COMPLETION_CODE(data[OFFSET_COMPLETION_CODE]);
    }
    else {
        throw runtime_error(("Cannot unpack response. Data length too short."
                             " Expected: ")
                           + to_string(XEON_RESPONSE_SIZE)
                           + " or "
                           + to_string(ATOM_RESPONSE_SIZE)
                           + " Received: "
                           + to_string(data.size()));
    }
}

bool response::GetDimmInfo::is_xeon(const vector<uint8_t>& data) const {
    if (XEON_RESPONSE_SIZE == data.size()) {
        return true;
    }
    return false;
}

bool response::GetDimmInfo::is_atom(const vector<uint8_t>& data) const {
    if (ATOM_RESPONSE_SIZE == data.size()) {
        return true;
    }
    return false;
}

bool response::GetDimmInfo::is_error(const vector<uint8_t>& data) const {
    if (ERROR_DATA_SIZE == data.size()) {
        return true;
    }
    return false;
}

void response::GetDimmInfo::xeon_unpack(const vector<uint8_t>& data) {
    m_dimm_presence = xeon_extract_presence(data);
    uint32_t size = xeon_extract_size(data);
    m_dimm_size_mbytes = size;

    common_unpack(data);
}

void response::GetDimmInfo::atom_unpack(const vector<uint8_t>& data) {
    m_dimm_presence = atom_extract_presence(data);
    uint16_t size = atom_extract_size(data);
    m_dimm_size_mbytes = size;

    common_unpack(data);
}

void response::GetDimmInfo::common_unpack(const vector<uint8_t>& data) {
    m_completion_code = COMPLETION_CODE(data[OFFSET_COMPLETION_CODE]);

    auto type = extract_type(data[OFFSET_DIMM_TYPE_AND_VOLTAGE]);

    // Changes to string.
    m_dimm_voltage = extract_voltage(data[OFFSET_DIMM_TYPE_AND_VOLTAGE]);
    m_dimm_type = type_to_string(type);
    m_dimm_speed_mhz = extract_speed(data);
}

uint8_t response::GetDimmInfo::extract_type(uint8_t value) const {
    return (MASK_DIMM_TYPE & value);
}

double response::GetDimmInfo::extract_voltage(uint8_t value) const {
    auto number = (MASK_DIMM_VOLTAGE & value) >> 6;

    if (m_voltage_mapping.find(DIMM_VOLTAGE(number)) == m_voltage_mapping.end()) {
        return m_voltage_mapping.at(DIMM_VOLTAGE_UNKNOWN);
    }
    return m_voltage_mapping.at(DIMM_VOLTAGE(number));
}

string response::GetDimmInfo::type_to_string(uint8_t value) const {
    if (m_type_mapping.find(DIMM_TYPE(value)) == m_type_mapping.end()) {
        return m_type_mapping.at(DIMM_TYPE_UNKNOWN);
    }
    return m_type_mapping.at(DIMM_TYPE(value));
}

uint32_t response::GetDimmInfo::extract_speed(const vector<uint8_t>& data) const {
    auto speed = uint32_t((data[OFFSET_DIMM_SPEED + 1] << 8)
                          | data[OFFSET_DIMM_SPEED + 0] << 0);
    return speed;
}

uint16_t response::GetDimmInfo::atom_extract_size(const vector<uint8_t>& data) const {
    uint32_t size = uint32_t(data[OFFSET_DIMM_SIZE + 1] << 8)
                  | uint32_t(data[OFFSET_DIMM_SIZE + 0] << 0);

    return uint16_t(size);
}

uint32_t response::GetDimmInfo::xeon_extract_size(const vector<uint8_t>& data) const {
    auto size =  (data[OFFSET_DIMM_SIZE + 3] << 24)
               | (data[OFFSET_DIMM_SIZE + 2] << 16)
               | (data[OFFSET_DIMM_SIZE + 1] << 8)
               | (data[OFFSET_DIMM_SIZE + 0] << 0);
    return uint32_t(size);
}

bool response::GetDimmInfo::atom_extract_presence(const vector<uint8_t>& data) const {
    if (DIMM_PRESENCE_PRESENT == data[ATOM_OFFSET_PRESENCE]) {
        return true;
    }
    return false;
}

bool response::GetDimmInfo::xeon_extract_presence(const vector<uint8_t>& data) const {
    if (DIMM_PRESENCE_PRESENT == data[XEON_OFFSET_PRESENCE]) {
        return true;
    }
    return false;
}
