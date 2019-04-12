/*!
 * @brief Get OOB Boot Options Command Implementation.
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file get_oob_boot_option.cpp
 */

#include "ipmi/command/sdv/iscsi_oob_boot/get_oob_boot_options.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/errors.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include <sstream>
#include <iomanip>



using namespace ipmi;
using namespace ipmi::command;
using namespace ipmi::command::sdv;


request::GetOobBootOptions::GetOobBootOptions(ParameterType id, uint8_t index) :
    Request(sdv::NetFn::RACKSCALE, sdv::Cmd::GET_OOB_BOOT_OPTION) {

    m_id = uint8_t(id);
    m_index = index;
}


request::GetOobBootOptions::~GetOobBootOptions() {}


response::GetOobBootOptions::GetOobBootOptions() :
    Response(sdv::NetFn::RACKSCALE, sdv::Cmd::GET_OOB_BOOT_OPTION, MIN_RESPONSE_SIZE) {}


response::GetOobBootOptions::~GetOobBootOptions() {}


bool response::GetOobBootOptions::valid_basic_parameter(const std::vector<uint8_t>& data) {
    set_max_length(pull_uint16(data, OFFSET_MAX_SIZE));
    set_length(pull_uint16(data, OFFSET_CUR_SIZE));
    set_version(data[OFFSET_VERSION]);
    set_index(data[OFFSET_INDEX]);

    if (get_version() != PARAMETER_VERSION) {
        throw InvalidParameterVersion(*this);
    }

    size_t data_size = data.size();
    if (data_size != v2_payload_size()) {
        throw InvalidIscsiOobResponseLength(*this);
    }

    return true;
}


void request::GetOobBootOptions::push_uint16(std::vector<uint8_t>& output, uint16_t input) const {
    for (size_t i = 0; i < sizeof(input); ++i) {
        output.push_back(uint8_t(input & 0xFF));
        input = uint16_t(input >> 8);
    }
}


uint64_t response::GetOobBootOptions::pull_uint64(const std::vector<uint8_t>& data, size_t offset) const {

    uint64_t val{0};

    for (size_t i = 0; i < sizeof(uint64_t); i++) {

        uint64_t byte = data[offset + i] << (8 * i);
        val |= byte;
    }

    return val;
}


std::string response::GetOobBootOptions::pull_ip(const std::vector<uint8_t>& data, size_t offset) const {

    std::stringstream ss;

    ss << unsigned(data[offset]) << "." << unsigned(data[offset + 1]) << "." << unsigned(data[offset + 2]) << "."
       << unsigned(data[offset + 3]);

    return ss.str();
}


std::string response::GetOobBootOptions::pull_mac(const std::vector<uint8_t>& data, size_t offset) const {

    std::stringstream ss;

    ss << std::hex;
    ss << std::setfill('0') << std::setw(2) << unsigned(data[offset]);
    for (size_t index = 1; index < MAC_BYTES; index++) {
        ss << std::setw(1) << ":" << std::setfill('0') << std::setw(2) << unsigned(data[offset + index]);
    }

    return ss.str();
}
