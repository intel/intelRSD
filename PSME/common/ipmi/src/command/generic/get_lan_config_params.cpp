/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file get_lan_config_params.cpp
 *
 * @brief IPMI Get LAN configuration parameters command request/response implementation.
 * */

#include "ipmi/command/generic/get_lan_config_params.hpp"

#include <exception>

#include <sstream>
#include <iomanip>



using namespace ipmi;
using namespace ipmi::command::generic;


request::GetLanConfigParams::GetLanConfigParams(std::uint8_t parameter_selector) :
    Request(generic::NetFn::TRANSPORT, generic::Cmd::GET_LAN_CONFIG_PARAMS),
    m_parameter_selector{parameter_selector} {}


void request::GetLanConfigParams::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(m_channel_get_revision);
    data.push_back(m_parameter_selector);
    data.push_back(m_set_selector);
    data.push_back(m_block_selector);
}


void request::GetLanConfigParams::select_revision_only() {
    m_channel_get_revision |= CHANNEL_MASK;
}


void request::GetLanConfigParams::set_channel(std::uint8_t channel) {
    m_channel_get_revision = std::uint8_t(m_channel_get_revision | (channel & CHANNEL_MASK));
}


request::GetLanIpAddress::GetLanIpAddress() : GetLanConfigParams(PARAM_INDEX) {}


request::GetLanIpAddress::~GetLanIpAddress() {}


request::GetLanIpAddressSource::GetLanIpAddressSource() : GetLanConfigParams(PARAM_INDEX) {}


request::GetLanIpAddressSource::~GetLanIpAddressSource() {}


request::GetLanMacAddress::GetLanMacAddress() : GetLanConfigParams(PARAM_INDEX) {}


request::GetLanMacAddress::~GetLanMacAddress() {}


request::GetLanNetMask::GetLanNetMask() : GetLanConfigParams(PARAM_INDEX) {}


request::GetLanNetMask::~GetLanNetMask() {}


request::GetLanGateway::GetLanGateway() : GetLanConfigParams(PARAM_INDEX) {}


request::GetLanGateway::~GetLanGateway() {}



response::GetLanConfigParams::GetLanConfigParams(const std::size_t response_size) :
    Response(generic::NetFn::TRANSPORT + 1, generic::Cmd::GET_LAN_CONFIG_PARAMS, response_size) {}


response::GetLanConfigParams::~GetLanConfigParams() {}


void response::GetLanConfigParams::unpack(const IpmiInterface::ByteBuffer& data) {
    response = data;
}


response::GetLanIpAddress::GetLanIpAddress() : GetLanConfigParams(RESP_SIZE) {
    response.assign(RESP_SIZE, 0);
}


response::GetLanIpAddress::~GetLanIpAddress() {}


std::string response::GetLanIpAddress::get_ip_address() const {
    std::ostringstream ret;
    ret << unsigned(response[DATA_OFFSET]) << "."
        << unsigned(response[DATA_OFFSET + 1]) << "."
        << unsigned(response[DATA_OFFSET + 2]) << "."
        << unsigned(response[DATA_OFFSET + 3]);
    return ret.str();
}


response::GetLanIpAddressSource::GetLanIpAddressSource() : GetLanConfigParams(RESP_SIZE) {
    response.assign(RESP_SIZE, 0);
}


response::GetLanIpAddressSource::~GetLanIpAddressSource() {}


LanConfigIpAddressSource response::GetLanIpAddressSource::get_ip_address_source() const {
    return static_cast<LanConfigIpAddressSource>(response[DATA_OFFSET] & ADDRESS_SOURCE_MASK);
}


response::GetLanMacAddress::GetLanMacAddress() : GetLanConfigParams(RESP_SIZE) {
    response.assign(RESP_SIZE, 0);
}


response::GetLanMacAddress::~GetLanMacAddress() {}


std::string response::GetLanMacAddress::get_mac_address() const {
    std::ostringstream ret;
    ret << std::hex << std::setfill('0')
        << std::setw(2) << unsigned(response[DATA_OFFSET]) << ":"
        << std::setw(2) << unsigned(response[DATA_OFFSET + 1]) << ":"
        << std::setw(2) << unsigned(response[DATA_OFFSET + 2]) << ":"
        << std::setw(2) << unsigned(response[DATA_OFFSET + 3]) << ":"
        << std::setw(2) << unsigned(response[DATA_OFFSET + 4]) << ":"
        << std::setw(2) << unsigned(response[DATA_OFFSET + 5]);
    return ret.str();
}


response::GetLanNetmask::GetLanNetmask() : GetLanConfigParams(RESP_SIZE) {}


response::GetLanNetmask::~GetLanNetmask() {}


std::string response::GetLanNetmask::get_netmask() const {
    std::ostringstream ret;
    ret << unsigned(response[DATA_OFFSET]) << "."
        << unsigned(response[DATA_OFFSET + 1]) << "."
        << unsigned(response[DATA_OFFSET + 2]) << "."
        << unsigned(response[DATA_OFFSET + 3]);
    return ret.str();
}


response::GetLanGateway::GetLanGateway() : GetLanConfigParams(RESP_SIZE) {}


response::GetLanGateway::~GetLanGateway() {}


std::string response::GetLanGateway::get_gateway() const {
    std::ostringstream ret;
    ret << unsigned(response[DATA_OFFSET]) << "."
        << unsigned(response[DATA_OFFSET + 1]) << "."
        << unsigned(response[DATA_OFFSET + 2]) << "."
        << unsigned(response[DATA_OFFSET + 3]);
    return ret.str();
}
