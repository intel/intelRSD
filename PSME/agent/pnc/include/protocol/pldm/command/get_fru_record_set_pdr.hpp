/*!
* @brief GetFRURecordSetPDR PLDM command declaration
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
* @file get_fru_record_set_pdr.hpp
*/

#pragma once



#include "tools/fpga_oob/oob_fpga_toolset.hpp"
#include "protocol/pldm/pldm.hpp"
#include "gas/utils.hpp"
#include "get_pdr.hpp"

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! PLDM namespace */
namespace pldm {
/*! Command namespace */
namespace command {

/*! GetFRURecordSetPDR PLDM command class declaration */
class GetFruRecordSetPdr : public GetPdr {
public:
    /*! Ordinal number of FRU Record Set PDR in the FPGA PDR repository */
    static constexpr uint8_t PDR_RECORD_HANDLE = 71;


    /*! Default constructor */
    GetFruRecordSetPdr(agent_framework::model::enums::PlatformType platform,
                       pldm::PldmType type = pldm::PldmType::PldmForPlatformMonitoringAndControlSpecification) : GetPdr(
        platform, PDR_RECORD_HANDLE, type) {}


    /*! Copy constructor */
    GetFruRecordSetPdr(const GetFruRecordSetPdr&) = default;


    /*! Assignment operator */
    GetFruRecordSetPdr& operator=(const GetFruRecordSetPdr&) = default;


    /*! Default destructor */
    virtual ~GetFruRecordSetPdr();


    /*!
     * Returns name of the PLDM command
     * @return name of GetFRURecordSetPDR PLDM command
     */
    const char* get_command_name() const override {
        return "GetFRURecordSetPDR";
    }


private:
    /*!
     * @brief Logs result of this PLDM command
     * @param[in] port Twi port number (partition number for MF3)
     * @param[in] channel i2c expander channel
     */
    void log_command_result(const gas::PM85X6TwiPort port, const gas::PCA9548TwiExpanderChannel channel);


    /*!
     * @brief Processes response bytes for this PLDM command
     * @param[in] smbus_frame byte buffer which is SMBus frame received as a response to this PLDM command request
     * @param[in] pldm_tool PLDM tool object reference
     * @return PLDM completion code received in the response
     */
    virtual pldm::PldmCompletionCode handle_response(ByteBuffer& smbus_frame, PldmTool& pldm_tool) override;


#pragma pack(push, 1)
    /*! FRU Record Set PDR structure */
    struct {
        pldm::CommonPdrHeader common_pdr;
        uint16_t pldm_terminus_handle;
        uint16_t fru_record_set_indentifier;
        uint16_t entity_type;
        uint16_t entity_instance_number;
        uint16_t container_id;
    } fru_record_set_pdr{};
#pragma pack(pop)
};

}
}
}
}
