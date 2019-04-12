/*!
* @brief GetPLDMCommand PLDM command declaration
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
* @file get_pldm_commands.hpp
*/

#pragma once



#include "tools/fpga_oob/oob_fpga_toolset.hpp"
#include "gas/utils.hpp"
#include "protocol/pldm/pldm.hpp"
#include "get_pdr.hpp"

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! PLDM namespace */
namespace pldm {
/*! Command namespace */
namespace command {

/*! GetPLDMCommand PLDM command class declaration */
class GetPldmCommands : public PldmCommand {
public:
    /*! Default constructor */
    GetPldmCommands(agent_framework::model::enums::PlatformType platform, pldm::PldmType type) : PldmCommand(platform,
                                                                                                             type,
                                                                                                             pldm::PldmCommand::GetPldmCommands) {}


    /*! Copy constructor */
    GetPldmCommands(const GetPldmCommands&) = default;


    /*! Assignment operator */
    GetPldmCommands& operator=(const GetPldmCommands&) = default;


    /*! Default destructor */
    virtual ~GetPldmCommands();


    /*!
     * Returns name of the PLDM command
     * @return name of GetPLDMCommand PLDM command
     */
    const char* get_command_name() const override {
        return "GetPLDMCommands";
    }


    /*!
     * @brief Get PLDM version
     * @return PLDM version requested in this PLDM command
     */
    Ver32 get_pldm_version(void) const {
        return m_pldm_version;
    }


    /*!
     * @brief Set PLDM version
     * @param pldm_version PLDM version this command will use in request
     */
    void set_pldm_version(const Ver32 pldm_version) {
        m_pldm_version = pldm_version;
    }


private:
    /*!
     * @brief Logs result of this PLDM command
     * @param[in] port Twi port number (partition number for MF3)
     * @param[in] channel i2c expander channel
     */
    void log_command_result(const gas::PM85X6TwiPort port, const gas::PCA9548TwiExpanderChannel channel);


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


    uint8_t m_pldm_commands[32];
    pldm::Ver32 m_pldm_version{};
};

}
}
}
}
