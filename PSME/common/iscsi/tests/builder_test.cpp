/*!
 * @brief Test of the iSCSI MDR builder.
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

#include "builder_fixture.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "iscsi/structs/iscsi_mdr_initiator.hpp"
#include "iscsi/structs/iscsi_mdr_target.hpp"
#include "iscsi/structs/iscsi_mdr_attempt.hpp"
#include "iscsi/structs/iscsi_mdr_version.hpp"

#include "iscsi/iscsi_mdr_builder.hpp"
#include "iscsi/iscsi_mdr_parser.hpp"
#include "mdr/printers.hpp"

#include <functional>
#include <vector>

using ::testing::Return;
using ::testing::Throw;
using namespace iscsi::parser;
using namespace iscsi::builder;
using namespace iscsi::structs;

namespace {

struct DeviceFunctionContext final {

    using NetworkDeviceFunctionGetter = std::function<const fixture::NetworkDeviceFunction&(void)>;

    /*!
     * @brief Constructor.
     */
    DeviceFunctionContext(NetworkDeviceFunctionGetter network_device_functions_getter = nullptr) :
        get_network_device_function(network_device_functions_getter) {
    }

    /*!
     * @brief Convert model IPAddressType to its iSCSI MDR counterpart.
     *
     * @param at the model IP address type.
     * @return iSCSI MDR IP address type.
     */
    template<typename T>
    iscsi::structs::IpAddressType get_ip_type(const T& at) const {
        return static_cast<iscsi::structs::IpAddressType>(at);
    }

    /*!
     * @brief Convert model DHCP status to its iSCSI MDR counterpart.
     *
     * @param enabled the DHCP status.
     * @return iSCSI MDR DHCP status.
     */
    iscsi::structs::TargetDhcpStatus get_dhcp_enabled(bool enabled) const {
        return enabled ? iscsi::structs::TargetDhcpStatus::ENABLED : iscsi::structs::TargetDhcpStatus::DISABLED;
    }

    /*!
     * @brief Convert model VLAN status to its iSCSI MDR counterpart.
     *
     * @param enabled the VLAN status.
     * @return iSCSI MDR VLAN status.
     */
    iscsi::structs::VlanStatus get_vlan_enabled(bool enabled) const {
        return enabled ? iscsi::structs::VlanStatus::ENABLED : iscsi::structs::VlanStatus::DISABLED;
    }

    /*!
     * @brief Convert model router advertisement status to its iSCSI MDR counterpart.
     *
     * @param enabled the router advertisement status.
     * @return iSCSI MDR router advertisement status.
     */
    iscsi::structs::RouterAdvertisement get_router_advertisment(bool enabled) const {
        return enabled ? iscsi::structs::RouterAdvertisement::ENABLED : iscsi::structs::RouterAdvertisement::DISABLED;
    }

    /*!
     * @brief Convert model authentication method to its iSCSI MDR counterpart.
     *
     * @param am the model authentication method.
     * @return iSCSI MDR authentication method.
     */
    template<typename T>
    iscsi::structs::AuthenticationMethod get_authentication_method(const T& am) const {
        return static_cast<iscsi::structs::AuthenticationMethod>(am);
    }

    NetworkDeviceFunctionGetter get_network_device_function;
};

}  // namespace

class IscsiMdrBuilderTest: public ::testing::Test {
protected:
    virtual void SetUp() {
        auto getter = [this]() -> fixture::NetworkDeviceFunction {return m_net_dev;};
        m_context.emplace_back(getter);
    }

    virtual void TearDown() {
    }

    std::vector<DeviceFunctionContext> m_context;

private:
    fixture::NetworkDeviceFunction m_net_dev;
};

TEST_F(IscsiMdrBuilderTest, BuildCompleteBlob) {
    ipmi::IpmiInterface::ByteBuffer buffer;
    IscsiMdrBuilder::build(buffer, m_context, fixture::connection_wait, fixture::retry_count);
    IscsiMdrParser parser(buffer.data(), buffer.size());

    std::stringstream ss;
    ASSERT_NO_THROW(ss << parser);
}

