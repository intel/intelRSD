/*!
* @brief PLDM command interface declaration
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
* @file pldm_command.hpp
*/

#pragma once



#include "tools/fpga_oob/oob_fpga_toolset.hpp"
#include "gas/utils.hpp"
#include "agent-framework/module/enum/chassis.hpp"
#include "protocol/pldm/pldm.hpp"

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! PLDM namespace */
namespace pldm {
/*! Command namespace */
namespace command {

using ByteBuffer = std::vector<uint8_t>;

/*! PLDM command interface class declaration */
class PldmCommand {
public:
    /*! Default constructor */
    PldmCommand(agent_framework::model::enums::PlatformType platform, pldm::PldmType type, pldm::PldmCommand command)
        : m_platform(platform), m_pldm_type(type), m_pldm_command(command) {}


    /*! Default destructor */
    virtual ~PldmCommand();


    /*!
     * @brief Executes PLDM command by sending request and receiving response
     * @param[in] oob_toolset FPGA out of band toolset object
     * @param[in] port TWI Port number
     * @param[in] channel TWI Expander channel number
     */
    void execute(const tools::OobFpgaToolset& oob_toolset, gas::PM85X6TwiPort port,
                 gas::PCA9548TwiExpanderChannel channel);


    /*!
     * @brief Set PLDM type
     * @param[in] type PLDM type for the command
     */
    void set_pldm_type(const pldm::PldmType type) {
        m_pldm_type = type;
    }


    /*!
     * @brief Get PLDM type
     * @return PLDM type of this command
     */
    pldm::PldmType get_pldm_type() const {
        return m_pldm_type;
    }


    /*!
     * @brief Set PLDM command
     * @param[in] command PLDM command type for this command object
     */
    void set_pldm_command(const pldm::PldmCommand command) {
        m_pldm_command = command;
    }


    /*!
     * @brief Get PLDM command
     * @return PLDM command type for this command object
     */
    pldm::PldmCommand get_pldm_command() const {
        return m_pldm_command;
    }


protected:
    /*!
     * @brief Logs result of this PLDM command
     * @param[in] port Twi port number (partition number for MF3)
     * @param[in] channel i2c expander channel
     */
    virtual void log_command_result(const gas::PM85X6TwiPort port, const gas::PCA9548TwiExpanderChannel channel);


    agent_framework::model::enums::PlatformType m_platform{agent_framework::model::enums::PlatformType::EDK};
    pldm::PldmType m_pldm_type{};
    pldm::PldmCommand m_pldm_command{};

private:
    /*!
     * @brief Selects channel of i2c expander
     * @param[in] i2c pointer to i2c access interface
     * @param[in] port Twi port number (partition number for MF3)
     * @param[in] channel i2c expander channel
     */
    void select_channel(const i2c::I2cAccessInterfacePtr i2c, gas::PM85X6TwiPort port,
                        gas::PCA9548TwiExpanderChannel channel);


    /*!
     * @brief Based on this PLDM command this method sends request and receives response
     * @param[in] oob_toolset FPGA out of band toolset object
     * @param[in] port Twi port number (partition number for MF3)
     */
    void send_receive(const tools::OobFpgaToolset& oob_toolset, gas::PM85X6TwiPort port);


    /*!
     * @brief Builds request for this PLDM command and returns byte buffer with request built
     * @param[in] pldm_tool PLDM tool object reference
     * @return vector of bytes containing request for this PLDM command
     */
    virtual ByteBuffer build_request(PldmTool& pldm_tool) = 0;


    /*!
     * @brief Processes response bytes for this PLDM command
     * @param[in] smbus_frame byte buffer which is SMBus frame received as a response to this PLDM command request
     * @param[in] pldm_tool PLDM tool object reference
     * @return PLDM completion code received in the response
     */
    virtual pldm::PldmCompletionCode handle_response(ByteBuffer& smbus_frame, PldmTool& pldm_tool) = 0;


    /*!
     * Returns name of the PLDM command
     * @return name of this PLDM command
     */
    virtual const char* get_command_name() const = 0;


    /*! Copy constructor */
    PldmCommand(const PldmCommand&) = default;


    /*! Assignment operator */
    PldmCommand& operator=(const PldmCommand&) = default;
};

}
}
}
}
