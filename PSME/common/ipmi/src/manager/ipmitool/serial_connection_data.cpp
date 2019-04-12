/*!
 * @brief "serial_oem" ManagementController configuration data.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file ipmitool/serial_connection_data.cpp
 */

#include "ipmi/manager/ipmitool/serial_connection_data.hpp"

using namespace ipmi::manager::ipmitool;


SerialConnectionData::SerialConnectionData(const std::string& _device) :
    ipmi::ConnectionData(SerialConnectionData::INTF_TYPE), device(_device) {

    set_composite_device();
}


SerialConnectionData::SerialConnectionData() : ipmi::ConnectionData(SerialConnectionData::INTF_TYPE) {
    set_composite_device();
}


SerialConnectionData::~SerialConnectionData() { }


void SerialConnectionData::set_device(const std::string& _device) {
    device = _device;
    set_composite_device();
}


void SerialConnectionData::set_baud_rate(unsigned rate) {
    baud_rate = rate;
    set_composite_device();
}


void SerialConnectionData::set_data_bits(unsigned bits) {
    data_bits = bits;
    set_composite_device();
}


void SerialConnectionData::set_parity(const std::string& _parity) {
    parity = _parity;
    set_composite_device();
}


void SerialConnectionData::set_stop_bits(float bits) {
    stop_bits = bits;
    set_composite_device();
}


void SerialConnectionData::set_flow_control(const std::string& control) {
    flow_control = control;
    set_composite_device();
}


void SerialConnectionData::set_composite_device() {
    /* Device name is tokenized during each open() with strtok(). Currently it ALTERS passed
     * pointer!
     * TODO implement "const" composite device parsing in plugins/serial_oem with all properties.
     * This contains only the device name, to avoid overwritting internal string data.
     * @see additional comments in serial_ipmi_interface.cpp
     */
    composite = device;
}

std::string SerialConnectionData::get_info() const {
    return device;
}
