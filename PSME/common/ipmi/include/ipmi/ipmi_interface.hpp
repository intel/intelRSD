/*!
 * @brief Interface to communicate with transport library.
 *
 * Represents IPMI session. If session is invalid, communication channel is closed
 * and opened on sending a (next) message
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
 * @file ipmi/ipmi_intf.hpp
 */

#pragma once

#include "ipmi/connection_data.hpp"
#include "ipmi/bridge_info.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace ipmi {

class IpmiInterface {

    /*!
     * @brief Looks for interfaces in factory cache
     *
     * Method matches() creates key of built interfaces
     */
    friend class IpmiInterfaceFactory;

    /*!
     * @brief Checks if configuration matches. Sends messages.
     */
    friend class IpmiController;

public:
    using Cmd = std::uint8_t;
    using NetFn = std::uint8_t;
    using Lun = std::uint8_t;

    using ByteBuffer = std::vector<std::uint8_t>;

    using Ptr = std::shared_ptr<IpmiInterface>;
    using ConstPtr = std::shared_ptr<const IpmiInterface>;

protected:
    IpmiInterface();
    virtual ~IpmiInterface();

    /*!
     * @brief Sends request and wait for response
     *
     * Connection is active when object exist. Sending a message means just passing
     * all information from the request to the channel. This send is multithread safe.
     * Useful for running single ipmi commands.
     *
     * @param netfn network function
     * @param command command
     * @param lun logical unit number
     * @param bridge bridging information
     * @param request bytes to be sent
     * @param[out] response received bytes
     */
    virtual void send(
        NetFn netfn, Cmd command, Lun lun,
        const BridgeInfo& bridge,
        const ByteBuffer& request, ByteBuffer& response) = 0;

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
    virtual void send_unlocked(
            NetFn netfn, Cmd command, Lun lun,
            const BridgeInfo& bridge,
            const ByteBuffer& request, ByteBuffer& response) = 0;

    /*!
     * @brief Locks the IPMI interface instance.
     */
    virtual void lock() = 0;

    /*!
     * @brief Unlocks the IPMI interface instance.
     */
    virtual void unlock() = 0;

    /*!
     * @brief Check if IPMI interface matches given connection data
     * @param connection_data configuration to be checked.
     * @return true if interface "equals" this configuration
     */
    virtual bool matches(ConnectionData::ConstPtr connection_data) const = 0;

    /*!
     * @brief Check if IPMI interface configuration matches given connection data
     * @param connection_data configuration to be checked.
     * @return true if interface uses same configuration.
     */
    virtual bool config_equals(ConnectionData::ConstPtr connection_data) const = 0;
};

}

