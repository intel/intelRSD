/*!
 * @brief Set OOB Boot Options Command Implementation.
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
 * @file set_oob_boot_option.cpp
 */

#include "ipmi/command/sdv/iscsi_oob_boot/set_oob_boot_options.hpp"
#include "ipmi/command/sdv/enums.hpp"

extern "C" {
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
}

#include <algorithm>


using namespace ipmi;
using namespace ipmi::command;
using namespace ipmi::command::sdv;

constexpr std::uint8_t request::SetOobBootOptions::PARAMETER_VERSION;
constexpr std::uint8_t request::SetOobBootOptions::PARAMETERS_VALID;
constexpr std::uint8_t request::SetOobBootOptions::PARAMETERS_UNAVAILABLE;


request::SetOobBootOptions::SetOobBootOptions(ParameterType id) :
    Request(sdv::NetFn::RACKSCALE, sdv::Cmd::SET_OOB_BOOT_OPTION) {

    m_id = uint8_t(id);
}


request::SetOobBootOptions::~SetOobBootOptions() {}


uint8_t request::SetOobBootOptions::get_parameter_id() const {
    uint8_t parameter_id = uint8_t(m_id);

    if (m_param_valid_flag) {
        parameter_id |= PARAMETERS_VALID;
    }

    return parameter_id;
}


void request::SetOobBootOptions::push_uint16(std::vector<uint8_t>& output, uint16_t input) const {
    for (size_t i = 0; i < sizeof(input); ++i) {
        output.push_back(uint8_t(input & 0xFF));
        input = uint16_t(input >> 8);
    }
}


void request::SetOobBootOptions::push_uint32(std::vector<uint8_t>& output, uint32_t input) const {
    for (size_t i = 0; i < sizeof(input); ++i) {
        output.push_back(uint8_t(input & 0xFF));
        input >>= 8;
    }
}


void request::SetOobBootOptions::push_uint64(std::vector<uint8_t>& output, uint64_t input) const {
    for (size_t i = 0; i < sizeof(input); ++i) {
        output.push_back(uint8_t(input & 0xFF));
        input >>= 8;
    }
}


void request::SetOobBootOptions::push_ip(std::vector<uint8_t>& output, std::string input) const {
    if (input.empty()) {
        // if an IP address was not set in request, send all zeros
        for (size_t i = 0; i < request::SetOobBootOptions::IP_BYTES; ++i) {
            output.push_back(0);
        }
        return;
    }

    static constexpr const int INET_PTON_SUCCESS{1};
    struct in_addr ipv4_address;
    if (inet_pton(AF_INET, input.c_str(), &ipv4_address) == INET_PTON_SUCCESS) {
        push_uint32(output, ipv4_address.s_addr);
        // if an IPv4 address is sent, fill rest of field with zeros
        for (size_t i = 0; i < request::SetOobBootOptions::IP_BYTES - sizeof(in_addr); ++i) {
            output.push_back(0);
        }
        return;
    }
    struct in6_addr ipv6_address;
    if (inet_pton(AF_INET6, input.c_str(), &ipv6_address) == INET_PTON_SUCCESS) {
        for (size_t i = 0; i < request::SetOobBootOptions::IP_BYTES; ++i) {
            output.insert(output.end(), ipv6_address.s6_addr, ipv6_address.s6_addr + request::SetOobBootOptions::IP_BYTES);
        }
        return;
    }
    throw std::runtime_error("Set OOB Boot Options: attempted to send an invalid IP address!");
}


void request::SetOobBootOptions::push_mac(std::vector<uint8_t>& output, std::string input) const {
    // according to metadata, MAC address can use both colons and dashes: aa:bb-cc:dd-ee:ff
    // however, ether_aton function only accepts notation with colons.
    // So, we convert the dashes to colons before checking if the address is correct
    std::replace(input.begin(), input.end(), '-', ':');

    const struct ether_addr* address = ether_aton(input.c_str());
    if (address == NULL) {
        throw std::runtime_error("Set OOB Boot Options: attempted to send an invalid MAC address!");
    } else {
        for (int i = 0; i < request::SetOobBootOptions::MAC_BYTES; ++i) {
            output.push_back(uint8_t(address->ether_addr_octet[i]));
        }
    }
}


response::SetOobBootOptions::SetOobBootOptions() :
    Response(sdv::NetFn::RACKSCALE, sdv::Cmd::SET_OOB_BOOT_OPTION, RESPONSE_SIZE) {}


response::SetOobBootOptions::~SetOobBootOptions() {}

