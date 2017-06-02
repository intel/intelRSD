/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "generic_handler_test.cpp"
#include "fabric_resources.hpp"

namespace psme {
namespace rest {
namespace model {
namespace handler {

using SubscriptionManager = psme::rest::eventing::manager::SubscriptionManager;
using namespace psme::rest::eventing;
using namespace agent_framework::model;
using namespace agent_framework::module;
using agent_framework::eventing::Notification;

// Required by gtest scheme for fixture tests
class FabricHandlersTest : public ::testing::Test {
public:
    FabricHandlersTest() { }

    ~FabricHandlersTest();

    void SetUp() {
        DatabaseTester::drop_all();

        auto agent = psme::core::agent::AgentManager::get_instance().get_agent("anything");
        agent->m_responses = {
            FabricManager1,
            // Fabrics collection
            R"([
                                    {"subcomponent": "fabric_1_uuid"}
                                ])",
            Fabric1,
            // Zones on Fabric1 collection
            R"([
                                    {"subcomponent": "zone_1_uuid"},
                                    {"subcomponent": "zone_2_uuid"}]
                                )",
            FabricZone1,
            // Endpoints on Zone1 collection
            R"([{
                                    "subcomponent": "zone_1_endpoint_1_uuid"
                                }])",
            FabricZone2,
            // Endpoints on Zone2 collection
            R"([
                                    {"subcomponent": "zone_2_endpoint_1_uuid"},
                                    {"subcomponent": "zone_2_endpoint_2_uuid"}
                                ])",
            // Endpoints on Fabric1 collection
            R"([
                                    {"subcomponent": "zone_1_endpoint_1_uuid"},
                                    {"subcomponent": "zone_2_endpoint_1_uuid"},
                                    {"subcomponent": "zone_2_endpoint_2_uuid"}
                                ])",
            Endpoint1InZone1,
            // Ports in Endpoint1inZone1
            R"([])",
            Endpoint1InZone2,
            // Ports in Endpoint1inZone2
            R"([])",
            Endpoint2InZone2,
            // Ports in Endpoint2inZone2
            R"([])",
            // Switches on Fabric1 collection
            R"([])"
        };

        EventData event;

        event.set_type("Manager");
        event.set_notification(Notification::Add);
        event.set_parent("");
        event.set_component("manager_1_uuid");
        auto handler = psme::rest::model::handler::HandlerManager::get_instance()->get_handler(event.get_type());
        handler->handle(agent, event);
    }

    void reset_id_policy() {
        static_cast<psme::rest::model::handler::ManagerHandler *>(psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
            enums::Component::Manager))->m_id_policy.reset();
        static_cast<psme::rest::model::handler::FabricHandler *>(psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
            enums::Component::Fabric))->m_id_policy.reset();
        static_cast<psme::rest::model::handler::ZoneHandler *>(psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
            enums::Component::Zone))->m_id_policy.reset();
        static_cast<psme::rest::model::handler::EndpointHandler *>(psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
            enums::Component::Endpoint))->m_id_policy.reset();
    }

    void TearDown() {
        auto agent = psme::core::agent::AgentManager::get_instance().get_agent("anything");
        agent->clear();
        agent_framework::module::get_manager<agent_framework::model::Manager>().clear_entries();
        agent_framework::module::get_manager<agent_framework::model::Fabric>().clear_entries();
        agent_framework::module::get_manager<agent_framework::model::Zone>().clear_entries();
        agent_framework::module::get_manager<agent_framework::model::Endpoint>().clear_entries();

        reset_id_policy();

        SubscriptionManager::get_instance()->clear();
    }

};

FabricHandlersTest::~FabricHandlersTest() { }

using namespace testing;
using namespace psme::rest::eventing;

