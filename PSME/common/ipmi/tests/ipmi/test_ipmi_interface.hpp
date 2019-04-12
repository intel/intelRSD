/*!
 * @brief Testing implementation of ManagementController interface.
 *
 * It uses simple "IPMI" interface (which implements test message mock).
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
 * @file test_ipmi_intf.hpp
 */

#pragma once

#include <atomic>
#include "test_connection_data.hpp"
#include "test_message.hpp"

#include "ipmi/ipmi_interface.hpp"

namespace testing {

class TestIpmiInterface : public ipmi::IpmiInterface {
public:
    TestIpmiInterface() = delete;

    TestIpmiInterface(ipmi::ConnectionData::ConstPtr connection_data) :
        data(dynamic_cast<const TestConnectionData&>(*connection_data)) { }

    virtual ~TestIpmiInterface() = default;


    /*!
     * @brief Send a message
     *
     * Forwards message to the base class and updates last message timestamp.
     * If connection is not valid, it is closed and opened again.
     */
    void send(ipmi::IpmiInterface::NetFn netfn, ipmi::IpmiInterface::Cmd command, ipmi::IpmiInterface::Lun lun,
              const ipmi::BridgeInfo& bridge,
              const ipmi::IpmiInterface::ByteBuffer& request, ipmi::IpmiInterface::ByteBuffer& response) override
    {
        if ((netfn == TestRequest::NETFN) && (command == TestRequest::CMD) && (lun == ipmi::Message::CFG_LUN_DEFAULT)) {
            process_test_request(request, response, (bridge.get_level() != ipmi::BridgeInfo::Level::DIRECT));
            return;
        }
        if ((netfn == DataRequest::NETFN) && (command == DataRequest::CMD)) {
            process_data_request(request, response);
            return;
        }

        /* message cannot be processed */
        response.push_back(ipmi::InvalidDataFieldError::ERROR_CODE);
    }


    /*!
     * @brief Check if IPMI interface matches current connection data
     * @param connection_data configuration to be checked.
     * @return true if interface "equals" this configuration
     */
    bool matches(ipmi::ConnectionData::ConstPtr connection_data) const override {
        if (connection_data->get_interface_type() != TestConnectionData::INTF_TYPE) {
            return false;
        }
        const TestConnectionData& other = dynamic_cast<const TestConnectionData&>(*connection_data);
        if (other.get_descr() != data.get_descr()) {
            return false;
        }
        return true;
    }

    /*!
     * @brief Check if settings matches
     * @return always true
     */
    bool config_equals(ipmi::ConnectionData::ConstPtr) const override {
        return true;
    }

    virtual void send_unlocked(ipmi::IpmiInterface::NetFn netfn, ipmi::IpmiInterface::Cmd command, ipmi::IpmiInterface::Lun lun,
        const ipmi::BridgeInfo& bridge,
        const ipmi::IpmiInterface::ByteBuffer& request, ipmi::IpmiInterface::ByteBuffer& response) override {
        if (locked) {
            send(netfn, command, lun, bridge, request, response);
        } else {
            response.push_back(ipmi::CommandNotSupportedInPresentStateError::ERROR_CODE);
        }
    }

    /*!
     * @brief Locks the IPMI interface instance.
     */
    virtual void lock() override {
        locked = true;
    }

    /*!
     * @brief Unlocks the IPMI interface instance.
     */
    virtual void unlock() override {
        locked = false;
    }


private:
    const TestConnectionData data;
    std::uint8_t serial{0};
    std::atomic<bool> locked;

    void process_test_request(const ipmi::IpmiInterface::ByteBuffer&, ipmi::IpmiInterface::ByteBuffer& response, bool fail) {
        serial++;
        if (fail) {
            response.push_back(
                ipmi::CommandNotSupportedInPresentStateError::ERROR_CODE);
            return;
        }

        response.push_back(ipmi::COMPLETION_CODE_NORMAL);
        response.push_back(serial);

        const std::string& descr = data.get_descr();
        for (unsigned int i = 0; i < descr.length(); i++) {
            response.push_back(static_cast<std::uint8_t>(descr.at(i)));
        }
    }

    void process_data_request(const ipmi::IpmiInterface::ByteBuffer&, ipmi::IpmiInterface::ByteBuffer& response) {
        /* produces empty message */
        response.push_back(ipmi::COMPLETION_CODE_NORMAL);
    }
};

}