TEST_F(IscsiMdrBuilderTest, IterateOverInitiators) {
    ipmi::IpmiInterface::ByteBuffer buffer;
    IscsiMdrBuilder::build(buffer, m_context, fixture::connection_wait, fixture::retry_count);
    IscsiMdrParser parser(buffer.data(), buffer.size());
    char ipaddr[IP_ADDRESS_SIZE];

    auto initiators = parser.get_all<ISCSI_MDR_INITIATOR>();
    ASSERT_EQ(1, initiators.size());
    const auto& init = initiators.front();
    const auto& boot = m_context.at(0).get_network_device_function().get_iscsi_boot();
    EXPECT_EQ(m_context.at(0).get_network_device_function().get_mac_address(), mdr::print_mac(init.data.nic_mac_address));
    EXPECT_EQ(boot.get_ip_mask_dns_via_dhcp(), static_cast<bool>(init.data.initiator_dhcp_enabled));
    EXPECT_EQ(boot.get_initiator_name(), init.get_string(init.data.initiator_name));
    EXPECT_EQ(IpAddressType::IPv4, init.data.ip_address_type);

    EXPECT_EQ(boot.get_initiator_address(), inet_ntop(AF_INET, init.data.initiator_ip_address, ipaddr, INET_ADDRSTRLEN))
        << "Unable to convert IP address";
    EXPECT_EQ(boot.get_initiator_netmask(), inet_ntop(AF_INET, init.data.initiator_subnet_mask, ipaddr, INET_ADDRSTRLEN))
        << "Unable to convert IP address";
    EXPECT_EQ(boot.get_initiator_default_gateway(), inet_ntop(AF_INET, init.data.initiator_gateway, ipaddr, INET_ADDRSTRLEN))
        << "Unable to convert IP address";
    EXPECT_EQ(boot.get_primary_dns(), inet_ntop(AF_INET, init.data.initiator_primary_dns, ipaddr, INET_ADDRSTRLEN))
        << "Unable to convert IP address";
    EXPECT_EQ(boot.get_secondary_dns(), inet_ntop(AF_INET, init.data.initiator_secondary_dns, ipaddr, INET_ADDRSTRLEN))
        << "Unable to convert IP address";
}

TEST_F(IscsiMdrBuilderTest, IterateOverTargets) {
    ipmi::IpmiInterface::ByteBuffer buffer;
    IscsiMdrBuilder::build(buffer, m_context, fixture::connection_wait, fixture::retry_count);
    IscsiMdrParser parser(buffer.data(), buffer.size());
    char ipaddr[IP_ADDRESS_SIZE];

    auto targets = parser.get_all<ISCSI_MDR_TARGET>();
    const auto& tgt = targets.front();
    const auto& boot = m_context.at(0).get_network_device_function().get_iscsi_boot();
    ASSERT_EQ(1, targets.size());

    EXPECT_EQ(boot.get_target_info_via_dhcp(), static_cast<bool>(tgt.data.target_dhcp_enabled));
    EXPECT_EQ(boot.get_primary_target_name(), tgt.get_string(tgt.data.target_name));
    EXPECT_EQ(IpAddressType::IPv4, tgt.data.ip_address_type);

    EXPECT_EQ(boot.get_primary_target_address(), inet_ntop(AF_INET, tgt.data.target_ip_address, ipaddr, INET_ADDRSTRLEN))
        << "Unable to convert IP address";

    EXPECT_EQ(boot.get_primary_target_port(), tgt.data.target_port);
    EXPECT_EQ(boot.get_primary_lun(), tgt.data.target_lun);
    EXPECT_EQ(boot.get_primary_vlan_enable(), static_cast<bool>(tgt.data.vlan_enabled));
    EXPECT_EQ(boot.get_primary_vlan_id(), tgt.data.vlan_id);
    EXPECT_EQ(boot.get_router_advertisement_enabled(), static_cast<bool>(tgt.data.router_advertisement));
    EXPECT_EQ(AuthenticationMethod::NO_CHAP, tgt.data.authentication_method);

    EXPECT_EQ(boot.get_chap_username(), tgt.get_string(tgt.data.chap_user_name));
    EXPECT_EQ(boot.get_chap_secret(), tgt.get_string(tgt.data.chap_secret));
    EXPECT_EQ(boot.get_mutual_chap_username(), tgt.get_string(tgt.data.mutual_chap_name));
    EXPECT_EQ(boot.get_mutual_chap_secret(), tgt.get_string(tgt.data.mutual_chap_secret));
}

