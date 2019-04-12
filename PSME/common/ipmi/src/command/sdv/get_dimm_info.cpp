/*!
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
 * @file get_dimm_info.cpp
 *
 * @brief ...
 * */
#include "ipmi/command/sdv/get_dimm_info.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetDimmInfo::GetDimmInfo(): Request(sdv::NetFn::QUANTA, sdv::Cmd::GET_DIMM_INFO) { }
request::GetDimmInfo::~GetDimmInfo() { }

void request::GetDimmInfo::pack(std::vector<std::uint8_t>& data) const {
    data.push_back(m_dimm_index);
}

response::GetDimmInfo::GetDimmInfo(): Response(sdv::NetFn::QUANTA, sdv::Cmd::GET_DIMM_INFO, ATOM_RESPONSE_SIZE) { }
response::GetDimmInfo::~GetDimmInfo() { }

void response::GetDimmInfo::unpack(const std::vector<std::uint8_t>& data) {
    if (is_xeon(data)) {
        xeon_unpack(data);
    }
    else if (is_atom(data)) {
        atom_unpack(data);
    }
    else {
        throw std::runtime_error(("Cannot unpack response. Data length is wrong."
                             " Expected: ") + std::to_string(XEON_RESPONSE_SIZE)
                           + " or " + std::to_string(ATOM_RESPONSE_SIZE)
                           + " Received: " + std::to_string(data.size()));
    }
}

bool response::GetDimmInfo::is_xeon(const std::vector<std::uint8_t>& data) const {
    if (XEON_RESPONSE_SIZE == data.size()) {
        return true;
    }
    return false;
}

bool response::GetDimmInfo::is_atom(const std::vector<std::uint8_t>& data) const {
    if (ATOM_RESPONSE_SIZE == data.size()) {
        return true;
    }
    return false;
}

void response::GetDimmInfo::xeon_unpack(const std::vector<std::uint8_t>& data) {
    m_dimm_presence = xeon_extract_presence(data);
    std::uint32_t size = xeon_extract_size(data);
    m_dimm_size_mbytes = size;

    common_unpack(data);
}

void response::GetDimmInfo::atom_unpack(const std::vector<std::uint8_t>& data) {
    m_dimm_presence = atom_extract_presence(data);
    std::uint16_t size = atom_extract_size(data);
    m_dimm_size_mbytes = size;

    common_unpack(data);
}

void response::GetDimmInfo::common_unpack(const std::vector<std::uint8_t>& data) {
    auto type = extract_type(data[OFFSET_DIMM_TYPE_AND_VOLTAGE]);

    // Changes to string.
    m_dimm_voltage = extract_voltage(data[OFFSET_DIMM_TYPE_AND_VOLTAGE]);
    m_dimm_type = type_to_string(type);
    m_dimm_speed_mhz = extract_speed(data);
}

std::uint8_t response::GetDimmInfo::extract_type(std::uint8_t value) const {
    return (MASK_DIMM_TYPE & value);
}

double response::GetDimmInfo::extract_voltage(std::uint8_t value) const {
    auto number = (MASK_DIMM_VOLTAGE & value) >> 6;

    if (m_voltage_mapping.find(DIMM_VOLTAGE(number)) == m_voltage_mapping.end()) {
        return m_voltage_mapping.at(DIMM_VOLTAGE_UNKNOWN);
    }
    return m_voltage_mapping.at(DIMM_VOLTAGE(number));
}

std::string response::GetDimmInfo::type_to_string(std::uint8_t value) const {
    if (m_type_mapping.find(DIMM_TYPE(value)) == m_type_mapping.end()) {
        return m_type_mapping.at(DIMM_TYPE_UNKNOWN);
    }
    return m_type_mapping.at(DIMM_TYPE(value));
}

std::uint32_t response::GetDimmInfo::extract_speed(const std::vector<std::uint8_t>& data) const {
    auto speed = std::uint32_t((data[OFFSET_DIMM_SPEED + 1] << 8)
                          | data[OFFSET_DIMM_SPEED + 0] << 0);
    return speed;
}

std::uint16_t response::GetDimmInfo::atom_extract_size(const std::vector<std::uint8_t>& data) const {
    std::uint32_t size = std::uint32_t(data[OFFSET_DIMM_SIZE + 1] << 8)
                  | std::uint32_t(data[OFFSET_DIMM_SIZE + 0] << 0);

    return std::uint16_t(size);
}

std::uint32_t response::GetDimmInfo::xeon_extract_size(const std::vector<std::uint8_t>& data) const {
    auto size = (data[OFFSET_DIMM_SIZE + 3] << 24)
               | (data[OFFSET_DIMM_SIZE + 2] << 16)
               | (data[OFFSET_DIMM_SIZE + 1] << 8)
               | (data[OFFSET_DIMM_SIZE + 0] << 0);
    return std::uint32_t(size);
}

bool response::GetDimmInfo::atom_extract_presence(const std::vector<std::uint8_t>& data) const {
    if (DIMM_PRESENCE_PRESENT == data[ATOM_OFFSET_PRESENCE]) {
        return true;
    }
    return false;
}

bool response::GetDimmInfo::xeon_extract_presence(const std::vector<std::uint8_t>& data) const {
    if (DIMM_PRESENCE_PRESENT == data[XEON_OFFSET_PRESENCE]) {
        return true;
    }
    return false;
}
