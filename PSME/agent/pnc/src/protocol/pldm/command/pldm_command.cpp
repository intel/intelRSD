/*!
* @brief PldmCommand generic PLDM command definition
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
* @file pldm_command.cpp
*/

#include "protocol/pldm/command/pldm_command.hpp"
#include "protocol/pldm/pldm_error.hpp"



using namespace agent::pnc::gas;
using namespace agent::pnc::i2c;
using namespace agent::pnc::pldm::command;


PldmCommand::~PldmCommand() {}


void PldmCommand::execute(const tools::OobFpgaToolset& oob_toolset, PM85X6TwiPort port,
                          PCA9548TwiExpanderChannel channel) {
    select_channel(oob_toolset.m_i2c_interface, port, channel);
    send_receive(oob_toolset, port);
    log_command_result(port, channel);
}


void PldmCommand::select_channel(const i2c::I2cAccessInterfacePtr i2c, PM85X6TwiPort port,
                                 PCA9548TwiExpanderChannel channel) {
    PM85X6TwiDeviceAddress address{};
    if (agent_framework::model::enums::PlatformType::EDK == m_platform) {
        address = PM85X6TwiDeviceAddress::EDK_TWI_EXPANDER;
    }
    else if (agent_framework::model::enums::PlatformType::MF3 == m_platform) {
        address = PM85X6TwiDeviceAddress::MF3_TWI_EXPANDER;
    }
    uint8_t data = uint8_t(channel);
    i2c->write(static_cast<uint8_t>(port),
               static_cast<uint16_t>(address),
               0,
               reinterpret_cast<uint8_t*>(&data),
               sizeof(data));
}


void PldmCommand::send_receive(const tools::OobFpgaToolset& oob_toolset, PM85X6TwiPort port) {

    PM85X6TwiDeviceAddress address = PM85X6TwiDeviceAddress::FPGA_RUSHCREEK_SLAVE_ADDRESS;

    auto req_bytes = this->build_request(*oob_toolset.m_pldm_tool);

    oob_toolset.m_i2c_interface->write(static_cast<uint8_t>(port),
                                       static_cast<uint16_t>(address),
                                       0,
                                       reinterpret_cast<uint8_t*>(req_bytes.data()),
                                       static_cast<std::uint16_t>(req_bytes.size()));

    auto resp_bytes = oob_toolset.m_aardvark_oob_tool->slave_receive();

    auto completion_code = handle_response(resp_bytes, *oob_toolset.m_pldm_tool);

    if (completion_code != pldm::PldmCompletionBaseCode::SUCCESS) {
        throw CommandError(*this, std::string("PLDM completion code: " + std::to_string((unsigned) completion_code)),
                           completion_code);
    }
}


void PldmCommand::log_command_result(const PM85X6TwiPort port, const PCA9548TwiExpanderChannel channel) {
    log_debug("pnc-pldm", "PLDM command " << this->get_command_name() << " for FPGA connected to TWI Port: "
                                          << std::uint32_t(port) << " TWI Channel: " << std::uint32_t(channel)
                                          << " succesfully executed.");
}