TEST_F(FabricHandlersTest, AddEverything) {
    auto agent = psme::core::agent::AgentManager::get_instance().get_agent("anything");
    auto expectedReq = std::vector<std::string> {"manager_1_uuid",
                                                 "manager_1_uuid",
                                                 "fabric_1_uuid",
                                                 "fabric_1_uuid",
                                                 "zone_1_uuid",
                                                 "zone_1_uuid",
                                                 "zone_2_uuid",
                                                 "zone_2_uuid",
                                                 "fabric_1_uuid",
                                                 "zone_1_endpoint_1_uuid",
                                                 "zone_1_endpoint_1_uuid",
                                                 "zone_2_endpoint_1_uuid",
                                                 "zone_2_endpoint_1_uuid",
                                                 "zone_2_endpoint_2_uuid",
                                                 "zone_2_endpoint_2_uuid",
                                                 "fabric_1_uuid"
    };

    CHECK_REQUESTS;

    auto expectedEvents = std::vector<std::pair<EventType, std::string>> {{EventType::ResourceAdded, "/redfish/v1/Managers/1"}};
    CHECK_EVENTS;


    EXPECT_EQ("fabric_1_uuid", psme::rest::model::Find<agent_framework::model::Fabric>("1").get_uuid());
    EXPECT_EQ("zone_1_uuid", psme::rest::model::Find<agent_framework::model::Zone>("1").via<agent_framework::model::Fabric>("1").get_uuid());
    EXPECT_EQ("zone_2_uuid", psme::rest::model::Find<agent_framework::model::Zone>("2").via<agent_framework::model::Fabric>("1").get_uuid());
    EXPECT_EQ("zone_1_endpoint_1_uuid", psme::rest::model::Find<agent_framework::model::Endpoint>("1").via<agent_framework::model::Fabric>("1").get_uuid());
    EXPECT_EQ("zone_2_endpoint_1_uuid", psme::rest::model::Find<agent_framework::model::Endpoint>("2").via<agent_framework::model::Fabric>("1").get_uuid());
    EXPECT_EQ("zone_2_endpoint_2_uuid", psme::rest::model::Find<agent_framework::model::Endpoint>("3").via<agent_framework::model::Fabric>("1").get_uuid());


    auto& zone_endpoint_manager = agent_framework::module::PncComponents::get_instance()->get_zone_endpoint_manager();
    auto zone_1_endpoint_children = zone_endpoint_manager.get_children("zone_1_uuid");
    EXPECT_EQ(zone_1_endpoint_children.size(), 1);
    EXPECT_EQ(zone_1_endpoint_children.front(), "zone_1_endpoint_1_uuid");

    auto zone_1_endpoint_1_parents = zone_endpoint_manager.get_parents("zone_1_endpoint_1_uuid");
    EXPECT_EQ(zone_1_endpoint_1_parents.size(), 1);
    EXPECT_EQ(zone_1_endpoint_1_parents.front(), "zone_1_uuid");

    auto zone_2_port_children = zone_endpoint_manager.get_children("zone_2_uuid");
    EXPECT_EQ(zone_2_port_children.size(), 2);
    EXPECT_EQ(zone_2_port_children.front(), "zone_2_endpoint_1_uuid");
    EXPECT_EQ(zone_2_port_children.back(), "zone_2_endpoint_2_uuid");
}

TEST_F(FabricHandlersTest, RemoveZone) {
    // Clear the Add event from Test SetUp
    SubscriptionManager::get_instance()->clear();

    EventData event;
    event.set_type("Zone");
    event.set_notification(Notification::Remove);
    event.set_parent("fabric_1_uuid");
    event.set_component("zone_1_uuid");

    auto agent = psme::core::agent::AgentManager::get_instance().get_agent("anything");
    auto handler = psme::rest::model::handler::HandlerManager::get_instance()->get_handler(event.get_type());
    handler->handle(agent, event);

    EXPECT_THROW(psme::rest::model::Find<agent_framework::model::Zone>("1").via<agent_framework::model::Fabric>("1").get_one(), agent_framework::exceptions::NotFound);
    EXPECT_EQ("zone_2_uuid", psme::rest::model::Find<agent_framework::model::Zone>("2").via<agent_framework::model::Fabric>("1").get_uuid());

    auto& zone_endpoint_manager = agent_framework::module::PncComponents::get_instance()->get_zone_endpoint_manager();
    auto zone_1_port_children = zone_endpoint_manager.get_children("zone_1_uuid");
    EXPECT_EQ(zone_1_port_children.size(), 0);

    auto zone_1_endpoint_1_parents = zone_endpoint_manager.get_parents("zone_1_endpoint_1_uuid");
    EXPECT_EQ(zone_1_endpoint_1_parents.size(), 0);


    auto expectedEvents = std::vector<std::pair<EventType, std::string>> {{EventType::ResourceRemoved, "/redfish/v1/Fabrics/1/Zones/1"}};
    CHECK_EVENTS;
}

