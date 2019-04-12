/*!
* @brief Example of PLDM commands usage.
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
* @file main.cpp
*/

#include "protocol/pldm/command/get_pldm_commands.hpp"
#include "protocol/pldm/command/get_pldm_version.hpp"
#include "protocol/pldm/command/get_terminus_uid.hpp"
#include "protocol/pldm/command/get_terminus_locator_pdr.hpp"
#include "tools/fpga_oob/oob_fpga_tool.hpp"



using namespace agent_framework::model::enums;
using namespace agent_framework::model;
using namespace std::chrono_literals;
using namespace agent::pnc::pldm;


int main(int argc, const char* argv[]) {

    (void) argc;
    (void) argv;

    if (argc != 4) {
        log_error(argv[0],
                  "Too few arguments. Please provide 3 arguments:\n"
                  "\t<1> - IP address of BMC of the MF3 switch\n"
                  "\t<2> - username to login to BMC of MF3 switch\n"
                  "\t<3> - password to login to BMC of MF3 switch\n");
        return -1;
    }

    try {
        std::string ip_address = std::string(argv[1]);
        std::string user_name = std::string(argv[2]);
        std::string passwd = std::string(argv[3]);

        agent::pnc::pldm::command::GetPldmVersion pldm_version(PlatformType::MF3);
        agent::pnc::pldm::command::GetPldmVersion pldm_version_2(PlatformType::MF3);
        agent::pnc::pldm::command::GetPdrRepositoryInfo pdr_info(PlatformType::MF3);
        agent::pnc::pldm::command::GetFruRecordSetPdr pdr(PlatformType::MF3);
        agent::pnc::pldm::command::GetPldmCommands pldm_commands(PlatformType::MF3,
                                                                 agent::pnc::pldm::PldmType::PldmMessagingControlAndDiscovery);
        agent::pnc::pldm::command::GetTerminusUid terminus_uid(PlatformType::MF3);
        agent::pnc::pldm::command::GetTerminusLocatorPdr locator_pdr(PlatformType::MF3);

        agent::pnc::aardvark::AardvarkOobTool::get_default_instance()->init_single_device();
        agent::pnc::i2c::I2cAccessInterfaceFactory::get_instance().init_ipmi_interface(ip_address, 623, user_name, passwd);
        agent::pnc::i2c::I2cAccessInterfaceFactory::get_instance().switch_to_ipmi();
        agent::pnc::tools::OobFpgaToolPtr oob_fpga_tool = std::make_shared<agent::pnc::tools::OobFpgaTool>();
        Port port{};
        port.set_twi_port(4);           // partition 4
        port.set_twi_channel(128);      // channel 128 (most right PCIe slot in MF3)


        log_info("pnc-pldm", "/*******__get_pldm_version__0x00 ********/");
        pldm_version.set_pldm_type(agent::pnc::pldm::PldmType::PldmMessagingControlAndDiscovery);
        oob_fpga_tool->get_pldm_version(pldm_version, port);

        for (unsigned i = 0; i < pldm_version.get_pldm_versions().size(); ++i) {
            log_info("pnc-pldm", "/*******__get_pldm_commands__0x00 ********/");
            pldm_commands.set_pldm_type(agent::pnc::pldm::PldmType::PldmMessagingControlAndDiscovery);
            pldm_commands.set_pldm_version(pldm_version.get_pldm_versions().at(i));
            oob_fpga_tool->get_pldm_commands(pldm_commands, port);
        }

        log_info("pnc-pldm", "/*******__get_pldm_version__0x02 ********/");
        pldm_version_2.set_pldm_type(PldmType::PldmForPlatformMonitoringAndControlSpecification);
        oob_fpga_tool->get_pldm_version(pldm_version_2, port);

        for (unsigned i = 0; i < pldm_version_2.get_pldm_versions().size(); ++i) {
            log_info("pnc-pldm", "/*******__get_pldm_commands__0x02 ********/");
            pldm_commands.set_pldm_type(agent::pnc::pldm::PldmType::PldmForPlatformMonitoringAndControlSpecification);
            pldm_commands.set_pldm_version(pldm_version_2.get_pldm_versions().at(i));
            oob_fpga_tool->get_pldm_commands(pldm_commands, port);
        }

        log_info("pnc-pldm", "/*******__get_pdr_repository_info__********/");
        oob_fpga_tool->get_pdr_repo_info(pdr_info, port);

        log_info("pnc-pldm", "/*******__get_pdr no. 71  __********/");
        oob_fpga_tool->get_pdr(pdr, port);

        log_info("pnc-pldm", "/*******__get_pdr no. 70  __********/");
        oob_fpga_tool->get_pdr(locator_pdr, port);

        log_info("pnc-pldm", "All PLDM requests succesfully completed.");
    }
    catch(std::exception& e) {
        log_error(argv[0], "Error occured, exiting.\n" + std::string(e.what()));
        return -2;
    }
    return 0;
}
