/*!
* @brief GetFRURecordSetPDR PLDM command definition
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
* @file get_fru_record_set_pdr.cpp
*/

#include "protocol/pldm/command/get_fru_record_set_pdr.hpp"



using namespace agent::pnc::gas;
using namespace agent::pnc::i2c;
using namespace agent::pnc::pldm::command;
using namespace agent::pnc::pldm;


GetFruRecordSetPdr::~GetFruRecordSetPdr() {}


PldmCompletionCode
GetFruRecordSetPdr::handle_response(ByteBuffer& smbus_frame, PldmTool& pldm_tool) {
    GetPdr::handle_response(smbus_frame, pldm_tool);
    auto get_pdr_response_offset = get_pdr_response_header_size();

    auto completion_code = pldm_tool.extract_number_of_bytes_from_pldm_response(smbus_frame,
                                                                                reinterpret_cast<uint8_t*>(&fru_record_set_pdr),
                                                                                sizeof(fru_record_set_pdr),
                                                                                get_pdr_response_offset);
    return completion_code;
}


void GetFruRecordSetPdr::log_command_result(const PM85X6TwiPort port, const PCA9548TwiExpanderChannel channel) {
    PldmCommand::log_command_result(port, channel);
    log_debug("pnc-pldm", "FRU Record Set PDR:");
    log_debug("pnc-pldm", "fru_record_set_pdr.common_pdr.record_handle (4):" +
                          std::to_string(fru_record_set_pdr.common_pdr.record_handle));
    log_debug("pnc-pldm", "fru_record_set_pdr.common_pdr.pdr_header_version (1):" +
                          std::to_string((unsigned) fru_record_set_pdr.common_pdr.pdr_header_version));
    log_debug("pnc-pldm", "fru_record_set_pdr.common_pdr.pdr_type (1):" +
                          std::to_string((unsigned) fru_record_set_pdr.common_pdr.pdr_type));
    log_debug("pnc-pldm", "fru_record_set_pdr.common_pdr.record_change_number (2):" +
                          std::to_string((unsigned) fru_record_set_pdr.common_pdr.record_change_number));
    log_debug("pnc-pldm", "fru_record_set_pdr.common_pdr.data_length (2):" +
                          std::to_string((unsigned) fru_record_set_pdr.common_pdr.data_length));
    log_debug("pnc-pldm", "fru_record_set_pdr.pldm_terminus_handle (2):" +
                          std::to_string((unsigned) fru_record_set_pdr.pldm_terminus_handle));
    log_debug("pnc-pldm", "fru_record_set_pdr.fru_record_set_indentifier (2):" +
                          std::to_string((unsigned) fru_record_set_pdr.fru_record_set_indentifier));
    log_debug("pnc-pldm",
              "fru_record_set_pdr.entity_type (2):" + std::to_string((unsigned) fru_record_set_pdr.entity_type));
    log_debug("pnc-pldm", "fru_record_set_pdr.entity_instance_number (2):" +
                          std::to_string((unsigned) fru_record_set_pdr.entity_instance_number));
    log_debug("pnc-pldm",
              "fru_record_set_pdr.container_id (2): " + std::to_string((unsigned) fru_record_set_pdr.container_id));
}
