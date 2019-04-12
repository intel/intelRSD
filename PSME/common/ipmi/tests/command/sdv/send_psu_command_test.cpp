/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * */

#include "ipmi/command/sdv/rmm/send_psu_command.hpp"
#include <gtest/gtest.h>
#include <tuple>

using namespace ipmi::command::sdv;

using OperationCode = request::SendPsuCommand::OperationCode;
using Command = request::SendPsuCommand::Command;

namespace {

constexpr const std::uint8_t PSU_ID = 2;

// X = Y * 2^N
// X - expected real value (double)
// Y, N from raw reading in linear format
const std::vector<std::tuple<ipmi::IpmiInterface::ByteBuffer, double>> expected_power_values = {
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0x00, 0x00}, 0.0),  // N=0  Y=0
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0x01, 0x00}, 1.0),  // N=0  Y=1
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0x01, 0xf0}, 0.25), // N=-2 Y=1
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0x01, 0xf8}, 0.5),  // N=-1 Y=1
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0xfc, 0xff}, -2.0), // N=-1 Y=-4
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0xff, 0xff}, -0.5),  // N=-1 Y=-1
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0xff, 0x7B}, 33521664.0),  // N=15 Y=1023
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0x00, 0x7C}, -33554432.0),  // N=15 Y=-1024
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0xff, 0x83}, 0.0156097412109375),  // N=-16 Y=1023
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0x00, 0x84}, -0.015625)  // N=-16 Y=-1024
};

const std::vector<std::tuple<ipmi::IpmiInterface::ByteBuffer, std::string>> expected_string_values = {
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, std::string>({0x00, 0x54, 0x45, 0x53, 0x54, 0x20, 0x30, 0x31, 0x20, 0x41}, "TEST 01 A"),
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, std::string>({0x00, 0x54, 0x45, 0x53, 0x54, 0x20, 0x30, 0x31}, "TEST 01"),
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, std::string>({0x00, 0x54, 0x45, 0x53, 0x54}, "TEST")
};

}

TEST(SendPsuReadStatusRequest, PackRequestData) {
    request::SendPsuReadStatusWord request;
    request.set_psu_id(PSU_ID);
    const auto req_data = request.do_pack();
    ASSERT_EQ(3, req_data.size());
    ASSERT_EQ(PSU_ID, req_data[0]);
    ASSERT_EQ(static_cast<std::uint8_t>(OperationCode::READ_WORD), req_data[1]);
    ASSERT_EQ(static_cast<std::uint8_t>(Command::STATUS_WORD), req_data[2]);
}

TEST(SendPsuReadStatusWordResponse, GetStatusWordWithoutUnpack) {
    response::SendPsuReadStatusWord response;
    ASSERT_EQ(0, response.get_status_word());
}

TEST(SendPsuReadStatusWordResponse, StatusOk) {
    ipmi::IpmiInterface::ByteBuffer data = {0x00, 0x00, 0x00};
    response::SendPsuReadStatusWord response;
    response.do_unpack(data);
    ASSERT_EQ(0, response.get_status_word());
}

TEST(SendPsuReadPowerOutputRequest, PackRequestData) {
    request::SendPsuReadPowerOutput request;
    request.set_psu_id(PSU_ID);
    const auto req_data = request.do_pack();
    ASSERT_EQ(3, req_data.size());
    ASSERT_EQ(PSU_ID, req_data[0]);
    ASSERT_EQ(static_cast<std::uint8_t>(OperationCode::READ_WORD), req_data[1]);
    ASSERT_EQ(static_cast<std::uint8_t>(Command::READ_POUT), req_data[2]);
}

TEST(SendPsuReadPowerOutputResponse, GetOutputPowerValueWithoutUnpack) {
    response::SendPsuReadPowerOutput response;
    ASSERT_DOUBLE_EQ(0.0, response.get_power_output_value());
}

TEST(SendPsuReadPowerOutputResponse, GetOutputPowerValue) {
    response::SendPsuReadPowerOutput response;

    for (const auto& expected: expected_power_values) {
        const auto& bytes = std::get<0>(expected);
        response.do_unpack(bytes);
        ASSERT_DOUBLE_EQ(std::get<1>(expected), response.get_power_output_value());
    }
}

TEST(SendPsuReadPowerCapacityRequest, PackRequestData) {
    request::SendPsuReadPowerCapacity request;
    request.set_psu_id(PSU_ID);
    const auto req_data = request.do_pack();
    ASSERT_EQ(3, req_data.size());
    ASSERT_EQ(PSU_ID, req_data[0]);
    ASSERT_EQ(static_cast<std::uint8_t>(OperationCode::READ_WORD), req_data[1]);
    ASSERT_EQ(static_cast<std::uint8_t>(Command::MFR_POUT_MAX), req_data[2]);
}

TEST(SendPsuReadPowerCapacityResponse, GetPowerCapacityValueWithoutUnpack) {
    response::SendPsuReadPowerCapacity response;
    ASSERT_DOUBLE_EQ(0.0, response.get_power_capacity_value());
}

