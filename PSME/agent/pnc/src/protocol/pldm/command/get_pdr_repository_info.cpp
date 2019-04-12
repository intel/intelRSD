/*!
* @brief GetPdrRepositoryInfo PLDM command definition
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
* @file get_pdr_repository_info.cpp
*/

#include "protocol/pldm/command/get_pdr_repository_info.hpp"



using namespace agent::pnc::gas;
using namespace agent::pnc::i2c;
using namespace agent::pnc::pldm::command;
using namespace agent::pnc::pldm;


GetPdrRepositoryInfo::~GetPdrRepositoryInfo() {}


ByteBuffer GetPdrRepositoryInfo::build_request(PldmTool& pldm_tool) {

    auto req_bytes = pldm_tool.get_common_request(
        PldmType::PldmForPlatformMonitoringAndControlSpecification,
        pldm::PldmCommand::GetPdrRepositoryInfo);
    pldm_tool.update_data_integrity(req_bytes);
    return req_bytes;
}


PldmCompletionCode
GetPdrRepositoryInfo::handle_response(ByteBuffer& smbus_frame, PldmTool& pldm_tool) {

    auto completion_code = pldm_tool.extract_fixed_size_pldm_response(smbus_frame,
                                                                      reinterpret_cast<uint8_t*>(&pdr_repository_info),
                                                                      sizeof(pdr_repository_info));
    return completion_code;
}


void GetPdrRepositoryInfo::log_command_result(const PM85X6TwiPort port, const PCA9548TwiExpanderChannel channel) {
    PldmCommand::log_command_result(port, channel);
    log_debug("pnc-pldm", "PDR Repository Info:");
    log_debug("pnc-pldm", "record count (4): " + std::to_string(pdr_repository_info.record_count));
    log_debug("pnc-pldm", "pdr_repository_info.data_transfer_handle_timeout (1): " +
                          std::to_string((unsigned) pdr_repository_info.data_transfer_handle_timeout));
    log_debug("pnc-pldm", "pdr_repository_info.largest_record_size (4): " +
                          std::to_string((unsigned) pdr_repository_info.largest_record_size));
    log_debug("pnc-pldm", "pdr_repository_info.oem_update_time.day (1): " +
                          std::to_string((unsigned) pdr_repository_info.oem_update_time.day));
    log_debug("pnc-pldm", "pdr_repository_info.oem_update_time.month (1): " +
                          std::to_string((unsigned) pdr_repository_info.oem_update_time.month));
    log_debug("pnc-pldm", "pdr_repository_info.oem_update_time.year (1): " +
                          std::to_string((unsigned) pdr_repository_info.oem_update_time.year));
    log_debug("pnc-pldm", "pdr_repository_info.oem_update_time.hour (1): " +
                          std::to_string((unsigned) pdr_repository_info.oem_update_time.hour));
    log_debug("pnc-pldm", "pdr_repository_info.oem_update_time.minute (1): " +
                          std::to_string((unsigned) pdr_repository_info.oem_update_time.minute));
    log_debug("pnc-pldm", "pdr_repository_info.oem_update_time.seconds (1): " +
                          std::to_string((unsigned) pdr_repository_info.oem_update_time.seconds));

}
