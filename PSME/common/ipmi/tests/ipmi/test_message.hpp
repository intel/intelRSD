/*!
 * @brief Testing message for IPMI interface.
 *
 * This message (30xff) is the only one handled by testing management controller.
 * Request is empty, response contains description (from the controller) and
 * ordinary number in the session.
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
 * @file test_message.hpp
 */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include <cstdint>

namespace testing {

class TestRequest : public ipmi::Request {
public:

    constexpr static ipmi::IpmiInterface::NetFn NETFN = 0x30;
    constexpr static ipmi::IpmiInterface::Cmd CMD = 0xff;

    /*!
     * @brief Default constructor.
     */
    TestRequest() : ipmi::Request(NETFN, CMD) { }

    /*!
     * @brief Default destructor.
     */
    virtual ~TestRequest() = default;

    const char* get_command_name() const override {
        return "TestRequest";
    }

protected:
    void pack(ipmi::IpmiInterface::ByteBuffer& data) const override {
        data.clear();
    }
};

/*!
 * @brief Response message for testing IPMI interface.
 *
 * Contains description and serial of the message (sent).
 */
class TestResponse : public ipmi::Response {
public:
    /*!
     * @brief Default constructor.
     */
    TestResponse() : ipmi::Response(TestRequest::NETFN, TestRequest::CMD, 0) {}

    /*!
     * @brief Default destructor.
     */
    virtual ~TestResponse() = default;

    const char* get_command_name() const override {
        return "TestResponse";
    }

    const std::string& get_descr() const {
        return descr;
    }
    std::uint8_t get_serial() const {
        return serial;
    }

protected:
    void unpack(const ipmi::IpmiInterface::ByteBuffer& data) override {
        descr.clear();
        serial = 0;
        for (unsigned i = 0; i < data.size(); i++) {
            switch (i) {
                case ipmi::Response::OFFSET_COMPLETION_CODE:
                    break;
                case 1:
                    serial = data[i];
                    break;
                default:
                    descr.append(1, static_cast<char>(data[i]));
                    break;
            }
        }
    }

private:
    std::string descr{};
    std::uint8_t serial{};
};

class DataRequest : public ipmi::Request {
public:

    constexpr static ipmi::IpmiInterface::NetFn NETFN = 0x30;
    constexpr static ipmi::IpmiInterface::Cmd CMD = 0xfe;

    /*!
     * @brief Default constructor.
     */
    DataRequest(unsigned value) : ipmi::Request(NETFN, CMD), bytes(value) { }

    /*!
     * @brief Default destructor.
     */
    virtual ~DataRequest() = default;

    const char* get_command_name() const override {
        return "DataRequest";
    }

protected:
    void pack(ipmi::IpmiInterface::ByteBuffer& data) const override {
        const std::uint8_t* ptr = (const std::uint8_t*)&bytes;

        data.clear();
        for (unsigned i = 0; i < 4; i++) {
            data.push_back(*(ptr++));
        }
    }

private:
    unsigned bytes;
};

class EmptyResponse : public ipmi::Response {
public:
    /*!
     * @brief Default constructor.
     *
     * Response must be 1 byte long: always CC is stored
     */
    EmptyResponse() : ipmi::Response(DataRequest::NETFN, DataRequest::CMD, 1) {}

    /*!
     * @brief Default destructor.
     */
    virtual ~EmptyResponse() = default;

    const char* get_command_name() const override {
        return "EmptyResponse";
    }

    unsigned get_size() const {
        return size;
    }

protected:
    void unpack(const ipmi::IpmiInterface::ByteBuffer& data) override {
        size = (unsigned) data.size();
    }

private:
    unsigned size{};
};

}

