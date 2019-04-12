/*!
* @brief GetPDR PLDM command declaration
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
* @file get_pdr.hpp
*/

#pragma once



#include "tools/fpga_oob/oob_fpga_toolset.hpp"
#include "gas/utils.hpp"
#include "agent-framework/module/enum/chassis.hpp"
#include "protocol/pldm/pldm.hpp"
#include "pldm_command.hpp"


/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! PLDM namespace */
namespace pldm {
/*! Command namespace */
namespace command {

/*! GetPDR PLDM command class declaration */
class GetPdr : public PldmCommand {
public:
    /*! Default constructor */
    GetPdr(agent_framework::model::enums::PlatformType platform, pldm::RecordHandle pdr_record, pldm::PldmType type)
        : PldmCommand(
        platform, type, pldm::PldmCommand::GetPdr) {
        set_pdr_record_handle(pdr_record);
        /*! Default values for PDR request parameters */
        m_pdr_request_header.request_count = 255;
        m_pdr_request_header.record_change_number = 0;
        m_pdr_request_header.transfer_operational_flag = pldm::TransferOperationalFlag::GET_FIRST_PART;
        m_pdr_request_header.data_transfer_handle = 0;
    }


    /*! Copy constructor */
    GetPdr(const GetPdr&) = default;


    /*! Assignment operator */
    GetPdr& operator=(const GetPdr&) = default;


    /*! Default destructor */
    virtual ~GetPdr() {};


    /*!
     * @brief Set PDR record handle
     * @param[in] record_handle record handle for Get PDR request
     */
    void set_pdr_record_handle(uint32_t record_handle) {
        m_pdr_request_header.record_handle = record_handle;
    }


    /*!
     * @brief Get PDR record handle
     * @return PDR record handle for Get PDR request
     */
    uint32_t get_pdr_record_handle() const {
        return m_pdr_request_header.record_handle;
    }


    /*!
     * @brief Get PDR response header size
     * @return size of Get PDR response header
     */
    size_t get_pdr_response_header_size() const {
        return sizeof(m_pdr_response_header);
    }


    /*!
     * Returns name of the PLDM command
     * @return name of GetPDR PLDM command
     */
    const char* get_command_name() const override {
        return "GetPDR";
    }


protected:
    /*!
     * @brief Builds request for this PLDM command and returns byte buffer with request built
     * @param[in] pldm_tool PLDM tool object reference
     * @return vector of bytes containing request for this PLDM command
     */
    virtual ByteBuffer build_request(PldmTool& pldm_tool) override;


    /*!
     * @brief Processes response bytes for this PLDM command
     * @param[in] smbus_frame byte buffer which is SMBus frame received as a response to this PLDM command request
     * @param[in] pldm_tool PLDM tool object reference
     * @return PLDM completion code received in the response
     */
    virtual pldm::PldmCompletionCode handle_response(ByteBuffer& smbus_frame, PldmTool& pldm_tool) override;


    /*!
     * @brief Logs result of this PLDM command
     * @param[in] port Twi port number (partition number for MF3)
     * @param[in] channel i2c expander channel
     */
    void log_command_result(const gas::PM85X6TwiPort port, const gas::PCA9548TwiExpanderChannel channel);


private:
    pldm::CommonGetPdrRequestHeader m_pdr_request_header{};
    pldm::CommonGetPdrResponseHeader m_pdr_response_header{};
};

}
}
}
}
