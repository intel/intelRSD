/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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

#include "ssdp/ssdp_packet_parser.hpp"
#include "parser_common.hpp"
#include "ssdp/ssdp_packet_validator.hpp"
#include "gtest/gtest.h"

using namespace ssdp;

class PacketValidatorTest : public PacketParserTest {
protected:
    ~PacketValidatorTest() override;
};

PacketValidatorTest::~PacketValidatorTest() {}

TEST_F(PacketValidatorTest, ValidateMSearch) {
    SsdpPacket packet = parse_string(VALID_M_SEARCH);
    SsdpSearchRequestPacketValidator validator;

    EXPECT_EQ(SsdpPacket::Type::SEARCH_REQUEST, packet.get_type());
    EXPECT_TRUE(validator.is_valid(packet));
    EXPECT_TRUE(validate_ssdp_packet(packet));
}

TEST_F(PacketValidatorTest, ValidateMSearchResponse) {
    SsdpPacket packet = parse_string(VALID_M_SEARCH_RESPONSE);
    SsdpSearchResponsePacketValidator validator;

    EXPECT_EQ(SsdpPacket::Type::SEARCH_RESPONSE, packet.get_type());
    EXPECT_TRUE(validator.is_valid(packet));
    EXPECT_TRUE(validate_ssdp_packet(packet));
}

TEST_F(PacketValidatorTest, ValidateAliveNotify) {
    SsdpPacket packet = parse_string(VALID_NOTIFY);
    SsdpAlivePacketValidator validator;

    EXPECT_EQ(SsdpPacket::Type::NOTIFY, packet.get_type());
    EXPECT_TRUE(validator.is_valid(packet));
    EXPECT_TRUE(validate_ssdp_packet(packet));
}

TEST_F(PacketValidatorTest, ValidateByeByeNotify) {
    SsdpPacket packet = parse_string(VALID_BYEBYE_NOTIFY);
    SsdpByeByePacketValidator validator;

    EXPECT_EQ(SsdpPacket::Type::NOTIFY, packet.get_type());
    EXPECT_TRUE(validator.is_valid(packet));
    EXPECT_TRUE(validate_ssdp_packet(packet));
}
