/*!
* @brief GetTerminusUid PLDM command declaration
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
* @file get_terminus_uid.hpp
*/

#pragma once



#include "tools/fpga_oob/oob_fpga_toolset.hpp"
#include "agent-framework/module/utils/uuid.hpp"
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

/*! GetTerminusUid PLDM command class declaration */
class GetTerminusUid : public PldmCommand {
public:
    /*! Default constructor */
    GetTerminusUid(agent_framework::model::enums::PlatformType platform,
                   pldm::PldmType type = PldmType::PldmForPlatformMonitoringAndControlSpecification) : PldmCommand(
        platform, type, pldm::PldmCommand::GetTerminusUid) {}


    /*! Copy constructor */
    GetTerminusUid(const GetTerminusUid&) = default;


    /*! Assignment operator */
    GetTerminusUid& operator=(const GetTerminusUid&) = default;


    /*!
     * Returns name of the PLDM command
     * @return name of GetTerminusUid PLDM command
     */
    const char* get_command_name() const override {
        return "GetTerminusUid";
    }


    /*!
     * @brief Get uuid of the device
     * @return device uuid
     */
    Uuid get_uuid() const {
        return m_uuid;
    }


    /*! Default destructor */
    virtual ~GetTerminusUid();


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
    virtual pldm::PldmCompletionCode handle_response(ByteBuffer& data, PldmTool& pldm_tool) override;


    Uuid m_uuid{};
};

}
}
}
}
