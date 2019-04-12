/*!
 * @brief Implementation of EndpointCreator tests.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file endpoint_creator_test.cpp
 */

#include "gmock/gmock.h"

#include "tools/endpoint_creator.hpp"
#include "nvme_agent_context.hpp"
#include "mocks/mock_nvme_target_handler.hpp"
#include "mocks/mock_database_factory.hpp"
#include "mocks/mock_transaction_handler_factory.hpp"
#include "mocks/mock_transaction_handler.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/model/model_storage.hpp"
#include "utils/transaction/default_transaction_handler.hpp"

#include <map>
#include <ostream>
#include <errno.h>



using namespace agent::nvme::tools;
using namespace utils::transaction;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace std;
using namespace testing;

namespace {

const string DRIVE_PATH{"fake_disk"};
const string VOLUME_PATH{"fake_partition"};
const string IPV4_ADDRESS{"10.0.3.4"};
const string ENDPOINT_NQN{"subsystem_name"};
const string DEVICE_PATH{"/dev/path"};
const string GOOD_IP{"192.168.0.1"};
const string BAD_IP_1{"192.168.1.1"};
const string BAD_IP_2{"192.168.2.1"};
const uint16_t GOOD_PORT{3260};
const uint16_t BAD_PORT_1{1234};
const uint16_t BAD_PORT_2{1235};


void set_errno_exists() {
    errno = EEXIST;
}

}

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

class EndpointCreatorTest : public ::testing::Test {
protected:
    EndpointCreator ec{};
    std::shared_ptr<agent::nvme::NvmeAgentContext> ctx{};
    std::shared_ptr<MockDatabaseFactory> db_factory_mock{};
    std::shared_ptr<MockTransactionHandlerFactory> transaction_factory_mock{};
    std::shared_ptr<MockTransactionHandler> transaction_mock{};
    std::shared_ptr<MockNvmeTargetHandler> target_handler_mock{};

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
        (void) called_once;

        database = std::make_shared<UTDatabase>("", "");
        ctx = std::make_shared<agent::nvme::NvmeAgentContext>();

        db_factory_mock = std::make_shared<MockDatabaseFactory>();
        ctx->db_factory = db_factory_mock;

        target_handler_mock = std::make_shared<MockNvmeTargetHandler>();
        ctx->nvme_target_handler = target_handler_mock;

        transaction_factory_mock = std::make_shared<MockTransactionHandlerFactory>();
        ctx->transaction_handler_factory = transaction_factory_mock;

        transaction_mock = std::make_shared<MockTransactionHandler>();

        EXPECT_CALL(*db_factory_mock, create_database(_, _)).WillOnce(Return(database)).RetiresOnSaturation();

        // initialize database
        auto db = ctx->db_factory->create_database({}, {});
        db->put("nvme_port", "0");
    }


    virtual ~EndpointCreatorTest();

};


EndpointCreatorTest::~EndpointCreatorTest() {}


TEST_F(EndpointCreatorTest, CreateSingleEndpoint) {
    // set the default actions for the implementation of the transaction handler
    DefaultTransactionHandler handler;
    ON_CALL(*transaction_mock, add_handler_pair(_)).WillByDefault(
        Invoke(&handler, &DefaultTransactionHandler::add_handler_pair));
    ON_CALL(*transaction_mock, commit()).WillByDefault(Invoke(&handler, &DefaultTransactionHandler::commit));

    EXPECT_CALL(*transaction_factory_mock, get_handler()).WillOnce(Return(transaction_mock));
    EXPECT_CALL(*transaction_mock, add_handler_pair(_)).Times(4);
    EXPECT_CALL(*transaction_mock, commit());
    EXPECT_CALL(*target_handler_mock, add_subsystem(StrEq(ENDPOINT_NQN)));
    EXPECT_CALL(*target_handler_mock, set_subsystem_allow_all_hosts(StrEq(ENDPOINT_NQN), false));

    EXPECT_CALL(*target_handler_mock, add_subsystem_namespace(StrEq(ENDPOINT_NQN), StrEq("1")));
    EXPECT_CALL(*target_handler_mock,
                set_subsystem_namespace_device(StrEq(ENDPOINT_NQN), StrEq("1"), StrEq(DEVICE_PATH)));
    EXPECT_CALL(*target_handler_mock, set_subsystem_namespace_enable(StrEq(ENDPOINT_NQN), StrEq("1"), false));

    EXPECT_CALL(*target_handler_mock, get_ports()).WillOnce(Return(nvme_target::IdList()));
    EXPECT_CALL(*target_handler_mock, add_port(StrEq("1")));
    EXPECT_CALL(*target_handler_mock, set_port_params("1", _));

    EXPECT_CALL(*target_handler_mock, add_port_subsystem(StrEq("1"), StrEq(ENDPOINT_NQN)));

    ec.create_target_endpoint(ctx, GOOD_IP, GOOD_PORT, ENDPOINT_NQN, DEVICE_PATH);
}


