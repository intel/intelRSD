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
 * @section DESCRIPTION
 * */

#include "ssdp/ssdp_packet_parser.hpp"
#include "parser_common.hpp"
#include "gtest/gtest.h"

using ssdp::PacketParserTest;
using ssdp::SsdpPacket;

TEST_F(PacketParserTest, ParseValidMSearch) {
    SsdpPacket packet = parse_string(ssdp::VALID_M_SEARCH);

    EXPECT_EQ(SsdpPacket::Type::SEARCH_REQUEST, packet.get_type());
    EXPECT_TRUE(packet.is_search_request());
    EXPECT_FALSE(packet.is_notify());
    EXPECT_FALSE(packet.is_search_response());
    EXPECT_EQ(ssdp::VALID_M_SEARCH, packet.to_string());
}

TEST_F(PacketParserTest, IncorrectLength) {
    EXPECT_THROW(ssdp::SsdpPacketParser<std::string> parser(ssdp::VALID_M_SEARCH, ssdp::VALID_M_SEARCH.size() + 1),
                 ssdp::PacketParserException);
}

TEST_F(PacketParserTest, CallParseTwice) {
    ssdp::SsdpPacketParser<std::string> parser(ssdp::VALID_M_SEARCH, ssdp::VALID_M_SEARCH.size());
    SsdpPacket packet = parser.parse();

    EXPECT_EQ(SsdpPacket::Type::SEARCH_REQUEST, packet.get_type());
    EXPECT_TRUE(packet.is_search_request());
    EXPECT_FALSE(packet.is_notify());
    EXPECT_FALSE(packet.is_search_response());
    EXPECT_EQ(ssdp::VALID_M_SEARCH, packet.to_string());

    auto packet2 = parser.parse();
    EXPECT_EQ(SsdpPacket::Type::UNKNOWN, packet2.get_type());
}

TEST_F(PacketParserTest, ParseValidMSearchResponse) {
    SsdpPacket packet = parse_string(ssdp::VALID_M_SEARCH_RESPONSE);

    EXPECT_EQ(SsdpPacket::Type::SEARCH_RESPONSE, packet.get_type());
    EXPECT_TRUE(packet.is_search_response());
    EXPECT_FALSE(packet.is_notify());
    EXPECT_FALSE(packet.is_search_request());
    EXPECT_EQ(ssdp::VALID_M_SEARCH_RESPONSE, packet.to_string());
}

TEST_F(PacketParserTest, ParseValidNotify) {
    SsdpPacket packet = parse_string(ssdp::VALID_NOTIFY);

    EXPECT_EQ(SsdpPacket::Type::NOTIFY, packet.get_type());
    EXPECT_TRUE(packet.is_notify());
    EXPECT_FALSE(packet.is_search_request());
    EXPECT_FALSE(packet.is_search_response());
    EXPECT_EQ(ssdp::VALID_NOTIFY, packet.to_string());
}

TEST_F(PacketParserTest, ParseValidByeByeNotify) {
    SsdpPacket packet = parse_string(ssdp::VALID_BYEBYE_NOTIFY);

    EXPECT_EQ(SsdpPacket::Type::NOTIFY, packet.get_type());
    EXPECT_TRUE(packet.is_notify());
    EXPECT_FALSE(packet.is_search_request());
    EXPECT_FALSE(packet.is_search_response());
    EXPECT_EQ(ssdp::VALID_BYEBYE_NOTIFY, packet.to_string());
}
