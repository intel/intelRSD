/*!
* @brief GetTerminusLocatorPdr PLDM command definition
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
* @file get_terminus_locator_pdr.cpp
*/

#include "utils/conversion.hpp"
#include "protocol/pldm/command/get_terminus_locator_pdr.hpp"



using namespace agent::pnc::gas;
using namespace agent::pnc::i2c;
using namespace agent::pnc::pldm::command;
using namespace agent::pnc::pldm;


GetTerminusLocatorPdr::~GetTerminusLocatorPdr() {}


PldmCompletionCode
GetTerminusLocatorPdr::handle_response(ByteBuffer& smbus_frame, PldmTool& pldm_tool) {
    GetPdr::handle_response(smbus_frame, pldm_tool);
    auto get_pdr_response_offset = get_pdr_response_header_size();

    pldm_tool.extract_number_of_bytes_from_pldm_response(smbus_frame, reinterpret_cast<uint8_t*>(&terminus_locator_pdr),
                                                         sizeof(terminus_locator_pdr), get_pdr_response_offset);
    pldm::PldmCompletionCode completion_code;
    auto terminus_locator_value_offset = get_pdr_response_offset + sizeof(terminus_locator_pdr);
    switch (terminus_locator_pdr.terminus_locator_type) {
        case TerminusLocatorType::UID : {
            completion_code = pldm_tool.extract_number_of_bytes_from_pldm_response(smbus_frame,
                                                                                   reinterpret_cast<uint8_t*>(&terminus_locator_value),
                                                                                   sizeof(terminus_locator_value.terminus_locator_value_uid),
                                                                                   terminus_locator_value_offset);
            uint8_t* buffer = reinterpret_cast<uint8_t*>(&terminus_locator_value.terminus_locator_value_uid.device_uid);
            m_device_uid = utils::buffer_to_string(buffer,
                                                   sizeof(terminus_locator_value.terminus_locator_value_uid.device_uid));
            break;
        }
        case TerminusLocatorType::MCTP_EID: {
            completion_code = pldm_tool.extract_number_of_bytes_from_pldm_response(smbus_frame,
                                                                                   reinterpret_cast<uint8_t*>(&terminus_locator_value),
                                                                                   sizeof(terminus_locator_value.terminus_locator_value_mctp_eid),
                                                                                   terminus_locator_value_offset);
            break;
        }
        case TerminusLocatorType::SMBUS_RELATIVE: {
            completion_code = pldm_tool.extract_number_of_bytes_from_pldm_response(smbus_frame,
                                                                                   reinterpret_cast<uint8_t*>(&terminus_locator_value),
                                                                                   sizeof(terminus_locator_value.terminus_locator_value_smbus_relative),
                                                                                   terminus_locator_value_offset);
            break;
        }
        case TerminusLocatorType::SYSTEM_SOFTWARE: {
            completion_code = pldm_tool.extract_number_of_bytes_from_pldm_response(smbus_frame,
                                                                                   reinterpret_cast<uint8_t*>(&terminus_locator_value),
                                                                                   sizeof(terminus_locator_value.terminus_locator_value_system_software),
                                                                                   terminus_locator_value_offset);
            break;
        }
        default: {
            completion_code = pldm_tool.extract_pldm_completion_code(smbus_frame);
            std::runtime_error(
                "Unknown GetPLDMTerminusLocatorPDR error: " + std::to_string((unsigned) completion_code));
        }
    }
    return completion_code;
}


