/*!
 * @brief SetSpeedSelectConfiguration command interface.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file set_speed_select_configuration.cpp
 */

#include "ipmi/command/sdv/rsd/set_speed_select_configuration.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv::rsd;

request::SetSpeedSelectConfiguration::SetSpeedSelectConfiguration() : Request(sdv::NetFn::GROUP_EXTENSION, sdv::Cmd::SET_SPEED_SELECT_CONFIGURATION) {}
request::SetSpeedSelectConfiguration::~SetSpeedSelectConfiguration() {}

void request::SetSpeedSelectConfiguration::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(static_cast<uint8_t>(GroupExtension::RACKSCALE));
    data.push_back(m_node_index);
    data.push_back(m_cpu_index);
    data.push_back(m_configuration_index);
}

response::SetSpeedSelectConfiguration::SetSpeedSelectConfiguration() : Response(sdv::NetFn::GROUP_EXTENSION, sdv::Cmd::SET_SPEED_SELECT_CONFIGURATION, RESPONSE_SIZE) {}
response::SetSpeedSelectConfiguration::~SetSpeedSelectConfiguration() {}

void response::SetSpeedSelectConfiguration::unpack(const IpmiInterface::ByteBuffer& data) {
    if (static_cast<uint8_t>(GroupExtension::RACKSCALE) == data[constants::GROUP_EXTENSION_ID_OFFSET]) {
        m_is_rackscale_extension_byte_present = true;
    }

    m_node_index = uint8_t(data[NODE_INDEX_OFFSET]);
    m_cpu_index = uint8_t(data[CPU_INDEX_OFFSET]);
    m_configuration_index = uint8_t(data[CONFIGURATION_INDEX_OFFSET]);
}



