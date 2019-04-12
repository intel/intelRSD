/*!
 * @brief Session implementation for "lan" communication interface.
 *
 * Current implementation of communication over ipmitool's "lan" interface.
 * It discuss with ipmitool library by the means of its structs.
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
 * @file ipmitool/lan_ipmi_intf.cpp
 */

#include "ipmi/manager/ipmitool/lan_ipmi_interface.hpp"
#include "ipmi/manager/ipmitool/lan_connection_data.hpp"
#include "ipmi/manager/ipmitool/ipmi_intf_impl.hpp"

#include <stdexcept>

using namespace ipmi::manager::ipmitool;

namespace {

std::chrono::milliseconds get_millis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}

void initialize_intf(ipmi_intf* intf, ipmi::ConnectionData::ConstPtr connection_data) {
    const LanConnectionData& data = dynamic_cast<const LanConnectionData&>(*connection_data);

    ipmi_intf_session_set_hostname(intf, const_cast<char*>(data.get_ip().c_str()));
    ipmi_intf_session_set_port(intf, static_cast<int>(data.get_port()));
    ipmi_intf_session_set_username(intf, const_cast<char*>(data.get_username().c_str()));
    ipmi_intf_session_set_password(intf, const_cast<char*>(data.get_password().c_str()));
}

}

LanIpmiInterface::LanIpmiInterface(ipmi::ConnectionData::ConstPtr connection_data) :
    data(dynamic_cast<const LanConnectionData&>(*connection_data)),
    intf(new IpmiIntfImpl(connection_data, initialize_intf)) { }

LanIpmiInterface::~LanIpmiInterface() { }

bool LanIpmiInterface::matches(ipmi::ConnectionData::ConstPtr connection_data) const {

    if (connection_data->get_interface_type() != LanConnectionData::INTF_TYPE) {
        return false;
    }
    const LanConnectionData& other = dynamic_cast<const LanConnectionData&>(*connection_data);
    return (other.get_ip() == data.get_ip()) &&
           (other.get_port() == data.get_port()) &&
           (other.get_username() == data.get_username());
}

bool LanIpmiInterface::config_equals(ConnectionData::ConstPtr connection_data) const {

    /* both matches.. but it is necessary to check if "additional" setting match as well */
    const LanConnectionData& other = dynamic_cast<const LanConnectionData&>(*connection_data);
    return (other.get_password() == data.get_password());
}

/*!
 * @brief Send a message.
 *
 * Connection is valid only for time specified in config file.
 * After that time, the conection must be reconnected.
 */
void LanIpmiInterface::send(ipmi::IpmiInterface::NetFn netfn, ipmi::IpmiInterface::Cmd command, ipmi::IpmiInterface::Lun lun,
                            const ipmi::BridgeInfo& bridge,
                            const ipmi::IpmiInterface::ByteBuffer& request, ipmi::IpmiInterface::ByteBuffer& response) {

    intf->send(netfn, command, lun, bridge, request, response, !is_valid());

    /* update last sent message time, to check if valid */
    last_sent = get_millis();
}

void LanIpmiInterface::send_unlocked(ipmi::IpmiInterface::NetFn netfn, ipmi::IpmiInterface::Cmd command,
                                     ipmi::IpmiInterface::Lun lun, const ipmi::BridgeInfo& bridge,
                                     const ipmi::IpmiInterface::ByteBuffer& request,
                                     ipmi::IpmiInterface::ByteBuffer& response) {

    intf->send_unlocked(netfn, command, lun, bridge, request, response, !is_valid());

    /* update last sent message time, to check if valid */
    last_sent = get_millis();
}

/*!
 * @brief Locks the IPMI interface instance.
 */
void LanIpmiInterface::lock() {
    intf->lock();
}

/*!
 * @brief Unlocks the IPMI interface instance.
 */
void LanIpmiInterface::unlock() {
    intf->unlock();
}

/*!
 * @brief Check if connection is valid
 * @return true if no message was sent in session validity time.
 */
bool LanIpmiInterface::is_valid() const {
    return (get_millis() - last_sent < data.get_session_timeout());
}
