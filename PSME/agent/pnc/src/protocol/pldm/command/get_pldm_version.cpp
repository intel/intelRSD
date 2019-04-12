/*!
* @brief GetPldmVersion PLDM command definition
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
* @file get_pldm_version.cpp
*/

#include "protocol/pldm/pldm_error.hpp"
#include "protocol/pldm/command/get_pldm_version.hpp"



using namespace agent::pnc::gas;
using namespace agent::pnc::i2c;
using namespace agent::pnc::pldm::command;
using namespace agent::pnc::pldm;


GetPldmVersion::~GetPldmVersion() {}


ByteBuffer GetPldmVersion::build_request(PldmTool& pldm_tool) {

    auto req_bytes = pldm_tool.get_common_request(PldmType::PldmMessagingControlAndDiscovery,
                                                  pldm::PldmCommand::GetPldmVersion);

    uint32_t data_transfer_handle(DEFAULT_DATA_TRANSFER_HANDLE);
    pldm_tool.pack_bytes(req_bytes, reinterpret_cast<uint8_t*>(&data_transfer_handle), sizeof(data_transfer_handle));

    req_bytes.push_back(TransferOperationalFlag::GET_FIRST_PART);
    req_bytes.push_back(m_pldm_type);

    pldm_tool.update_data_integrity(req_bytes);
    return req_bytes;
}


PldmCompletionCode
GetPldmVersion::handle_response(ByteBuffer& smbus_frame, PldmTool& pldm_tool) {
    ByteBuffer pldm_data;
    auto completion_code = pldm_tool.extract_pldm_response(smbus_frame, pldm_data);
    if (completion_code == pldm::PldmCompletionBaseCode::SUCCESS) {
        const auto ver_size = sizeof(Ver32);
        const auto data_offset = sizeof(pldm::DataTransferHandle) + sizeof(pldm::TransferFlag);
        size_t pldm_version_data_size = pldm_data.size() - data_offset - sizeof(PldmVersionDataIntegrityChecksum);
        if (pldm_version_data_size % ver_size) {
            throw pldm::InvalidPldmMessageSize("Extract pldm version response procedure failed.");
        }

        size_t versions_count = pldm_version_data_size / ver_size;
        for (size_t i = 0; i < versions_count; ++i) {
            Ver32* version = reinterpret_cast<Ver32*>(&pldm_data[data_offset + i * ver_size]);
            m_pldm_versions.push_back(*version);
        }
    }
    return completion_code;
}


void GetPldmVersion::log_command_result(const PM85X6TwiPort port, const PCA9548TwiExpanderChannel channel) {
    PldmCommand::log_command_result(port, channel);
    log_debug("pnc-pldm", "PLDM versions supported:");
    for (unsigned i = 0; i < m_pldm_versions.size(); ++i) {
        pldm::Ver32 ver = m_pldm_versions[i];

        log_debug("pnc-pldm", "PLDM VERSION no. " + std::to_string(i));
        log_debug("pnc-pldm", "major_version_number: " + std::to_string((unsigned) ver.major_version_number));
        log_debug("pnc-pldm", "minor_version_number: " + std::to_string((unsigned) ver.minor_version_number));
        log_debug("pnc-pldm", "update_version_number: " + std::to_string((unsigned) ver.update_version_number));
        log_debug("pnc-pldm", "alpha: " + std::to_string((unsigned) ver.alpha));
    }
}
