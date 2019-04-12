/*!
* @brief Out of band tool for FPGA implementation
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
* @file oob_fpga_tool.cpp
*/

#include "tools/fpga_oob/oob_fpga_tool.hpp"



using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::pnc::tools;
using namespace agent::pnc;
using namespace agent::pnc::pldm::command;
using namespace agent::pnc::i2c;
using namespace agent::pnc::gas;


OobFpgaTool::~OobFpgaTool() {}


bool OobFpgaTool::get_pldm_version(GetPldmVersion& pldm_version, const Port& port) const {
    try {
        pldm_version.execute(OobFpgaToolset::get(),
                             static_cast<PM85X6TwiPort>(port.get_twi_port()),
                             static_cast<PCA9548TwiExpanderChannel>(port.get_twi_channel()));
        return true;
    }
    catch (const std::exception& e) {
        log_error("oob-fpga-tool",
                  "Reading PLDM Version for fpga on port " << port.get_port_id() << " failed: " << e.what());
        log_error("oob-fpga-tool", "Cannot read PLDM Version for fpga.");
        return false;
    }
}


bool OobFpgaTool::get_pdr_repo_info(GetPdrRepositoryInfo& pdr_info, const Port& port) const {
    try {
        pdr_info.execute(OobFpgaToolset::get(),
                         static_cast<PM85X6TwiPort>(port.get_twi_port()),
                         static_cast<PCA9548TwiExpanderChannel>(port.get_twi_channel()));
        return true;
    }
    catch (const std::exception& e) {
        log_error("oob-fpga-tool",
                  "Reading PDR Repository Info for fpga on port " << port.get_port_id() << " failed: " << e.what());
        log_error("oob-fpga-tool", "Cannot read PDR Repository Info for fpga.");
        return false;
    }
}


bool OobFpgaTool::get_pdr(GetPdr& pdr, const Port& port) const {
    try {
        pdr.execute(OobFpgaToolset::get(),
                    static_cast<PM85X6TwiPort>(port.get_twi_port()),
                    static_cast<PCA9548TwiExpanderChannel>(port.get_twi_channel()));
        return true;
    }
    catch (const std::exception& e) {
        log_error("oob-fpga-tool", "Reading PDR for fpga on port " << port.get_port_id() << " failed: " << e.what());
        log_error("oob-fpga-tool", "Cannot read PDR for fpga.");
        return false;
    }
}


bool OobFpgaTool::get_terminus_uid(GetTerminusUid& terminus_uid, const Port& port) const {
    try {
        terminus_uid.execute(OobFpgaToolset::get(),
                             static_cast<PM85X6TwiPort>(port.get_twi_port()),
                             static_cast<PCA9548TwiExpanderChannel>(port.get_twi_channel()));
        return true;
    }
    catch (const std::exception& e) {
        log_error("oob-fpga-tool",
                  "Reading Terminus Uid for fpga on port " << port.get_port_id() << " failed: " << e.what());
        log_error("oob-fpga-tool", "Cannot read Terminus Uid for fpga.");
        return false;
    }
}


bool OobFpgaTool::get_pldm_commands(GetPldmCommands& pldm_commands, const Port& port) const {
    try {
        pldm_commands.execute(OobFpgaToolset::get(),
                              static_cast<PM85X6TwiPort>(port.get_twi_port()),
                              static_cast<PCA9548TwiExpanderChannel>(port.get_twi_channel()));
        return true;
    }
    catch (const std::exception& e) {
        log_error("oob-fpga-tool",
                  "Reading PLDM Commands for fpga on port " << port.get_port_id() << " failed: " << e.what());
        log_error("oob-fpga-tool", "Cannot read PLDM Commands for fpga.");
        return false;
    }
}