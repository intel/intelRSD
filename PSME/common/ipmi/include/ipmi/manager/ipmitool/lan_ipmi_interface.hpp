/*!
 * @brief Session implementation for "lan" communication interface.
 *
 * Implementation of communication over ipmitool's "lan" interface. This class controlls
 * session if kept by "other" side.
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
 * @file ipmitool/lan_ipmi_intf.hpp
 */

#pragma once

#include "ipmi/ipmi_interface.hpp"
#include "ipmi/manager/ipmitool/lan_connection_data.hpp"

#include <chrono>

namespace ipmi {
namespace manager {
namespace ipmitool {

class IpmiIntfImpl;

class LanIpmiInterface: public ipmi::IpmiInterface {

public:
    LanIpmiInterface(ConnectionData::ConstPtr connection_data);

    virtual ~LanIpmiInterface();

protected:
    /*!
     * @brief Send a message
     *
     * Implementation for ipmitool's lan interface.
     * Forwards message to the ipmitool interface and updates last message timestamp.
     * If is it very first message, connection is established.
     * If connection is not valid, it is closed and opened again.
     *
     * @param netfn network function
     * @param command command
     * @param lun logical unit number
     * @param bridge bridging information
     * @param request bytes to be sent
     * @param[out] response received bytes
     */
    void send(
        ipmi::IpmiInterface::NetFn netfn, ipmi::IpmiInterface::Cmd command, ipmi::IpmiInterface::Lun lun,
        const ipmi::BridgeInfo& bridge,
        const ipmi::IpmiInterface::ByteBuffer& request, ipmi::IpmiInterface::ByteBuffer& response) override;

    /*!
    * @brief Sends request and wait for response
    *
    * Connection is active when object exist. Sending a message means just passing
    * all information from the request to the channel. This send is not multithread
    * safe, it has to be encapsulated with a pair of lock/unlock commands. The use
    * of RAII is highly encouraged. Useful for sending bundled IPMI commands.
    *
    * @param netfn network function
    * @param command command
    * @param lun logical unit number
    * @param bridge bridging information
    * @param request bytes to be sent
    * @param[out] response received bytes
    */
   void send_unlocked(
       NetFn netfn, Cmd command, Lun lun,
       const BridgeInfo& bridge,
       const ByteBuffer& request, ByteBuffer& response) override;

   /*!
    * @brief Locks the IPMI interface instance.
    */
   void lock() override;

   /*!
    * @brief Unlocks the IPMI interface instance.
    */
   void unlock() override;

    /*!
     * @brief Check if IPMI interface matches current connection data
     * @param connection_data configuration to be checked.
     * @return true if interface "equals" given configuration
     */
    bool matches(ipmi::ConnectionData::ConstPtr connection_data) const override;

    /*!
     * @brief Check if IPMI interface configuration matches given connection data
     * @param connection_data configuration to be checked.
     * @return true if interface uses same configuration.
     */
    bool config_equals(ConnectionData::ConstPtr connection_data) const override;

private:
    /*! Connection data to be used by matches()/config_equals() */
    const LanConnectionData data;

    /*! ipmitool interface to be used */
    const std::unique_ptr<IpmiIntfImpl> intf;

    /*!
     * @brief checking if connection is valid
     * @return true if message was sent not earlier than 30s ago
     */
    bool is_valid() const;
    std::chrono::milliseconds last_sent{};
};

}
}
}
