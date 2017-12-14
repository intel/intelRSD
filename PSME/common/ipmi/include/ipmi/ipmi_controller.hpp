/*!
 * @brief Generic IPMI controller
 *
 * Generic "interface" class. It should be used as a reference to "real" controller.
 * Handle all passed requests. Connection data is to be passed as shared ptr,
 * no references are implemented (memory management actually is to be done in
 * the code)
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @header{Filesystem}
 * @file ipmi/ipmi_controller.hpp
 */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/bridge_info.hpp"
#include "ipmi/connection_data.hpp"
#include "ipmi/ipmi_interface.hpp"

namespace testing {

class IpmiInterface_Factory_Test;

}

namespace ipmi {

class IpmiController {

    friend class testing::IpmiInterface_Factory_Test;

public:
    using Ptr = std::shared_ptr<IpmiController>;

    IpmiController(ConnectionData::Ptr connection_data);

    IpmiController() = delete;

    IpmiController(const IpmiController&) = default;

    IpmiController(IpmiController&&) = default;

    virtual ~IpmiController();

    /*!
     * @brief Send message using bridge info.
     *
     * Default implementation with bridging.
     *
     * @param request message to be sent
     * @param via information about bridges
     * @param response received response
     */
    virtual void send(const Request& request, const BridgeInfo& via, Response& response);

    /*!
     * @brief Send a message.
     *
     * Default implementation without bridging.
     *
     * @param request message to be sent.
     * @param response message with output/result.
     */
    virtual void send(const Request& request, Response& response);

    /*!
     * @brief Release IPMI interface used by the controller
     *
     * Interface is closed only when all controllers have the connection closed.
     * The last one causes physical removal of the interface.
     */
    void close();

protected:
    /*!
     * @brief Access to controller connection data
     *
     * Returned data is to be modified. Next send(req) will check if the data
     * changed, and if necessary build new interace.
     * TODO method to be removed, connection data should be always const
     * This is done all over the code, default IpmiController doesn't use construct with
     * all settings/connection data pattern.
     *
     * @return data to be modified by the controller
     */
    ConnectionData::Ptr data_to_modify() {
        check_interface = true;
        return connection_data;
    }

    /*!
     * @brief Access to controller connection data
     * @return data to get values for the controller
     */
    ConnectionData::ConstPtr data() const {
        return connection_data;
    }

private:
    ConnectionData::Ptr connection_data;
    bool check_interface{true};
    IpmiInterface::Ptr interface{};

    /*!
     * @brief Send message using bridge info.
     *
     * Sending a message checks connection (if opened or connection data changed),
     * packs request, pass <<raw>> message via interface and unpacks received response.
     * Implementation with bridge.
     *
     * @param request message to be sent
     * @param via information about bridges
     * @param[out] response received response
     */
    void do_send(const Request& request, const BridgeInfo& via, Response& response);
};

}
