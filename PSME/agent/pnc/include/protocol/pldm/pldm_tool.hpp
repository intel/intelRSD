/*!
* @brief PldmTool class used for common PLDM protocol processing operations.
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
* @file pldm_tool.hpp
*/

#pragma once



#include "protocol/pldm/pldm.hpp"
#include "protocol/smbus/smbus.hpp"
#include "protocol/mctp/mctp.hpp"



namespace agent {
namespace pnc {
namespace pldm {

using ByteBuffer = std::vector<std::uint8_t>;

/*! PldmTool class */
class PldmTool {
public:
    /*! Default constructor */
    PldmTool() {}


    /*! Enable copy */
    PldmTool(const PldmTool&) {}


    PldmTool& operator=(const PldmTool&) { return *this; }


    /*! Default destructor */
    virtual ~PldmTool();


    /*!
     * @brief Extracts fixed number of PLDM bytes from SMBus frame
     * @param[in] smbus_frame vector of bytes of received smbus frame
     * @param[out] out_data pointer to destination data buffer
     * @param[in] data_size number of bytes to extract to data buffer
     * @param[in] pldm_data_offset offset from the beginning of PLDM data in bytes
     * @return PLDM completion code received with smbus message
     */
    pldm::PldmCompletionCode
    extract_fixed_size_pldm_response(const ByteBuffer& smbus_frame, std::uint8_t* out_data, size_t data_size,
                                     size_t pldm_data_offset = 0);


    /*!
     * @brief Extract given number of bytes from SMBus frame starting from the beginning of the PLDM data
     * @param[in] smbus_frame vector of bytes of received smbus frame
     * @param[out] out_data pointer to destination data buffer
     * @param[in] data_size number of bytes to extract to data buffer
     * @param[in] pldm_data_offset offset from the beginning of PLDM data in bytes
     * @return PLDM completion code received with smbus message
     */
    pldm::PldmCompletionCode
    extract_number_of_bytes_from_pldm_response(const ByteBuffer& smbus_frame, std::uint8_t* out_data, size_t data_size,
                                               size_t pldm_data_offset = 0);


    /*!
     * @brief Extracts all PLDM bytes from SMBus frame
     * @param[in] smbus_frame vector of bytes of received smbus frame
     * @param[out] pldm_data destination data buffer
     * @return PLDM completion code received with smbus message
     */
    pldm::PldmCompletionCode extract_pldm_response(const ByteBuffer& smbus_frame, ByteBuffer& pldm_data);


    /*!
     * @brief Extracts PLDM completion code from SMBus frame
     * @param[in] smbus_frame vector of bytes of received smbus frame
     * @return PLDM completion code received with smbus message
     */
    pldm::PldmCompletionCode extract_pldm_completion_code(const ByteBuffer& smbus_frame);


    /*!
     * @brief Modifies smbus frame by correcting number of smbus frame bytes and puts CRC8 checksum at the end of it
     * @param[in,out] smbus_frame vector of bytes of smbus frame
     */
    void update_data_integrity(ByteBuffer& smbus_frame);


    /*!
     * @brief Returns common bytes for PLDM over MCTP over SMBus request
     * @param[in] type PLDM type of the requested command
     * @param[in] command PLDM command of the request
     * @return buffer with common request bytes
     */
    ByteBuffer get_common_request(pldm::PldmType type, pldm::PldmCommand command);


    /*!
     * @brief Inserts number of bytes from given address to destination buffer
     * @param[out] dest_buffer destination vector of bytes
     * @param[in] data pointer to data bytes to be packed to destination buffer
     * @param[in] data_size size of the data to insert to destination buffer
     */
    void pack_bytes(ByteBuffer& dest_buffer, const uint8_t* data, size_t data_size);


private:
    /*!
     * @brief Prepares new PLDM instance id for next PLDM request
     * @return PLDM instance id
     */
    std::uint8_t update_instance_id();


    /*!
     * @brief Corrects smbus bytes count in smbus frame to proper value
     * @param[in,out] smbus_frame vector of bytes of smbus frame
     */
    void update_smbus_bytes_count(ByteBuffer& smbus_frame);


    std::uint8_t m_pldm_rq_bit{0x80};
    std::uint8_t m_pldm_d_bit{0x00};
    std::uint8_t m_pldm_instance_id{0};
    std::uint8_t m_pldm_hdr_ver{0x00};

    static constexpr uint32_t MCTP_SMBUS_HEADER_SIZE =
        mctp::MCTP_OVER_SMBUS_HEADER_SIZE_BYTES + smbus::AARDVARK_SMBUS_BLOCK_WRITE_HEADER_SIZE_BYTES;

    static constexpr uint32_t PLDM_MCTP_SMBUS_HEADER_SIZE =
        pldm::PLDM_OVER_MCTP_HEADER_SIZE_BYTES + MCTP_SMBUS_HEADER_SIZE;
};

using PldmToolPtr = std::shared_ptr<PldmTool>;

}
}
}
