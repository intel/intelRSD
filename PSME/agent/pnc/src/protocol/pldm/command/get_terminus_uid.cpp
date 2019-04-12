/*!
* @brief GetTerminusUid PLDM command definition
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
* @file get_terminus_uid.cpp
*/

#include "protocol/pldm/command/get_terminus_uid.hpp"
#include "utils/conversion.hpp"



using namespace agent::pnc::gas;
using namespace agent::pnc::i2c;
using namespace agent::pnc::pldm::command;
using namespace agent::pnc::pldm;


GetTerminusUid::~GetTerminusUid() {}


ByteBuffer GetTerminusUid::build_request(PldmTool& pldm_tool) {

    auto req_bytes = pldm_tool.get_common_request(PldmType::PldmMessagingControlAndDiscovery,
                                                  pldm::PldmCommand::GetTerminusUid);
    pldm_tool.update_data_integrity(req_bytes);
    return req_bytes;
}


PldmCompletionCode
GetTerminusUid::handle_response(ByteBuffer& smbus_frame, PldmTool& pldm_tool) {

    pldm::PldmUuid data;
    auto completion_code = pldm_tool.extract_fixed_size_pldm_response(smbus_frame, reinterpret_cast<uint8_t*>(&data), sizeof(data));
    m_uuid = utils::buffer_to_string(reinterpret_cast<uint8_t*>(&data), sizeof(data));

    return completion_code;
}


void GetTerminusUid::log_command_result(const PM85X6TwiPort port, const PCA9548TwiExpanderChannel channel) {
    PldmCommand::log_command_result(port, channel);
    log_debug("pnc-pldm", "Terminus UID: " + m_uuid);
}
