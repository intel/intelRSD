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

#include "ssdp/ssdp_packet_factory.hpp"
#include "ssdp/ssdp_service_config.hpp"
#include "gtest/gtest.h"

using namespace ssdp;

class PacketFactoryTest : public ::testing::Test {
protected:
    std::unique_ptr<SsdpPacketFactory> packet_factory{};

    void SetUp() override {
        auto config = std::make_shared<const SsdpServiceConfig>(create_config());
        packet_factory.reset(new SsdpPacketFactory(config));
    }

    void TearDown() override { }

    SsdpServiceConfig create_config() {
        SsdpServiceConfig config;
        config.set_announce_interval(std::chrono::seconds(15));
        config.set_service_url("http://localhost:8888/redfish/v1");
        config.set_service_uuid("7e698e98-f1cc-11e5-bc07-4325b1f9a7b9");
        return config;
    }

    ~PacketFactoryTest();
};

PacketFactoryTest::~PacketFactoryTest() {}

TEST_F(PacketFactoryTest, CreateSearchRequest) {
    auto packet = packet_factory->create_ssdp_packet(SsdpPacket::Type::SEARCH_REQUEST);

    EXPECT_EQ(SsdpPacket::Type::SEARCH_REQUEST, packet.get_type());
    EXPECT_TRUE(packet.is_search_request());
    EXPECT_FALSE(packet.is_notify());
    EXPECT_FALSE(packet.is_search_response());
    EXPECT_TRUE(packet.has_header(SsdpPacket::MX));
    EXPECT_TRUE(packet.has_header(SsdpPacket::MAN));
    EXPECT_EQ(SsdpPacket::SSDP_DISCOVER, packet.get_header(SsdpPacket::MAN));
}

TEST_F(PacketFactoryTest, CreateSearchResponse) {
    auto packet = packet_factory->create_ssdp_packet(SsdpPacket::Type::SEARCH_RESPONSE);

    EXPECT_EQ(SsdpPacket::Type::SEARCH_RESPONSE, packet.get_type());
    EXPECT_TRUE(packet.is_search_response());
    EXPECT_FALSE(packet.is_notify());
    EXPECT_FALSE(packet.is_search_request());
}

TEST_F(PacketFactoryTest, CreateAliveNotify) {
    std::vector<SsdpPacket> alive_packets{};
    alive_packets.emplace_back(packet_factory->create_ssdp_packet(SsdpPacket::Type::NOTIFY));
    alive_packets.emplace_back(packet_factory->create_ssdp_packet({SsdpPacket::Type::NOTIFY,
                                                            SsdpPacket::SSDP_ALIVE_STR}));
    for (const auto& packet : alive_packets) {
        EXPECT_EQ(SsdpPacket::Type::NOTIFY, packet.get_type());
        EXPECT_TRUE(packet.is_notify());
        EXPECT_FALSE(packet.is_search_request());
        EXPECT_FALSE(packet.is_search_response());
        EXPECT_TRUE(packet.has_header(SsdpPacket::AL));
        EXPECT_EQ("http://localhost:8888/redfish/v1", packet.get_header(SsdpPacket::AL));
        EXPECT_TRUE(packet.has_header(SsdpPacket::NTS));
        EXPECT_EQ(SsdpPacket::SSDP_ALIVE_STR, packet.get_header(SsdpPacket::NTS));
    }
}

TEST_F(PacketFactoryTest, CreateByeByeNotify) {
    auto packet = packet_factory->create_ssdp_packet(
                        {SsdpPacket::Type::NOTIFY, SsdpPacket::SSDP_BYEBYE_STR});

    EXPECT_EQ(SsdpPacket::Type::NOTIFY, packet.get_type());
    EXPECT_TRUE(packet.is_notify());
    EXPECT_FALSE(packet.is_search_request());
    EXPECT_FALSE(packet.is_search_response());
    EXPECT_TRUE(packet.has_header(SsdpPacket::NTS));
    EXPECT_EQ(SsdpPacket::SSDP_BYEBYE_STR, packet.get_header(SsdpPacket::NTS));
}
