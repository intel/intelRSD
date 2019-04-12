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
#include "ssdp/ssdp_packet_validator.hpp"
#include "gtest/gtest.h"

using namespace ssdp;

namespace {

constexpr const char AL_HEADER_ICASE[] = "aL";
constexpr const char AL_VALUE1[] = "http://localhost:8888";
constexpr const char AL_VALUE2[] = "https://localhost:8443";

const std::string SAMPLE_PACKET("NOTIFY * HTTP/1.1\r\n"
        "NT:urn:dmtf-org:service:redfish-rest:1\r\n"
        "AL:http://localhost:8888\r\n"
        "\r\n");
}

TEST(PacketTest, UnknownPacketType) {
    SsdpPacket packet(SsdpPacket::Type::UNKNOWN);

    EXPECT_EQ(SsdpPacket::Type::UNKNOWN, packet.get_type());
    EXPECT_FALSE(packet.is_notify());
    EXPECT_FALSE(packet.is_search_request());
    EXPECT_FALSE(packet.is_search_response());
    EXPECT_FALSE(validate_ssdp_packet(packet));
    EXPECT_EQ("UNKNOWN\r\n\r\n", packet.to_string());
}

TEST(PacketTest, NotifyPacketType) {
    SsdpPacket packet(SsdpPacket::Type::NOTIFY);

    EXPECT_EQ(SsdpPacket::Type::NOTIFY, packet.get_type());
    EXPECT_TRUE(packet.is_notify());
    EXPECT_FALSE(packet.is_search_request());
    EXPECT_FALSE(packet.is_search_response());
    EXPECT_FALSE(validate_ssdp_packet(packet));
    EXPECT_EQ("NOTIFY * HTTP/1.1\r\n\r\n", packet.to_string());
}

TEST(PacketTest, SearchRequestPacketType) {
    SsdpPacket packet(SsdpPacket::Type::SEARCH_REQUEST);

    EXPECT_EQ(SsdpPacket::Type::SEARCH_REQUEST, packet.get_type());
    EXPECT_FALSE(packet.is_notify());
    EXPECT_TRUE(packet.is_search_request());
    EXPECT_FALSE(packet.is_search_response());
    EXPECT_FALSE(validate_ssdp_packet(packet));
    EXPECT_EQ("M-SEARCH * HTTP/1.1\r\n\r\n", packet.to_string());
}

TEST(PacketTest, SearchResponsePacketType) {
    SsdpPacket packet(SsdpPacket::Type::SEARCH_RESPONSE);

    EXPECT_EQ(SsdpPacket::Type::SEARCH_RESPONSE, packet.get_type());
    EXPECT_FALSE(packet.is_notify());
    EXPECT_FALSE(packet.is_search_request());
    EXPECT_TRUE(packet.is_search_response());
    EXPECT_FALSE(validate_ssdp_packet(packet));
    EXPECT_EQ("HTTP/1.1 200 OK\r\n\r\n", packet.to_string());
}

TEST(PacketTest, SetHeader) {
    SsdpPacket packet(SsdpPacket::Type::NOTIFY);

    EXPECT_FALSE(packet.has_header(SsdpPacket::AL));
    EXPECT_FALSE(packet.has_header(AL_HEADER_ICASE));
    EXPECT_STRNE(AL_HEADER_ICASE, SsdpPacket::AL);
    EXPECT_STRCASEEQ(AL_HEADER_ICASE, SsdpPacket::AL);

    packet.set_header(SsdpPacket::AL, AL_VALUE1);
    EXPECT_TRUE(packet.has_header(SsdpPacket::AL));
    // headers are case insensitive
    EXPECT_TRUE(packet.has_header(AL_HEADER_ICASE));
    EXPECT_EQ(AL_VALUE1, packet.get_header(SsdpPacket::AL));
    // replace header
    packet.set_header(SsdpPacket::AL, AL_VALUE2);
    EXPECT_TRUE(packet.has_header(SsdpPacket::AL));
    EXPECT_TRUE(packet.has_header(AL_HEADER_ICASE));
    EXPECT_EQ(AL_VALUE2, packet.get_header(SsdpPacket::AL));
    // replace again with AL_HEADER_ICASE
    packet.set_header(AL_HEADER_ICASE, AL_VALUE1);
    EXPECT_TRUE(packet.has_header(AL_HEADER_ICASE));
    EXPECT_TRUE(packet.has_header(SsdpPacket::AL));
    EXPECT_EQ(AL_VALUE1, packet.get_header(SsdpPacket::AL));
}

TEST(PacketTest, ToString) {
    SsdpPacket packet(SsdpPacket::Type::NOTIFY);
    packet.set_header(SsdpPacket::NT, "urn:dmtf-org:service:redfish-rest:1");
    packet.set_header(SsdpPacket::AL, AL_VALUE1);
    EXPECT_EQ(SAMPLE_PACKET, packet.to_string());
}
