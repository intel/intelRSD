/*!
 * @brief Implementation of EndpointReader tests.
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
 * @file endpoint_reader_test.cpp
 */

#include "gmock/gmock.h"

#include "tools/endpoint_reader.hpp"
#include "nvme_agent_context.hpp"
#include "mocks/mock_nvme_target_handler.hpp"
#include "mocks/mock_database_factory.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/model/model_storage.hpp"

#include <map>
#include <ostream>

using namespace agent::nvme::tools;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace std;
using namespace testing;

static const string DRIVE_PATH{"fake_disk"};
static const string VOLUME_PATH{"fake_partition"};
static const string IPV4_ADDRESS{"10.0.3.4"};
static const uint16_t RDMA_PORT{1234};
static const nvme_target::IdList PORTS{"1", "2", "3", "4", "5"};
static const nvme_target::IdList NAMESPACES{"10"};
static const nvme_target::PortParams PORT_4_PARAMS{IPV4_ADDRESS, RDMA_PORT, "ipv4", "other"};
static const nvme_target::PortParams PORT_5_PARAMS{IPV4_ADDRESS, RDMA_PORT, "ipv4", "rdma"};
static const nvme_target::PortParams DEFAULT_PORT_PARAMS{"0.0.0.0", 0, "ipv4", "rdma"};

class UTDatabase final : public Database {
public:
    UTDatabase(const Uuid&, const string&) {}

    void put(const string& key, const string& value) override {
        m_map[key] = value;
    }

    string get(const string& key) const override {
        return m_map.at(key);
    }

    void del(const string& key) override {
        m_map.erase(key);
    }

    void append(const string&, const string&) override {
        throw runtime_error("Not implemented");
    }

    vector<string> get_multiple_values(const string&) const override {
        throw runtime_error("Not implemented");
    }

    void remove() override {
        throw runtime_error("Not implemented");
    }

private:
    static map<string, string> m_map;
};

map<string, string> UTDatabase::m_map{};

class EndpointReaderTest: public ::testing::Test {
protected:
    EndpointReader er{};
    std::shared_ptr<agent::nvme::NvmeAgentContext> ctx{};
    std::shared_ptr<MockDatabaseFactory> db_factory_mock{};
    std::shared_ptr<MockNvmeTargetHandler> target_handler_mock{};
    Fabric fabric{};
    std::shared_ptr<UTDatabase> database;
public:

    void SetUp() override {
        Drive d1{}, d2{};
        Volume v1{};
        // define systemPath identifier for only one drive
        d2.set_name(DRIVE_PATH);
        get_manager<Drive>().add_entry(d1);
        get_manager<Drive>().add_entry(d2);
        v1.set_name(VOLUME_PATH);
        get_manager<Volume>().add_entry(v1);

        // add NetworkInterface
        static auto called_once = []() {
            NetworkInterface ni{};
            ni.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
            attribute::Ipv4Address ipv4_address{};
            ipv4_address.set_address(IPV4_ADDRESS);
            ni.add_ipv4_address(ipv4_address);
            get_manager<NetworkInterface>().add_entry(ni);
            return 0;
        }();
        (void)called_once;

        database = std::make_shared<UTDatabase>("", "");
        ctx = std::make_shared<agent::nvme::NvmeAgentContext>();

        db_factory_mock = std::make_shared<MockDatabaseFactory>();
        ctx->db_factory = db_factory_mock;

        target_handler_mock = std::make_shared<MockNvmeTargetHandler>();
        ctx->nvme_target_handler = target_handler_mock;

        EXPECT_CALL(*db_factory_mock, create_database(_, _)).WillOnce(Return(database)).RetiresOnSaturation();

        // initialize database
        auto db = ctx->db_factory->create_database({}, {});
        db->put("nvme_port", "0");
    }

    virtual ~EndpointReaderTest();

    void set_expectations() {
        // return a list of ports
        EXPECT_CALL(*target_handler_mock, get_ports()).WillOnce(Return(PORTS));
        // return invalid parameters for all ports but 4 and 5
        EXPECT_CALL(*target_handler_mock, get_port_params(MatchesRegex("1|2|3"))).Times(3).WillRepeatedly(Return(DEFAULT_PORT_PARAMS));
        // return non-RDMA transport type for port 4
        EXPECT_CALL(*target_handler_mock, get_port_params(StrEq("4"))).WillOnce(Return(PORT_4_PARAMS));
        // return valid parameters only for port 5
        EXPECT_CALL(*target_handler_mock, get_port_params(StrEq("5"))).WillOnce(Return(PORT_5_PARAMS));
        // do not return subsystems only for port 1
        EXPECT_CALL(*target_handler_mock, get_port_subsystems(StrEq("1"))).WillOnce(Return(nvme_target::IdList()));
        EXPECT_CALL(*target_handler_mock, get_port_subsystems(StrEq("2"))).WillOnce(Return(nvme_target::IdList({"s2"})));
        EXPECT_CALL(*target_handler_mock, get_port_subsystems(StrEq("3"))).WillOnce(Return(nvme_target::IdList({"s3"})));
        EXPECT_CALL(*target_handler_mock, get_port_subsystems(StrEq("4"))).WillOnce(Return(nvme_target::IdList({"s4"})));
        EXPECT_CALL(*target_handler_mock, get_port_subsystems(StrEq("5"))).WillOnce(Return(nvme_target::IdList({"s5"})));
        // return no namespaces for port 2
        EXPECT_CALL(*target_handler_mock, get_subsystem_namespaces(MatchesRegex("s2"))).WillOnce(Return(nvme_target::IdList()));
        // for all other subsystems return one namespace "10"
        EXPECT_CALL(*target_handler_mock, get_subsystem_namespaces(MatchesRegex("s[3-5]?"))).Times(3).WillRepeatedly(Return(NAMESPACES));
        // no device_path for subsystem s3
        EXPECT_CALL(*target_handler_mock, get_subsystem_namespace_device(StrEq("s3"), StrEq("10"))).WillOnce(Return(""));
        // for subsystem s4 return Drive path
        EXPECT_CALL(*target_handler_mock, get_subsystem_namespace_device(StrEq("s4"), StrEq("10"))).WillOnce(Return(DRIVE_PATH));
        // for subsystem s5 return Volume path
        EXPECT_CALL(*target_handler_mock, get_subsystem_namespace_device(StrEq("s5"), StrEq("10"))).WillOnce(Return(VOLUME_PATH));
        // return the database instance
        EXPECT_CALL(*db_factory_mock, create_database(_, _)).WillOnce(Return(database));
    }