TEST_F(IscsiMdrBuilderTest, IterateOverAttempts) {
    ipmi::IpmiInterface::ByteBuffer buffer;
    IscsiMdrBuilder::build(buffer, m_context, fixture::connection_wait, fixture::retry_count);
    IscsiMdrParser parser(buffer.data(), buffer.size());

    auto attempts = parser.get_all<ISCSI_MDR_ATTEMPT>();
    const auto& attempt = attempts.front();
    ASSERT_EQ(1, attempts.size());
    EXPECT_EQ(0x01, attempt.data.initiator_handle);
    EXPECT_EQ(0x02, attempt.data.target_handle);
    EXPECT_EQ(fixture::connection_wait, attempt.data.connection_wait_time);
    EXPECT_EQ(fixture::retry_count, attempt.data.connection_retry_count);
}

TEST_F(IscsiMdrBuilderTest, IterateOverVersion) {
    ipmi::IpmiInterface::ByteBuffer buffer;
    IscsiMdrBuilder::build(buffer, m_context, fixture::connection_wait, fixture::retry_count);
    IscsiMdrParser parser(buffer.data(), buffer.size());

    auto version = parser.get_all<ISCSI_MDR_VERSION>();
    ASSERT_EQ(1, version.size());
    EXPECT_EQ(0x01, version[0].data.major_version);
    EXPECT_EQ(0x00, version[0].data.minor_version);
}

// multiple entries in MDR

TEST_F(IscsiMdrBuilderTest, BuildCompleteBlobMany) {
    ipmi::IpmiInterface::ByteBuffer buffer;
    auto tmp = m_context.front();
    m_context.push_back(tmp);
    m_context.push_back(tmp);
    IscsiMdrBuilder::build(buffer, m_context, fixture::connection_wait, fixture::retry_count);
    IscsiMdrParser parser(buffer.data(), buffer.size());

    std::stringstream ss;
    ASSERT_NO_THROW(ss << parser);
}

TEST_F(IscsiMdrBuilderTest, IterateOverManyInitiators) {
    ipmi::IpmiInterface::ByteBuffer buffer;
    auto tmp = m_context.front();
    m_context.push_back(tmp);
    m_context.push_back(tmp);
    IscsiMdrBuilder::build(buffer, m_context, fixture::connection_wait, fixture::retry_count);
    IscsiMdrParser parser(buffer.data(), buffer.size());
    char ipaddr[IP_ADDRESS_SIZE];

    auto initiators = parser.get_all<ISCSI_MDR_INITIATOR>();
    const auto& boot = m_context.at(0).get_network_device_function().get_iscsi_boot();
    ASSERT_EQ(3, initiators.size());
    for (const auto& init : initiators) {
        EXPECT_EQ(m_context.at(0).get_network_device_function().get_mac_address(), mdr::print_mac(init.data.nic_mac_address));
        EXPECT_EQ(boot.get_ip_mask_dns_via_dhcp(), static_cast<bool>(init.data.initiator_dhcp_enabled));
        EXPECT_EQ(boot.get_initiator_name(), init.get_string(init.data.initiator_name));
        EXPECT_EQ(IpAddressType::IPv4, init.data.ip_address_type);

        EXPECT_EQ(boot.get_initiator_address(), inet_ntop(AF_INET, init.data.initiator_ip_address, ipaddr, INET_ADDRSTRLEN))
            << "Unable to convert IP address";
        EXPECT_EQ(boot.get_initiator_netmask(), inet_ntop(AF_INET, init.data.initiator_subnet_mask, ipaddr, INET_ADDRSTRLEN))
            << "Unable to convert IP address";
        EXPECT_EQ(boot.get_initiator_default_gateway(), inet_ntop(AF_INET, init.data.initiator_gateway, ipaddr, INET_ADDRSTRLEN))
            << "Unable to convert IP address";
        EXPECT_EQ(boot.get_primary_dns(), inet_ntop(AF_INET, init.data.initiator_primary_dns, ipaddr, INET_ADDRSTRLEN))
            << "Unable to convert IP address";
        EXPECT_EQ(boot.get_secondary_dns(), inet_ntop(AF_INET, init.data.initiator_secondary_dns, ipaddr, INET_ADDRSTRLEN))
            << "Unable to convert IP address";
    }
}

