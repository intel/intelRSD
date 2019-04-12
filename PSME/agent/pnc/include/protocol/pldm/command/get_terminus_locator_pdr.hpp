/*!
* @brief GetTerminusLocatorPDR PLDM command declaration
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
* @file get_terminus_locator_pdr.hpp
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

enum Validity : uint8_t {
    NOT_VALID,
    VALID
};

enum TerminusLocatorType : uint8_t {
    UID = 0x00,
    MCTP_EID,
    SMBUS_RELATIVE,
    SYSTEM_SOFTWARE
};

enum SoftwareClass : uint8_t {
    UNSPECIFIED,
    OTHER,
    SYSTEM_FIRMWARE,
    OS_LOADER,
    OS,
    CIM_PROVIDER,
    OTHER_PROVIDER,
    VIRTUAL_MACHINE_MANAGER
};

/*! GetTerminusLocatorPDR PLDM command class declaration */
class GetTerminusLocatorPdr : public GetPdr {
public:
    /*! Size of the serial_number field of the output (in bytes) */
    static constexpr uint8_t PDR_RECORD_HANDLE = 70;


    /*! Default constructor */
    GetTerminusLocatorPdr(agent_framework::model::enums::PlatformType platform,
                          pldm::PldmType type = pldm::PldmType::PldmForPlatformMonitoringAndControlSpecification)
        : GetPdr(platform, PDR_RECORD_HANDLE, type) {}


    /*! Copy constructor */
    GetTerminusLocatorPdr(const GetTerminusLocatorPdr&) = default;


    /*! Assignment operator */
    GetTerminusLocatorPdr& operator=(const GetTerminusLocatorPdr&) = default;


    /*! Default destructor */
    virtual ~GetTerminusLocatorPdr();


    /*!
     * @brief Get device uid
     * @return uid of the device
     */
    std::string get_device_uid() const {
        return m_device_uid;
    }


    /*!
     * Returns name of the PLDM command
     * @return name of GetTerminusLocatorPDR PLDM command
     */
    const char* get_command_name() const override {
        return "GetTerminusLocatorPDR";
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
    virtual pldm::PldmCompletionCode handle_response(ByteBuffer& data, PldmTool& pldm_tool) override;


#pragma pack(push, 1)

    union {
        struct {
            uint8_t terminus_instance;
            PldmUuid device_uid;
        } terminus_locator_value_uid;

        struct {
            uint8_t eid;
        } terminus_locator_value_mctp_eid;

        struct {
            PldmUuid uid;
            uint8_t bus_number;
            uint8_t slave_address;
        } terminus_locator_value_smbus_relative;

        struct {
            SoftwareClass software_class;
            PldmUuid uuid;
        } terminus_locator_value_system_software;
    } terminus_locator_value{};

    /*! Terminus Locator PDR format */
    struct TerminusLocatorPdr {
        pldm::CommonPdrHeader common_pdr_header;
        uint16_t pldm_terminus_handle;
        Validity validity;
        pldm::PldmTerminusId tid;
        pldm::ContainerId container_id;
        TerminusLocatorType terminus_locator_type;
        uint8_t terminus_locator_value_size;
    } terminus_locator_pdr{};
#pragma pack(pop)

    std::string m_device_uid{};
};

}
}
}
}