TEST(SendPsuReadPowerCapacityResponse, GetPowerCapacityValue) {
    response::SendPsuReadPowerCapacity response;

    for (const auto& expected: expected_power_values) {
        const auto& bytes = std::get<0>(expected);
        response.do_unpack(bytes);
        ASSERT_DOUBLE_EQ(std::get<1>(expected), response.get_power_capacity_value());
    }
}

TEST(SendPsuReadSerialRequest, PackRequestData) {
    request::SendPsuReadSerial request;
    request.set_psu_id(PSU_ID);
    const auto req_data = request.do_pack();
    ASSERT_EQ(3, req_data.size());
    ASSERT_EQ(PSU_ID, req_data[0]);
    ASSERT_EQ(static_cast<std::uint8_t>(OperationCode::READ_BLOCK), req_data[1]);
    ASSERT_EQ(static_cast<std::uint8_t>(Command::MFR_SERIAL), req_data[2]);
}

TEST(SendPsuReadSerialResponse, SendPsuReadSerialWithoutUnpack) {
    response::SendPsuReadSerial response;
    ASSERT_STREQ("", response.get_serial().c_str());
}

TEST(SendPsuReadSerialResponse, SendPsuReadSerialValue) {
    response::SendPsuReadSerial response;

    for (const auto& expected: expected_string_values) {
        const auto& bytes = std::get<0>(expected);
        response.do_unpack(bytes);
        ASSERT_STREQ(std::get<1>(expected).c_str(), response.get_serial().c_str());
    }
}

TEST(SendPsuReadManufacturerRequest, PackRequestData) {
    request::SendPsuReadManufacturer request;
    request.set_psu_id(PSU_ID);
    const auto req_data = request.do_pack();
    ASSERT_EQ(3, req_data.size());
    ASSERT_EQ(PSU_ID, req_data[0]);
    ASSERT_EQ(static_cast<std::uint8_t>(OperationCode::READ_BLOCK), req_data[1]);
    ASSERT_EQ(static_cast<std::uint8_t>(Command::MFR_ID), req_data[2]);
}

TEST(SendPsuReadManufacturerResponse, SendPsuReadManufacturerWithoutUnpack) {
    response::SendPsuReadManufacturer response;
    ASSERT_STREQ("", response.get_manufacturer().c_str());
}

TEST(SendPsuReadManufacturerResponse, SendPsuReadManufacturerValue) {
    response::SendPsuReadManufacturer response;

    for (const auto& expected: expected_string_values) {
        const auto& bytes = std::get<0>(expected);
        response.do_unpack(bytes);
        ASSERT_STREQ(std::get<1>(expected).c_str(), response.get_manufacturer().c_str());
    }
}

TEST(SendPsuReadModelRequest, PackRequestData) {
    request::SendPsuReadModel request;
    request.set_psu_id(PSU_ID);
    const auto req_data = request.do_pack();
    ASSERT_EQ(3, req_data.size());
    ASSERT_EQ(PSU_ID, req_data[0]);
    ASSERT_EQ(static_cast<std::uint8_t>(OperationCode::READ_BLOCK), req_data[1]);
    ASSERT_EQ(static_cast<std::uint8_t>(Command::MFR_MODEL), req_data[2]);
}

TEST(SendPsuReadModelResponse, SendPsuReadModelWithoutUnpack) {
    response::SendPsuReadModel response;
    ASSERT_STREQ("", response.get_model().c_str());
}

TEST(SendPsuReadModelResponse, SendPsuReadModelValue) {
    response::SendPsuReadModel response;

    for (const auto& expected: expected_string_values) {
        const auto& bytes = std::get<0>(expected);
        response.do_unpack(bytes);
        ASSERT_STREQ(std::get<1>(expected).c_str(), response.get_model().c_str());
    }
}

TEST(SendPsuReadRevisionRequest, PackRequestData) {
    request::SendPsuReadRevision request;
    request.set_psu_id(PSU_ID);
    const auto req_data = request.do_pack();
    ASSERT_EQ(3, req_data.size());
    ASSERT_EQ(PSU_ID, req_data[0]);
    ASSERT_EQ(static_cast<std::uint8_t>(OperationCode::READ_BLOCK), req_data[1]);
    ASSERT_EQ(static_cast<std::uint8_t>(Command::MFR_REVISION), req_data[2]);
}

TEST(SendPsuReadRevisionResponse, SendPsuReadRevisionWithoutUnpack) {
    response::SendPsuReadRevision response;
    ASSERT_STREQ("", response.get_revision().c_str());
}

TEST(SendPsuReadRevisionResponse, SendPsuReadRevisionValue) {
    response::SendPsuReadRevision response;

    for (const auto& expected: expected_string_values) {
        const auto& bytes = std::get<0>(expected);
        response.do_unpack(bytes);
        ASSERT_STREQ(std::get<1>(expected).c_str(), response.get_revision().c_str());
    }
}
