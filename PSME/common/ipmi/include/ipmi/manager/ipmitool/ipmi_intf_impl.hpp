/*!
 * @brief Transport implementation for ipmitool library.
 *
 * Class directly uses ipmitool library by the means of its structs.
 * This is only wrapper to the library.
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
 * @file ipmitool/ipmi_intf_impl.hpp
 */

#include "ipmi/connection_data.hpp"
#include "ipmi/message.hpp"
#include "ipmi/bridge_info.hpp"



extern "C" {
#include <ipmitool/ipmi_sol.h>
#include <ipmitool/ipmi_intf.h>
}



#include <functional>
#include <mutex>



namespace ipmi {
namespace manager {
namespace ipmitool {

class IpmiIntfImpl final {
public:
    using InitFn = std::function<void(ipmi_intf*, ipmi::ConnectionData::ConstPtr)>;


    /*!
     * @brief Create access to ipmitool library wrapper
     * @param connection_data configuration to be passed to init
     * @param init implementation if initialization
     */
    IpmiIntfImpl(ipmi::ConnectionData::ConstPtr connection_data, InitFn init);


    /*!
     * @brief non-virtual destructor
     */
    ~IpmiIntfImpl();


    /*!
     * @brief Send request and wait for the response. Multithread safe version.
     * @param netfn network function
     * @param command command
     * @param lun logical unit number
     * @param bridge bridging info
     * @param request request data to be sent
     * @param response received response
     * @param reconnect is session is expected to be ended by other side
     */
    void send(ipmi::IpmiInterface::NetFn netfn, ipmi::IpmiInterface::Cmd command, ipmi::IpmiInterface::Lun lun,
              const ipmi::BridgeInfo& bridge,
              const ipmi::IpmiInterface::ByteBuffer& request, ipmi::IpmiInterface::ByteBuffer& response,
              bool reconnect);

    /*!
     * @brief Send request and wait for the response. Multithread unsafe version.
     * @param netfn network function
     * @param command command
     * @param lun logical unit number
     * @param bridge bridging info
     * @param request request data to be sent
     * @param response received response
     * @param reconnect is session is expected to be ended by other side
     */
    void send_unlocked(ipmi::IpmiInterface::NetFn netfn, ipmi::IpmiInterface::Cmd command, ipmi::IpmiInterface::Lun lun,
                  const ipmi::BridgeInfo& bridge,
                  const ipmi::IpmiInterface::ByteBuffer& request, ipmi::IpmiInterface::ByteBuffer& response,
                  bool reconnect);

    /*!
     * @brief Lock the IPMI interface.
     */
    void lock();

    /*!
     * @brief Unlock the IPMI interface.
     */
    void unlock();


private:
    void open();


    void close();


    ipmi_intf m_ipmi_intf;

    /*!
     * @brief Mutex to allow a thread at once
     *
     * All requests in the session are synchronized. Requests between sessions does not require synchronization
     * (two resources of any type might be run simultanously)
     */
    std::mutex mutex{};
};

}
}
}
