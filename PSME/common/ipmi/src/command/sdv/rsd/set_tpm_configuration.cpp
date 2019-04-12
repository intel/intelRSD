/*!
 * @brief SetTpmConfiguration command implementation.
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
 * @file set_tpm_configuration.cpp
 */

#include "ipmi/command/sdv/rsd/set_tpm_configuration.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv::rsd;


request::SetTpmConfiguration::SetTpmConfiguration() : Request(NetFn::GROUP_EXTENSION, Cmd::SET_TPM_CONFIGURATION) {}


request::SetTpmConfiguration::~SetTpmConfiguration() {}


void request::SetTpmConfiguration::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(static_cast<uint8_t>(GroupExtension::RACKSCALE));
    data.push_back(static_cast<std::uint8_t>(m_tpm_command_status));
    data.push_back(m_tpm_version_selection);
    data.push_back(static_cast<std::uint8_t>(m_clear_tpm_action));
}


response::SetTpmConfiguration::SetTpmConfiguration() : Response(NetFn::GROUP_EXTENSION + 1, Cmd::SET_TPM_CONFIGURATION,
                                                                RESPONSE_SIZE) {}


response::SetTpmConfiguration::~SetTpmConfiguration() {}


void response::SetTpmConfiguration::unpack(const IpmiInterface::ByteBuffer& data) {
    if (static_cast<uint8_t>(GroupExtension::RACKSCALE) == data[constants::GROUP_EXTENSION_ID_OFFSET]) {
        m_is_rackscale_extension_byte_present = true;
    }

    m_tpm_command_status = static_cast<TpmCommandStatus>(data[TPM_COMMAND_STATUS_OFFSET]);
    m_tpm_version_selection = data[TPM_VERSION_SELECTION_OFFSET];
    m_clear_tpm_action = static_cast<ClearTpmAction>(data[CLEAR_TPM_ACTION_OFFSET]);
}