TEST_F(EndpointCreatorTest, ExistingPortsNoMatch) {
    // set the default actions for the implementation of the transaction handler
    DefaultTransactionHandler handler;
    ON_CALL(*transaction_mock, add_handler_pair(_)).WillByDefault(
        Invoke(&handler, &DefaultTransactionHandler::add_handler_pair));
    ON_CALL(*transaction_mock, commit()).WillByDefault(Invoke(&handler, &DefaultTransactionHandler::commit));

    EXPECT_CALL(*transaction_factory_mock, get_handler()).WillOnce(Return(transaction_mock));
    EXPECT_CALL(*transaction_mock, add_handler_pair(_)).Times(4);
    EXPECT_CALL(*transaction_mock, commit());
    EXPECT_CALL(*target_handler_mock, add_subsystem(StrEq(ENDPOINT_NQN)));
    EXPECT_CALL(*target_handler_mock, set_subsystem_allow_all_hosts(StrEq(ENDPOINT_NQN), false));

    EXPECT_CALL(*target_handler_mock, add_subsystem_namespace(StrEq(ENDPOINT_NQN), StrEq("1")));
    EXPECT_CALL(*target_handler_mock,
                set_subsystem_namespace_device(StrEq(ENDPOINT_NQN), StrEq("1"), StrEq(DEVICE_PATH)));
    EXPECT_CALL(*target_handler_mock, set_subsystem_namespace_enable(StrEq(ENDPOINT_NQN), StrEq("1"), false));

    EXPECT_CALL(*target_handler_mock, get_ports()).WillOnce(Return(nvme_target::IdList{"1", "2"}));
    EXPECT_CALL(*target_handler_mock, get_port_params(StrEq("1"))).WillOnce(
        Return(std::make_tuple(BAD_IP_1, BAD_PORT_1, "", "")));
    EXPECT_CALL(*target_handler_mock, get_port_params(StrEq("2"))).WillOnce(
        Return(std::make_tuple(BAD_IP_2, BAD_PORT_2, "", "")));
    EXPECT_CALL(*target_handler_mock, add_port(StrEq("1"))).WillOnce(
        DoAll(InvokeWithoutArgs(set_errno_exists), Throw(std::runtime_error(""))));
    EXPECT_CALL(*target_handler_mock, add_port(StrEq("2"))).WillOnce(
        DoAll(InvokeWithoutArgs(set_errno_exists), Throw(std::runtime_error(""))));
    EXPECT_CALL(*target_handler_mock, add_port(StrEq("3")));
    EXPECT_CALL(*target_handler_mock, set_port_params("3", _));

    EXPECT_CALL(*target_handler_mock, add_port_subsystem(StrEq("3"), StrEq(ENDPOINT_NQN)));

    ec.create_target_endpoint(ctx, GOOD_IP, GOOD_PORT, ENDPOINT_NQN, DEVICE_PATH);
}


