/*!
* @brief GetPDR PLDM command definition
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
* @file get_pdr.cpp
*/

#include "protocol/pldm/command/get_pdr.hpp"



using namespace agent::pnc::gas;
using namespace agent::pnc::i2c;
using namespace agent::pnc::pldm::command;
using namespace agent::pnc::pldm;


ByteBuffer GetPdr::build_request(PldmTool& pldm_tool) {

    auto req_bytes = pldm_tool.get_common_request(
        PldmType::PldmForPlatformMonitoringAndControlSpecification,
        pldm::PldmCommand::GetPdr);

    pldm::RecordHandle record_handle = get_pdr_record_handle();
    pldm_tool.pack_bytes(req_bytes, reinterpret_cast<uint8_t*>(&record_handle), sizeof(record_handle));
    pldm_tool.pack_bytes(req_bytes, reinterpret_cast<uint8_t*>(&m_pdr_request_header.data_transfer_handle),
                         sizeof(m_pdr_request_header.data_transfer_handle));
    req_bytes.push_back(m_pdr_request_header.transfer_operational_flag);
    pldm_tool.pack_bytes(req_bytes, reinterpret_cast<uint8_t*>(&m_pdr_request_header.request_count),
                         sizeof(m_pdr_request_header.request_count));
    pldm_tool.pack_bytes(req_bytes, reinterpret_cast<uint8_t*>(&m_pdr_request_header.record_change_number),
                         sizeof(m_pdr_request_header.record_change_number));

    pldm_tool.update_data_integrity(req_bytes);
    return req_bytes;
}


PldmCompletionCode
GetPdr::handle_response(ByteBuffer& smbus_frame, PldmTool& pldm_tool) {

    auto completion_code = pldm_tool.extract_number_of_bytes_from_pldm_response(smbus_frame,
                                                                                reinterpret_cast<uint8_t*>(&m_pdr_response_header),
                                                                                sizeof(m_pdr_response_header));
    return completion_code;
}


void GetPdr::log_command_result(const gas::PM85X6TwiPort port, const gas::PCA9548TwiExpanderChannel channel) {
    log_debug("pnc-pldm", "PLDM command " << this->get_command_name() << " for FPGA connected to TWI Port: "
                                          << std::uint32_t(port) << " TWI Channel: " << std::uint32_t(channel)
                                          << " succesfully executed.");
}