TEST_F(FabricHandlersTest, RemoveEndpoint) {
    // Clear the Add event from Test SetUp
    SubscriptionManager::get_instance()->clear();

    EventData event;
    event.set_type("Endpoint");
    event.set_notification(Notification::Remove);
    event.set_parent("fabric_1_uuid");
    event.set_component("zone_2_endpoint_1_uuid");

    auto agent = psme::core::agent::AgentManager::get_instance().get_agent("anything");
    auto handler = psme::rest::model::handler::HandlerManager::get_instance()->get_handler(event.get_type());
    handler->handle(agent, event);

    EXPECT_THROW(psme::rest::model::Find<agent_framework::model::Endpoint>("2").via<agent_framework::model::Fabric>("1").get_one(), agent_framework::exceptions::NotFound);
    EXPECT_EQ("zone_1_endpoint_1_uuid", psme::rest::model::Find<agent_framework::model::Endpoint>("1").via<agent_framework::model::Fabric>("1").get_uuid());
    EXPECT_EQ("zone_2_endpoint_2_uuid", psme::rest::model::Find<agent_framework::model::Endpoint>("3").via<agent_framework::model::Fabric>("1").get_uuid());

    auto& zone_endpoint_manager = agent_framework::module::PncComponents::get_instance()->get_zone_endpoint_manager();
    auto zone_2_endpoints = zone_endpoint_manager.get_children("zone_2_uuid");
    EXPECT_EQ(zone_2_endpoints.size(), 1);
    EXPECT_EQ(zone_2_endpoints.front(), "zone_2_endpoint_2_uuid");

    auto zone_2_endpoint_1_parents = zone_endpoint_manager.get_parents("zone_2_endpoint_1_uuid");
    EXPECT_EQ(zone_2_endpoint_1_parents.size(), 0);

    auto zone_2_endpoint_2_parents = zone_endpoint_manager.get_parents("zone_2_endpoint_2_uuid");
    EXPECT_EQ(zone_2_endpoint_2_parents.size(), 1);
    EXPECT_EQ(zone_2_endpoint_2_parents.front(), "zone_2_uuid");

    auto expectedEvents = std::vector<std::pair<EventType, std::string>> {{EventType::ResourceRemoved, "/redfish/v1/Fabrics/1/Endpoints/2"}};
    CHECK_EVENTS;
}

TEST_F(FabricHandlersTest, LoadAfterAddingEndpointToFabricZone) {
    // Clear the Add event from Test SetUp
    SubscriptionManager::get_instance()->clear();

    // assumption: an AddZoneEndpoint reqest was sent, adding zone_1_endpoint_1 to zone_2

    auto agent = psme::core::agent::AgentManager::get_instance().get_agent("anything");
    agent->clear();
    agent-> m_responses = {
        FabricZone2,
        // Functions on Zone2 collection
        R"([
                                    {"subcomponent": "zone_2_endpoint_1_uuid"},
                                    {"subcomponent": "zone_2_endpoint_2_uuid"},
                                    {"subcomponent": "zone_1_endpoint_1_uuid"}
                                ])",
    };
    auto handler = psme::rest::model::handler::HandlerManager::get_instance()->get_handler(agent_framework::model::enums::Component::Zone);
    handler->load(agent, "fabric_1_uuid", agent_framework::model::enums::Component::Fabric, "zone_2_uuid", true);

    auto expectedReq = std::vector<std::string> {"zone_2_uuid", "zone_2_uuid"};
    CHECK_REQUESTS;

    // expect no changed ids:
    EXPECT_EQ("zone_2_uuid", psme::rest::model::Find<agent_framework::model::Zone>("2").via<agent_framework::model::Fabric>("1").get_uuid());
    EXPECT_EQ("zone_1_endpoint_1_uuid", psme::rest::model::Find<agent_framework::model::Endpoint>("1").via<agent_framework::model::Fabric>("1").get_uuid());

    auto& zone_endpoint_manager = agent_framework::module::PncComponents::get_instance()->get_zone_endpoint_manager();
    auto zone_1_port_children = zone_endpoint_manager.get_children("zone_1_uuid");
    EXPECT_EQ(zone_1_port_children.size(), 1);
    EXPECT_EQ(zone_1_port_children.front(), "zone_1_endpoint_1_uuid");

    auto zone_1_endpoint_1_parents = zone_endpoint_manager.get_parents("zone_1_endpoint_1_uuid");
    EXPECT_EQ(zone_1_endpoint_1_parents.size(), 2);
    EXPECT_EQ(zone_1_endpoint_1_parents.front(), "zone_1_uuid");
    EXPECT_EQ(zone_1_endpoint_1_parents.back(), "zone_2_uuid");

    auto zone_2_port_children = zone_endpoint_manager.get_children("zone_2_uuid");
    EXPECT_EQ(zone_2_port_children.size(), 3);

    // use sets for order-independent comparison of vectors
    std::set<std::string> zone_2_port_endpoints_set(zone_2_port_children.begin(), zone_2_port_children.end());
    std::set<std::string> expected_zone_2_children_set {"zone_2_endpoint_1_uuid", "zone_2_endpoint_2_uuid", "zone_1_endpoint_1_uuid"};
    EXPECT_EQ(zone_2_port_children.size(), zone_2_port_endpoints_set.size());
    EXPECT_EQ(zone_2_port_endpoints_set, expected_zone_2_children_set);

    // no event expected
    auto expectedEvents = std::vector<std::pair<EventType, std::string>> {};
    CHECK_EVENTS;
}


}
}
}
}
