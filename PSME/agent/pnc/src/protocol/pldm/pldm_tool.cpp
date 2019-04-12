/*!
* @brief PldmTool implementation
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
* @file pldm_tool.cpp
*/

#include "protocol/pldm/pldm_tool.hpp"
#include "protocol/pldm/pldm_error.hpp"
#include "protocol/smbus/smbus_error.hpp"
#include "protocol/pldm/utils/pldm_utils.hpp"

#include <bitset>



using namespace agent::pnc::pldm;


PldmTool::~PldmTool() {}


void PldmTool::pack_bytes(ByteBuffer& dest_buffer, const uint8_t* data, size_t data_size) {
    for (size_t i = 0; i < data_size; ++i) {
        dest_buffer.push_back(data[i]);
    }
}


ByteBuffer PldmTool::get_common_request(PldmType type, PldmCommand command) {
    ByteBuffer buffer;

    /*! SMBus header*/
    buffer.push_back(smbus::CommandCode::MCTP);
    buffer.push_back(0x00);                                         // SMBus Byte Count (0x00 temporary value)
    buffer.push_back(smbus::WellKnownSlaveAddresses::IPMI_BMC | mctp::MCTP_OVER_SMBUS_BINDING_BYTE);

    /*! MCTP header*/
    buffer.push_back(mctp::header::RSVD | mctp::header::HEADER_VERSION);
    buffer.push_back(mctp::header::DESTINATION_ENDPOINT_ID);
    buffer.push_back(mctp::header::SOURCE_ENDPOINT_ID);
    buffer.push_back(mctp::header::SOM_BIT | mctp::header::EOM_BIT | mctp::header::PKT_SEQ | mctp::header::TO_BIT |
                     mctp::header::MSG_TAG);
    buffer.push_back(pldm::PLDM_OVER_MCTP_BINDING_BYTE);

    /*! PLDM header*/
    buffer.push_back(m_pldm_rq_bit | m_pldm_d_bit | update_instance_id());
    buffer.push_back(m_pldm_hdr_ver | type);
    buffer.push_back(command);
    return buffer;
}


PldmCompletionCode
PldmTool::extract_fixed_size_pldm_response(const ByteBuffer& smbus_frame, std::uint8_t* out_data, size_t data_size,
                                           size_t pldm_data_offset) {
    auto real_data_offset = sizeof(pldm::PldmCompletionCode) + pldm_data_offset;
    if (smbus_frame.size() != PLDM_MCTP_SMBUS_HEADER_SIZE + real_data_offset + data_size + smbus::PEC_SIZE_BYTES) {
        auto real_data_size =
            smbus_frame.size() - (PLDM_MCTP_SMBUS_HEADER_SIZE + real_data_offset + smbus::PEC_SIZE_BYTES);
        std::string error_message("Pldm reponse extract procedure failed.");
        if (smbus_frame.size() > PLDM_MCTP_SMBUS_HEADER_SIZE) {
            error_message.append(
                " PLDM completion code: " + std::to_string((unsigned) smbus_frame.at(PLDM_MCTP_SMBUS_HEADER_SIZE)));
        }
        throw InvalidPldmMessageSize(error_message, data_size, real_data_size);
    }
    real_data_offset += PLDM_MCTP_SMBUS_HEADER_SIZE;
    for (unsigned i = 0; i < data_size; ++i) {
        out_data[i] = smbus_frame.at(i + real_data_offset);
    }
    return smbus_frame.at(PLDM_MCTP_SMBUS_HEADER_SIZE);
}


PldmCompletionCode
PldmTool::extract_number_of_bytes_from_pldm_response(const ByteBuffer& smbus_frame, std::uint8_t* out_data,
                                                     size_t data_size, size_t pldm_data_offset) {
    auto real_data_offset = sizeof(pldm::PldmCompletionCode) + pldm_data_offset;
    if (smbus_frame.size() < PLDM_MCTP_SMBUS_HEADER_SIZE + real_data_offset + data_size + smbus::PEC_SIZE_BYTES) {
        auto real_data_size =
            smbus_frame.size() - (PLDM_MCTP_SMBUS_HEADER_SIZE + real_data_offset + smbus::PEC_SIZE_BYTES);
        std::string error_message("Pldm reponse extract bytes procedure failed.");
        if (smbus_frame.size() > PLDM_MCTP_SMBUS_HEADER_SIZE) {
            error_message.append(
                " PLDM completion code: " + std::to_string((unsigned) smbus_frame.at(PLDM_MCTP_SMBUS_HEADER_SIZE)));
        }
        throw InvalidPldmMessageSize(error_message, data_size, real_data_size);
    }
    real_data_offset += PLDM_MCTP_SMBUS_HEADER_SIZE;
    for (unsigned i = 0; i < data_size; ++i) {
        out_data[i] = smbus_frame.at(i + real_data_offset);
    }
    return smbus_frame.at(PLDM_MCTP_SMBUS_HEADER_SIZE);
}


PldmCompletionCode PldmTool::extract_pldm_response(const ByteBuffer& smbus_frame, ByteBuffer& pldm_data) {
    if (smbus_frame.size() <= PLDM_MCTP_SMBUS_HEADER_SIZE) {
        throw smbus::InvalidSmbusFrameSize("Pldm reponse extract bytes procedure failed. PLDM message incomplete.");
    }
    PldmCompletionCode completion_code = smbus_frame.at(PLDM_MCTP_SMBUS_HEADER_SIZE);
    if (completion_code == pldm::SUCCESS) {
        auto data_offset = PLDM_MCTP_SMBUS_HEADER_SIZE + sizeof(pldm::PldmCompletionCode);
        std::copy(begin(smbus_frame) + data_offset, end(smbus_frame) - smbus::PEC_SIZE_BYTES,
                  std::back_inserter(pldm_data));
    }
    return completion_code;
}


PldmCompletionCode
PldmTool::extract_pldm_completion_code(const ByteBuffer& smbus_frame) {
    if (smbus_frame.size() > PLDM_MCTP_SMBUS_HEADER_SIZE) {
        return smbus_frame.at(PLDM_MCTP_SMBUS_HEADER_SIZE);
    }
    else {
        throw smbus::InvalidSmbusFrameSize(
            "Pldm reponse extract completion code procedure failed. PLDM message incomplete.");
    }
}


std::uint8_t PldmTool::update_instance_id() {
    m_pldm_instance_id = static_cast<uint8_t>(m_pldm_instance_id + 1) % 32;       // instance id has a size of 5 bits
    return m_pldm_instance_id;
}


void PldmTool::update_smbus_bytes_count(ByteBuffer& smbus_frame) {
    int64_t smbus_bytes_count = smbus_frame.size() - smbus::AARDVARK_SMBUS_BLOCK_WRITE_HEADER_SIZE_BYTES;
    if (smbus_bytes_count > std::numeric_limits<uint8_t>::max()) {
        throw smbus::InvalidSmbusFrameSize(std::string("PldmTool error: smbus frame is too long"));
    }
    else if (smbus_bytes_count < 0) {
        throw smbus::InvalidSmbusFrameSize(std::string("PldmTool error: incomplete smbus frame \n"));
    }
    smbus_frame.at(1) = static_cast<uint8_t>(smbus_bytes_count);
}


void PldmTool::update_data_integrity(ByteBuffer& smbus_frame) {
    update_smbus_bytes_count(smbus_frame);
    smbus_frame.push_back(utils::crc8(smbus_frame));
}
