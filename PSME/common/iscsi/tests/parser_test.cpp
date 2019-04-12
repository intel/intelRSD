/*!
 * @brief Test of the iSCSI MDR parser.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <cstdio>
#include <fstream>
#include <sstream>
#include <arpa/inet.h>

#include "test_fixture.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "iscsi/structs/iscsi_mdr_initiator.hpp"
#include "iscsi/structs/iscsi_mdr_target.hpp"
#include "iscsi/structs/iscsi_mdr_attempt.hpp"
#include "iscsi/structs/iscsi_mdr_version.hpp"

using ::testing::Return;
using ::testing::Throw;
using namespace iscsi::parser;
using namespace iscsi::structs;

class IscsiMdrParserTest: public ::testing::Test {
protected:
    virtual void SetUp() {
        m_blob = fixture::compose_iscsi_mdr_blob(2, 3, 4);
    }

    virtual void TearDown() {
    }

    std::vector<uint8_t> m_blob;

    uint8_t* get_blob() {
        return m_blob.data();
    }
};

TEST_F(IscsiMdrParserTest, ExceptionWhenBlobIsNull) {
    ASSERT_THROW(IscsiMdrParser(nullptr, 100000), IscsiMdrParser::Exception);
}

TEST_F(IscsiMdrParserTest, ThrowsOnUnknownVersion) {
    std::vector<uint8_t> check_version;
    fixture::add_version(check_version, 2, 1);
    ASSERT_THROW(IscsiMdrParser(check_version.data(), check_version.size()), IscsiMdrEntryPoint::Exception);
}

TEST_F(IscsiMdrParserTest, ReadAndParseCompleteBlob) {
    IscsiMdrParser parser(get_blob(), m_blob.size());

    std::stringstream ss;
    ASSERT_NO_THROW(ss << parser);
}

TEST_F(IscsiMdrParserTest, IterateOverInitiators) {
    IscsiMdrParser parser(get_blob(), m_blob.size());
    char ipaddr[INET6_ADDRSTRLEN];

    auto initiators = parser.get_all<ISCSI_MDR_INITIATOR>();
    ASSERT_EQ(2, initiators.size());
    const auto& init = initiators.front();
    EXPECT_EQ(InitiatiorDhcpStatus::DISABLED, init.data.initiator_dhcp_enabled);
    EXPECT_EQ(fixture::INITIATOR_NAME, init.get_string(init.data.initiator_name));
    EXPECT_EQ(IpAddressType::IPv4, init.data.ip_address_type);

    EXPECT_EQ(ipaddr, inet_ntop(AF_INET, init.data.initiator_ip_address, ipaddr, INET_ADDRSTRLEN))
        << "Unable to convert IP address";
    EXPECT_STREQ(fixture::IPv4_ADDRESS_1, ipaddr);
    EXPECT_EQ(ipaddr, inet_ntop(AF_INET, init.data.initiator_subnet_mask, ipaddr, INET_ADDRSTRLEN))
        << "Unable to convert IP address";
    EXPECT_STREQ(fixture::IPv4_MASK, ipaddr);
    EXPECT_EQ(ipaddr, inet_ntop(AF_INET, init.data.initiator_gateway, ipaddr, INET_ADDRSTRLEN))
        << "Unable to convert IP address";
    EXPECT_STREQ(fixture::IPv4_ADDRESS_2, ipaddr);
    EXPECT_EQ(ipaddr, inet_ntop(AF_INET6, init.data.initiator_primary_dns, ipaddr, INET6_ADDRSTRLEN))
        << "Unable to convert IP address";
    EXPECT_STREQ(fixture::IPv6_ADDRESS, ipaddr);
    EXPECT_EQ(ipaddr, inet_ntop(AF_INET, init.data.initiator_secondary_dns, ipaddr, INET_ADDRSTRLEN))
        << "Unable to convert IP address";
    EXPECT_STREQ(fixture::IPv4_ADDRESS_2, ipaddr);
}

TEST_F(IscsiMdrParserTest, IterateOverTargets) {
    IscsiMdrParser parser(get_blob(), m_blob.size());
    char ipaddr[INET_ADDRSTRLEN];

    auto targets = parser.get_all<ISCSI_MDR_TARGET>();
    const auto& tgt = targets.front();
    ASSERT_EQ(3, targets.size());
    EXPECT_EQ(TargetDhcpStatus::ENABLED, tgt.data.target_dhcp_enabled);
    EXPECT_EQ(fixture::TARGET_NAME, tgt.get_string(tgt.data.target_name));
    EXPECT_EQ(IpAddressType::IPv6, tgt.data.ip_address_type);

    EXPECT_EQ(ipaddr, inet_ntop(AF_INET, tgt.data.target_ip_address, ipaddr, INET_ADDRSTRLEN))
        << "Unable to convert IP address";
    EXPECT_STREQ(fixture::IPv4_ADDRESS_1, ipaddr);

    EXPECT_EQ(0x1F90, tgt.data.target_port);
    EXPECT_EQ(0x64, tgt.data.target_lun);
    EXPECT_EQ(VlanStatus::ENABLED, tgt.data.vlan_enabled);
    EXPECT_EQ(0x0102, tgt.data.vlan_id);
    EXPECT_EQ(RouterAdvertisement::DISABLED, tgt.data.router_advertisement);
    EXPECT_EQ(AuthenticationMethod::MUTUAL_CHAP, tgt.data.authentication_method);

    EXPECT_EQ(fixture::CHAP_USER_NAME, tgt.get_string(tgt.data.chap_user_name));
    EXPECT_EQ(fixture::CHAP_USER_SECRET, tgt.get_string(tgt.data.chap_secret));
    EXPECT_EQ(fixture::CHAP_MUTUAL_NAME, tgt.get_string(tgt.data.mutual_chap_name));
    EXPECT_EQ(fixture::CHAP_MUTUAL_SECRET, tgt.get_string(tgt.data.mutual_chap_secret));
}

TEST_F(IscsiMdrParserTest, IterateOverAttempts) {
    IscsiMdrParser parser(get_blob(), m_blob.size());

    auto attempts = parser.get_all<ISCSI_MDR_ATTEMPT>();
    const auto& attempt = attempts.front();
    ASSERT_EQ(4, attempts.size());
    EXPECT_EQ(0x02, attempt.data.initiator_handle);
    EXPECT_EQ(0x04, attempt.data.target_handle);
    EXPECT_EQ(0x0400, attempt.data.connection_wait_time);
    EXPECT_EQ(0x10, attempt.data.connection_retry_count);
}

TEST_F(IscsiMdrParserTest, IterateOverVersion) {
    IscsiMdrParser parser(get_blob(), m_blob.size());

    auto attempts = parser.get_all<ISCSI_MDR_VERSION>();
    ASSERT_EQ(1, attempts.size());
    EXPECT_EQ(0x01, attempts[0].data.major_version);
    EXPECT_EQ(0x00, attempts[0].data.minor_version);
}

