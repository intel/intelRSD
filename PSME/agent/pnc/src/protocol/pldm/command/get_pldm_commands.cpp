/*!
* @brief GetPldmCommands PLDM command definition
*
* @copyright Copyright (c) 2019 Intel Corporation.
*
* Licensed under the Apache License, Version 2.0 (the "License") override;
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License
*
* @file get_pldm_commands.cpp
*/

#include "protocol/pldm/command/get_pldm_commands.hpp"
#include "utils/conversion.hpp"



using namespace agent::pnc::gas;
using namespace agent::pnc::i2c;
using namespace agent::pnc::pldm::command;
using namespace agent::pnc::pldm;


GetPldmCommands::~GetPldmCommands() {}


ByteBuffer GetPldmCommands::build_request(PldmTool& pldm_tool) {
    auto req_bytes = pldm_tool.get_common_request(PldmType::PldmMessagingControlAndDiscovery,
                                                  pldm::PldmCommand::GetPldmCommands);
    req_bytes.push_back(m_pldm_type);
    pldm_tool.pack_bytes(req_bytes, reinterpret_cast<uint8_t*>(&m_pldm_version), sizeof(m_pldm_version));
    pldm_tool.update_data_integrity(req_bytes);
    return req_bytes;
}


PldmCompletionCode
GetPldmCommands::handle_response(ByteBuffer& smbus_frame, PldmTool& pldm_tool) {

    auto completion_code = pldm_tool.extract_fixed_size_pldm_response(smbus_frame,
                                                                      reinterpret_cast<uint8_t*>(&m_pldm_commands),
                                                                      sizeof(m_pldm_commands));
    return completion_code;
}


void GetPldmCommands::log_command_result(const PM85X6TwiPort port, const PCA9548TwiExpanderChannel channel) {
    PldmCommand::log_command_result(port, channel);
    std::string commands_hexdump = utils::buffer_to_string(m_pldm_commands,
                                                           sizeof(m_pldm_commands) / sizeof(m_pldm_commands[0]));

    log_debug("pnc-pldm", "PLDM Commands hexdump: " + commands_hexdump);
}
