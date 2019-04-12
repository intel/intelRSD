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
 * @file get_system_mac_address_haswell.cpp
 *
 * @brief Implementation of GetSystemMacAddressHaswell Request/Response.
 * */

#include "ipmi/command/sdv/get_system_mac_address_haswell.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetSystemMacAddressHaswell::GetSystemMacAddressHaswell() :
        Request(sdv::NetFn::QUANTA, sdv::Cmd::GET_SYSTEM_MAC_INFO) {}
request::GetSystemMacAddressHaswell::~GetSystemMacAddressHaswell() {}

void request::GetSystemMacAddressHaswell::pack(std::vector<std::uint8_t>& data) const {
    std::uint8_t interface_id_high = std::uint8_t((m_interface_id & 0xFF00) >> 8);
    std::uint8_t interface_id_low = std::uint8_t(m_interface_id & 0x00FF);
    data.push_back(interface_id_high);
    data.push_back(interface_id_low);
}

response::GetSystemMacAddressHaswell::GetSystemMacAddressHaswell() :
        Response(sdv::NetFn::QUANTA, sdv::Cmd::GET_SYSTEM_MAC_INFO, RESPONSE_SIZE) {}
response::GetSystemMacAddressHaswell::~GetSystemMacAddressHaswell() {}

void response::GetSystemMacAddressHaswell::unpack(const std::vector<std::uint8_t>& data) {
    char mac_address[MAC_ADDRESS_LENGTH];
    int result = std::snprintf(&mac_address[0],
                               MAC_ADDRESS_LENGTH ,
                               "%02x:%02x:%02x:%02x:%02x:%02x",
                    static_cast<unsigned int>(data[MAC_ADDRESS_OFFSET + 0]),
                    static_cast<unsigned int>(data[MAC_ADDRESS_OFFSET + 1]),
                    static_cast<unsigned int>(data[MAC_ADDRESS_OFFSET + 2]),
                    static_cast<unsigned int>(data[MAC_ADDRESS_OFFSET + 3]),
                    static_cast<unsigned int>(data[MAC_ADDRESS_OFFSET + 4]),
                    static_cast<unsigned int>(data[MAC_ADDRESS_OFFSET + 5]));

    mac_address[MAC_ADDRESS_LENGTH - 1] = '\0';
    if (result == MAC_ADDRESS_LENGTH - 1) {
        m_system_mac_address = std::string(mac_address);
    }
}

