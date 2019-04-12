/*!
 * @brief GetSystemMode command implementation.
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
 * @file get_system_mode.cpp
 */


#include "ipmi/command/sdv/rsd/get_system_mode.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"

#include <exception>



using namespace ipmi;
using namespace ipmi::command::sdv::rsd;


request::GetSystemMode::GetSystemMode() : Request(NetFn::GROUP_EXTENSION, Cmd::GET_SYSTEM_MODE) {}


request::GetSystemMode::~GetSystemMode() {}


void request::GetSystemMode::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(static_cast<uint8_t>(GroupExtension::RACKSCALE));
}


response::GetSystemMode::GetSystemMode() : Response(NetFn::GROUP_EXTENSION + 1, Cmd::GET_SYSTEM_MODE, RESPONSE_SIZE) {}


response::GetSystemMode::~GetSystemMode() {}


void response::GetSystemMode::unpack(const IpmiInterface::ByteBuffer& data) {
    if (static_cast<uint8_t>(GroupExtension::RACKSCALE) == data[constants::GROUP_EXTENSION_ID_OFFSET]) {
        m_is_rackscale_extension_byte_present = true;
    }

    m_admin_mode = static_cast<SystemMode>(data[ADMIN_MODE_OFFSET]);
    m_is_rack_scale_mode_enabled = data[RACKSCALE_MODE_OFFSET] ? true : false;
}