TEST_F(IscsiMdrBuilderTest, IterateOverManyTargets) {
    ipmi::IpmiInterface::ByteBuffer buffer;
    auto tmp = m_context.front();
    m_context.push_back(tmp);
    m_context.push_back(tmp);
    IscsiMdrBuilder::build(buffer, m_context, fixture::connection_wait, fixture::retry_count);
    IscsiMdrParser parser(buffer.data(), buffer.size());
    char ipaddr[IP_ADDRESS_SIZE];

    auto targets = parser.get_all<ISCSI_MDR_TARGET>();
    const auto& boot = m_context.at(0).get_network_device_function().get_iscsi_boot();
    ASSERT_EQ(3, targets.size());

    for (const auto& tgt : targets) {
        EXPECT_EQ(boot.get_target_info_via_dhcp(), static_cast<bool>(tgt.data.target_dhcp_enabled));
        EXPECT_EQ(boot.get_primary_target_name(), tgt.get_string(tgt.data.target_name));
        EXPECT_EQ(IpAddressType::IPv4, tgt.data.ip_address_type);

        EXPECT_EQ(boot.get_primary_target_address(), inet_ntop(AF_INET, tgt.data.target_ip_address, ipaddr, INET_ADDRSTRLEN))
            << "Unable to convert IP address";

        EXPECT_EQ(boot.get_primary_target_port(), tgt.data.target_port);
        EXPECT_EQ(boot.get_primary_lun(), tgt.data.target_lun);
        EXPECT_EQ(boot.get_primary_vlan_enable(), static_cast<bool>(tgt.data.vlan_enabled));
        EXPECT_EQ(boot.get_primary_vlan_id(), tgt.data.vlan_id);
        EXPECT_EQ(boot.get_router_advertisement_enabled(), static_cast<bool>(tgt.data.router_advertisement));
        EXPECT_EQ(AuthenticationMethod::NO_CHAP, tgt.data.authentication_method);

        EXPECT_EQ(boot.get_chap_username(), tgt.get_string(tgt.data.chap_user_name));
        EXPECT_EQ(boot.get_chap_secret(), tgt.get_string(tgt.data.chap_secret));
        EXPECT_EQ(boot.get_mutual_chap_username(), tgt.get_string(tgt.data.mutual_chap_name));
        EXPECT_EQ(boot.get_mutual_chap_secret(), tgt.get_string(tgt.data.mutual_chap_secret));
    }
}

TEST_F(IscsiMdrBuilderTest, IterateOverManyAttempts) {
    ipmi::IpmiInterface::ByteBuffer buffer;
    auto tmp = m_context.front();
    m_context.push_back(tmp);
    m_context.push_back(tmp);
    IscsiMdrBuilder::build(buffer, m_context, fixture::connection_wait, fixture::retry_count);
    IscsiMdrParser parser(buffer.data(), buffer.size());

    auto attempts = parser.get_all<ISCSI_MDR_ATTEMPT>();
    ASSERT_EQ(3, attempts.size());
    for (const auto& attempt : attempts) {
        EXPECT_EQ(attempt.data.initiator_handle + attempts.size(), attempt.data.target_handle);
        EXPECT_EQ(fixture::connection_wait, attempt.data.connection_wait_time);
        EXPECT_EQ(fixture::retry_count, attempt.data.connection_retry_count);
    }
}
