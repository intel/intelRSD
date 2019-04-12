/*!
 * @brief Session implementation for "serial_oem" communication interface.
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
 * @file ipmitool/serial_ipmi_intf.cpp
 */

#include "ipmi/manager/ipmitool/serial_ipmi_interface.hpp"
#include "ipmi/manager/ipmitool/serial_connection_data.hpp"
#include "ipmi/manager/ipmitool/ipmi_intf_impl.hpp"

#include <stdexcept>

using namespace ipmi::manager::ipmitool;

namespace {

void initialize_intf(ipmi_intf* intf, ipmi::ConnectionData::ConstPtr connection_data) {
    const SerialConnectionData& data = dynamic_cast<const SerialConnectionData&>(*connection_data);

    /*
     * ipmitool implementation always uses 8N1 without flow controll.
     * Baud rates: 2400, 9600, 19200, 38400, 57600, 115200 and 230400 are allowed, but
     * need to be appended to the device name (device:rate).
     * Current ipmitool implementation modifies this string, so second opne() doesn't have
     * baud rate given.
     * TODO ipmi_intf struct has context field where all settings should be stored to be reused.
     * appropriate implementation must be done in ipmitool library.
     */
    if (data.get_baud_rate() != 115200) {
        throw std::runtime_error("Not handled speed " + std::to_string(data.get_baud_rate()));
    }
    if (data.get_data_bits() != 8) {
        throw std::runtime_error("Not handled number of bits " + std::to_string(data.get_data_bits()));
    }
    if (data.get_parity() != "none") {
        throw std::runtime_error("Not handled parity " + data.get_parity());
    }
    if (data.get_stop_bits() != 1) {
        throw std::runtime_error("Not handled stop bits " + std::to_string(data.get_stop_bits()));
    }
    if (data.get_flow_control() != "none") {
        throw std::runtime_error("Not handled flow control " + data.get_flow_control());
    }

    intf->devfile = const_cast<char*>(data.get_device().c_str());
}

}

SerialIpmiInterface::SerialIpmiInterface(ipmi::ConnectionData::ConstPtr connection_data) :
    data(dynamic_cast<const SerialConnectionData&>(*connection_data)),
    intf(new IpmiIntfImpl(connection_data, initialize_intf)) { }

SerialIpmiInterface::~SerialIpmiInterface() { }

bool SerialIpmiInterface::matches(ipmi::ConnectionData::ConstPtr connection_data) const {

    if (connection_data->get_interface_type() != SerialConnectionData::INTF_TYPE) {
        return false;
    }
    const SerialConnectionData& other = dynamic_cast<const SerialConnectionData&>(*connection_data);
    return (data.get_device() == other.get_device());
}

bool SerialIpmiInterface::config_equals(ConnectionData::ConstPtr connection_data) const {
    const SerialConnectionData& other = dynamic_cast<const SerialConnectionData&>(*connection_data);
    return
        (data.get_baud_rate() == other.get_baud_rate()) &&
        (data.get_data_bits() == other.get_data_bits()) &&
        (data.get_parity() == other.get_parity()) &&
        (data.get_stop_bits() == other.get_stop_bits()) &&
        (data.get_flow_control() == other.get_flow_control());
}

void SerialIpmiInterface::send(ipmi::IpmiInterface::NetFn netfn, ipmi::IpmiInterface::Cmd command, ipmi::IpmiInterface::Lun lun,
                            const ipmi::BridgeInfo& bridge,
                            const ipmi::IpmiInterface::ByteBuffer& request, ipmi::IpmiInterface::ByteBuffer& response) {

    /* send() fails only if critical condition is met (eg. connection is broken). New session is to be reestablished */
    try {
        intf->send(netfn, command, lun, bridge, request, response, request_failed);
        request_failed = false;
    }
    catch (std::runtime_error&) {
        request_failed = true;
        throw;
    }
}

void SerialIpmiInterface::send_unlocked(ipmi::IpmiInterface::NetFn netfn, ipmi::IpmiInterface::Cmd command,
                                        ipmi::IpmiInterface::Lun lun, const ipmi::BridgeInfo& bridge,
                                        const ipmi::IpmiInterface::ByteBuffer& request,
                                        ipmi::IpmiInterface::ByteBuffer& response) {
    /* send() fails only if critical condition is met (eg. connection is broken). New session is to be reestablished */
    try {
        intf->send_unlocked(netfn, command, lun, bridge, request, response, request_failed);
        request_failed = false;
    }
    catch (std::runtime_error&) {
        request_failed = true;
        throw;
    }
}

void SerialIpmiInterface::lock() {
    intf->lock();
}

void SerialIpmiInterface::unlock() {
    intf->unlock();
}