TEST_F(EndpointCreatorTest, ExistingPortsMatch) {
    // set the default actions for the implementation of the transaction handler
    DefaultTransactionHandler handler;
    ON_CALL(*transaction_mock, add_handler_pair(_)).WillByDefault(
        Invoke(&handler, &DefaultTransactionHandler::add_handler_pair));
    ON_CALL(*transaction_mock, commit()).WillByDefault(Invoke(&handler, &DefaultTransactionHandler::commit));

    EXPECT_CALL(*transaction_factory_mock, get_handler()).WillOnce(Return(transaction_mock));
    EXPECT_CALL(*transaction_mock, add_handler_pair(_)).Times(4);
    EXPECT_CALL(*transaction_mock, commit());
    EXPECT_CALL(*target_handler_mock, add_subsystem(StrEq(ENDPOINT_NQN)));
    EXPECT_CALL(*target_handler_mock, set_subsystem_allow_all_hosts(StrEq(ENDPOINT_NQN), false));

    EXPECT_CALL(*target_handler_mock, add_subsystem_namespace(StrEq(ENDPOINT_NQN), StrEq("1")));
    EXPECT_CALL(*target_handler_mock,
                set_subsystem_namespace_device(StrEq(ENDPOINT_NQN), StrEq("1"), StrEq(DEVICE_PATH)));
    EXPECT_CALL(*target_handler_mock, set_subsystem_namespace_enable(StrEq(ENDPOINT_NQN), StrEq("1"), false));

    EXPECT_CALL(*target_handler_mock, get_ports()).WillOnce(Return(nvme_target::IdList{"1", "2"}));
    EXPECT_CALL(*target_handler_mock, get_port_params(StrEq("1"))).WillOnce(
        Return(std::make_tuple(BAD_IP_1, BAD_PORT_1, "", "")));
    EXPECT_CALL(*target_handler_mock, get_port_params(StrEq("2"))).WillOnce(
        Return(std::make_tuple(GOOD_IP, GOOD_PORT, "", "")));

    EXPECT_CALL(*target_handler_mock, add_port_subsystem(StrEq("2"), StrEq(ENDPOINT_NQN)));

    ec.create_target_endpoint(ctx, GOOD_IP, GOOD_PORT, ENDPOINT_NQN, DEVICE_PATH);
}


TEST_F(EndpointCreatorTest, NoMorePortsAvailable) {
    // set the default actions for the implementation of the transaction handler
    DefaultTransactionHandler handler;
    ON_CALL(*transaction_mock, add_handler_pair(_)).WillByDefault(
        Invoke(&handler, &DefaultTransactionHandler::add_handler_pair));
    ON_CALL(*transaction_mock, commit()).WillByDefault(Invoke(&handler, &DefaultTransactionHandler::commit));

    EXPECT_CALL(*transaction_factory_mock, get_handler()).WillOnce(Return(transaction_mock));
    EXPECT_CALL(*transaction_mock, add_handler_pair(_)).Times(4);
    EXPECT_CALL(*transaction_mock, commit());
    EXPECT_CALL(*transaction_mock, rollback());
    EXPECT_CALL(*target_handler_mock, add_subsystem(StrEq(ENDPOINT_NQN)));
    EXPECT_CALL(*target_handler_mock, set_subsystem_allow_all_hosts(StrEq(ENDPOINT_NQN), false));

    EXPECT_CALL(*target_handler_mock, add_subsystem_namespace(StrEq(ENDPOINT_NQN), StrEq("1")));
    EXPECT_CALL(*target_handler_mock,
                set_subsystem_namespace_device(StrEq(ENDPOINT_NQN), StrEq("1"), StrEq(DEVICE_PATH)));
    EXPECT_CALL(*target_handler_mock, set_subsystem_namespace_enable(StrEq(ENDPOINT_NQN), StrEq("1"), false));

    EXPECT_CALL(*target_handler_mock, get_ports()).WillOnce(Return(nvme_target::IdList()));
    EXPECT_CALL(*target_handler_mock, add_port(_)).WillRepeatedly(
        DoAll(InvokeWithoutArgs(set_errno_exists), Throw(std::runtime_error(""))));

    EXPECT_THROW(ec.create_target_endpoint(ctx, GOOD_IP, GOOD_PORT, ENDPOINT_NQN, DEVICE_PATH), std::runtime_error);
}

