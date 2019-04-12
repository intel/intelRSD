/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file i2c_tool.cpp
 *
 * @brief I2cTool implementation
 * */

#include "tools/i2c_tool.hpp"
#include "i2c/i2c_access_interface_factory.hpp"
#include "gas/utils.hpp"



using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::pnc::tools;
using namespace agent::pnc;
using namespace agent::pnc::nvme;
using namespace agent::pnc::i2c;
using namespace agent::pnc::gas;


I2cTool::~I2cTool() {}


bool I2cTool::get_seeprom(Seeprom& seeprom) const {
    try {
        seeprom.read(I2cAccessInterfaceFactory::get_instance().get_interface());
        return true;
    }
    catch (const std::exception& e) {
        log_debug("i2c-tool", "Cannot read Seeprom data for PCIe Switch: " << e.what());
        log_warning("i2c-tool", "Reading PCIe Switch Seeprom failed");
        return false;
    }
}


bool I2cTool::get_cable_id(CableId& cable_id, const Port& port) const {
    try {
        cable_id.read(I2cAccessInterfaceFactory::get_instance().get_interface(),
                      PM85X6TwiPort(port.get_twi_port()),
                      PCA9548TwiExpanderChannel(port.get_twi_channel()));
        return true;
    }
    catch (const std::exception& e) {
        log_debug("i2c-tool", "Reading CableId on port " << port.get_port_id() << " failed: " << e.what());
        log_warning("i2c-tool", "Cannot read CableId on port");
        return false;
    }
}


bool I2cTool::get_vpd(VitalProductData& vpd, const Port& port) const {
    try {

        vpd.read(I2cAccessInterfaceFactory::get_instance().get_interface(),
                 static_cast<PM85X6TwiPort>(port.get_twi_port()),
                 static_cast<PCA9548TwiExpanderChannel>(port.get_twi_channel()));
        return true;
    }
    catch (const std::exception& e) {
        log_debug("i2c-tool", "Reading VPD for drive on port " << port.get_port_id() << " failed: " << e.what());
        log_warning("i2c-tool", "Cannot read VPD for drive.");
        return false;
    }
}


bool I2cTool::get_smart(Smart& smart, const Port& port) const {
    try {
        smart.read(I2cAccessInterfaceFactory::get_instance().get_interface(),
                   static_cast<PM85X6TwiPort>(port.get_twi_port()),
                   static_cast<PCA9548TwiExpanderChannel>(port.get_twi_channel()));
        return true;
    }
    catch (const std::exception& e) {
        log_debug("i2c-tool", "Reading Smart for drive on port " << port.get_port_id() <<
                                                                 " failed: " << e.what());
        log_warning("i2c-tool", "Cannot read Smart for drive.");
        return false;
    }
}


bool I2cTool::get_firmware_version(FirmwareVersion& fw, const Port& port) const {
    try {
        fw.read(I2cAccessInterfaceFactory::get_instance().get_interface(),
                static_cast<PM85X6TwiPort>(port.get_twi_port()),
                static_cast<PCA9548TwiExpanderChannel>(port.get_twi_channel()));
        return true;
    }
    catch (const std::exception& e) {
        log_debug("i2c-tool", "Reading Firmware Version for drive on port " << port.get_port_id() <<
                                                                            " failed: " << e.what());
        log_warning("i2c-tool", "Cannot read Firmware Version for drive.");
        return false;
    }
}


bool I2cTool::get_fru_eeprom(FruEeprom& fru_eeprom) const {
    try {
        fru_eeprom.read(I2cAccessInterfaceFactory::get_instance().get_interface());
        return true;
    }
    catch (const std::exception& e) {
        log_warning("i2c-tool", "Cannot read FRU EEPROM data: " << e.what());
        return false;
    }
}