void GetTerminusLocatorPdr::log_command_result(const PM85X6TwiPort port, const PCA9548TwiExpanderChannel channel) {
    PldmCommand::log_command_result(port, channel);
    log_debug("pnc-pldm", "Terminus Locator PDR:");
    log_debug("pnc-pldm", "terminus_locator_pdr.common_pdr_header.record_handle (4):" +
                          std::to_string(terminus_locator_pdr.common_pdr_header.record_handle));
    log_debug("pnc-pldm", "terminus_locator_pdr.common_pdr_header.pdr_header_version (1):" +
                          std::to_string((unsigned) terminus_locator_pdr.common_pdr_header.pdr_header_version));
    log_debug("pnc-pldm", "terminus_locator_pdr.common_pdr_header.pdr_type (1):" +
                          std::to_string((unsigned) terminus_locator_pdr.common_pdr_header.pdr_type));
    log_debug("pnc-pldm", "terminus_locator_pdr.common_pdr_header.record_change_number (2):" +
                          std::to_string((unsigned) terminus_locator_pdr.common_pdr_header.record_change_number));
    log_debug("pnc-pldm", "terminus_locator_pdr.common_pdr_header.data_length (2):" +
                          std::to_string((unsigned) terminus_locator_pdr.common_pdr_header.data_length));
    log_debug("pnc-pldm", "terminus_locator_pdr.pldm_terminus_handle (2):" +
                          std::to_string((unsigned) terminus_locator_pdr.pldm_terminus_handle));
    log_debug("pnc-pldm",
              "terminus_locator_pdr.validity (1):" + std::to_string((unsigned) terminus_locator_pdr.validity));
    log_debug("pnc-pldm", "terminus_locator_pdr.tid (1):" + std::to_string((unsigned) terminus_locator_pdr.tid));
    log_debug("pnc-pldm",
              "terminus_locator_pdr.container id (2):" + std::to_string((unsigned) terminus_locator_pdr.container_id));
    log_debug("pnc-pldm", "terminus_locator_pdr.terminus_locator_type (1): " +
                          std::to_string((unsigned) terminus_locator_pdr.terminus_locator_type));
    log_debug("pnc-pldm", "terminus_locator_pdr.terminus_locator_value_size (1): " +
                          std::to_string((unsigned) terminus_locator_pdr.terminus_locator_value_size));

    switch (terminus_locator_pdr.terminus_locator_type) {
        case TerminusLocatorType::UID : {
            log_debug("pnc-pldm", "terminus_locator_value.terminus_locator_value_uid.terminus_instance (1):" +
                                  std::to_string(
                                      (unsigned) terminus_locator_value.terminus_locator_value_uid.terminus_instance));
            log_debug("pnc-pldm", "terminus_locator_value.terminus_locator_value_uid.device_uid (16):" + m_device_uid);
            break;
        }
        case TerminusLocatorType::MCTP_EID: {
            log_debug("pnc-pldm", "terminus_locator_value.terminus_locator_value_mctp_eid.eid (1):" + std::to_string(
                (unsigned) terminus_locator_value.terminus_locator_value_mctp_eid.eid));
            break;
        }
        case TerminusLocatorType::SMBUS_RELATIVE: {
            uint8_t* buffer = reinterpret_cast<uint8_t*>(&terminus_locator_value.terminus_locator_value_smbus_relative.uid);
            auto uid = utils::buffer_to_string(buffer,
                                               sizeof(terminus_locator_value.terminus_locator_value_smbus_relative.uid));
            log_debug("pnc-pldm", "terminus_locator_value.terminus_locator_value_smbus_relative.uid (16):" + uid);
            log_debug("pnc-pldm", "terminus_locator_value.terminus_locator_value_smbus_relative.bus_number (1):" +
                                  std::to_string(
                                      (unsigned) terminus_locator_value.terminus_locator_value_smbus_relative.bus_number));
            log_debug("pnc-pldm", "terminus_locator_value.terminus_locator_value_smbus_relative.slave_address (1):" +
                                  std::to_string(
                                      (unsigned) terminus_locator_value.terminus_locator_value_smbus_relative.slave_address));
            break;
        }
        case TerminusLocatorType::SYSTEM_SOFTWARE: {
            log_debug("pnc-pldm", "terminus_locator_value.terminus_locator_value_system_software.software_class (1):" +
                                  std::to_string(
                                      (unsigned) terminus_locator_value.terminus_locator_value_system_software.software_class));
            uint8_t* buffer = reinterpret_cast<uint8_t*>(&terminus_locator_value.terminus_locator_value_system_software.uuid);
            auto uid = utils::buffer_to_string(buffer,
                                               sizeof(terminus_locator_value.terminus_locator_value_system_software.uuid));
            log_debug("pnc-pldm",
                      "terminus_locator_value.terminus_locator_value_system_software.uuid (16):" + uid);
            break;
        }
        default:
            log_debug("pnc-pldm", "Unknown value of the terminus locator type.");
    }
}