    void add_endpoint() {
        set_expectations();
        database->put("nvme_port", "5");
    }
};

EndpointReaderTest::~EndpointReaderTest() { }

TEST_F(EndpointReaderTest, TestNoEndpoints) {
    set_expectations();

    auto endpoints = er.read_endpoints(ctx, fabric.get_uuid());
    // we expect no endpoint discovered
    ASSERT_EQ(endpoints.size(), 0);
}

TEST_F(EndpointReaderTest, TestOneEndpoint) {
    set_expectations();
    // update database with a valid port numer
    database->put("nvme_port", "5");
    auto endpoints = er.read_endpoints(ctx, fabric.get_uuid());
    // we expect now one endpoint discovered
    ASSERT_EQ(endpoints.size(), 1);

}

TEST_F(EndpointReaderTest, TestEndpointOneConnectedEntity) {
    add_endpoint();
    auto endpoint = er.read_endpoints(ctx, fabric.get_uuid()).front();
    auto ce = endpoint.get_connected_entities();
    ASSERT_EQ(ce.size(), 1);
}

TEST_F(EndpointReaderTest, TestEndpointConnectedEntityRoleTarget) {
    add_endpoint();
    auto endpoint = er.read_endpoints(ctx, fabric.get_uuid()).front();
    const auto& ce = endpoint.get_connected_entities()[0];
    ASSERT_EQ(ce.get_entity_role(), enums::EntityRole::Target);
}

TEST_F(EndpointReaderTest, TestEndpointOneTransport) {
    add_endpoint();
    auto endpoint = er.read_endpoints(ctx, fabric.get_uuid()).front();
    ASSERT_EQ(endpoint.get_ip_transport_details().size(), 1);
}

TEST_F(EndpointReaderTest, TestEndpointTransportProtocol) {
    add_endpoint();
    auto endpoint = er.read_endpoints(ctx, fabric.get_uuid()).front();
    const auto& itd = endpoint.get_ip_transport_details()[0];
    ASSERT_EQ(itd.get_transport_protocol(), enums::TransportProtocol::RoCEv2);
}

TEST_F(EndpointReaderTest, TestEndpointTransportDetailFamily) {
    add_endpoint();
    auto endpoint = er.read_endpoints(ctx, fabric.get_uuid()).front();
    const auto& itd = endpoint.get_ip_transport_details()[0];
    const auto& a = itd.get_ipv4_address();
    ASSERT_TRUE(a.get_address().has_value());
}

TEST_F(EndpointReaderTest, TestEndpointTransportDetailAddress) {
    add_endpoint();
    auto endpoint = er.read_endpoints(ctx, fabric.get_uuid()).front();
    const auto& itd = endpoint.get_ip_transport_details()[0];
    const auto& a = itd.get_ipv4_address();
    ASSERT_EQ(a.get_address().value(), IPV4_ADDRESS);
}

TEST_F(EndpointReaderTest, TestEndpointTransportDetailPort) {
    add_endpoint();
    auto endpoint = er.read_endpoints(ctx, fabric.get_uuid()).front();
    const auto& itd = endpoint.get_ip_transport_details()[0];
    ASSERT_EQ(itd.get_port(), RDMA_PORT);
}

TEST_F(EndpointReaderTest, TestEndpointProtocol) {
    add_endpoint();
    auto endpoint = er.read_endpoints(ctx, fabric.get_uuid()).front();
    ASSERT_EQ(endpoint.get_protocol(), enums::TransportProtocol::NVMeOverFabrics);
}

TEST_F(EndpointReaderTest, TestEndpointIdentifiers) {
    add_endpoint();
    auto endpoint = er.read_endpoints(ctx, fabric.get_uuid()).front();
    ASSERT_EQ(endpoint.get_identifiers().size(), 2);
}

TEST_F(EndpointReaderTest, TestEndpointIdentifierNqn) {
    add_endpoint();
    auto endpoint = er.read_endpoints(ctx, fabric.get_uuid()).front();
    const auto& i = endpoint.get_identifiers()[0];
    ASSERT_EQ(i.get_durable_name_format(), enums::IdentifierType::NQN);
    ASSERT_EQ(i.get_durable_name(), "nqn.2014-08.org.nvmexpress:uuid:s5");
}

TEST_F(EndpointReaderTest, TestEndpointIdentifierUuid) {
    add_endpoint();
    auto endpoint = er.read_endpoints(ctx, fabric.get_uuid()).front();
    const auto& i = endpoint.get_identifiers()[1];
    ASSERT_EQ(i.get_durable_name_format(), enums::IdentifierType::UUID);
    ASSERT_EQ(i.get_durable_name(), endpoint.get_uuid());
}
