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

#include "mocks.hpp"
#include "resources.hpp"

#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/requests/common/get_collection.hpp"

#include "psme/rest/model/find.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "psme/rest/server/multiplexer.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>



#define PSME_REST_EVENTING_SUBSCRIPTION_MANAGER



#include "../src/rest/model/handlers/handler_manager.cpp"



namespace psme {
namespace rest {
namespace model {
namespace handler {

class TestEndpoint : public server::MethodsHandler {
public:
    explicit TestEndpoint(const std::string& path) : server::MethodsHandler(path) {}


    ~TestEndpoint();


    virtual void get(const server::Request&, server::Response&) override {}


    virtual void patch(const server::Request&, server::Response&) override {}


    virtual void post(const server::Request&, server::Response&) override {}


    virtual void put(const server::Request&, server::Response&) override {}


    virtual void del(const server::Request&, server::Response&) override {}
};


TestEndpoint::~TestEndpoint() {}


using namespace psme::rest::eventing;
using namespace psme::rest::constants;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace testing;

using agent_framework::model::enums::Notification;
using agent_framework::model::attribute::EventData;
using SubscriptionManager = psme::rest::eventing::manager::SubscriptionManager;
using ExpectedEventRecord = std::pair<EventType, std::string>; // event type and resource URL
using ExpectedEventArray = std::vector<ExpectedEventRecord>;
using ExpectedEvents = std::vector<ExpectedEventArray>;

class DatabaseTester {
public:
    static void drop_all() {
        database::Database::SPtr db = database::Database::create("*drop_all");
        database::AlwaysMatchKey key{};
        db->drop(key);
        db->remove();
    }
};

// Required by gtest scheme for fixture tests
class HandlerTest : public Test {
public:
    HandlerTest() {
        m_multiplexer.register_handler(TestEndpoint::UPtr(new TestEndpoint(Routes::REDFISH_PATH)));
        m_multiplexer.register_handler(TestEndpoint::UPtr(new TestEndpoint(Routes::ROOT_PATH)));
        m_multiplexer.register_handler(TestEndpoint::UPtr(new TestEndpoint(Routes::MANAGER_COLLECTION_PATH)));
        m_multiplexer.register_handler(TestEndpoint::UPtr(new TestEndpoint(Routes::MANAGER_PATH)));
        m_multiplexer.register_handler(TestEndpoint::UPtr(new TestEndpoint(Routes::SYSTEMS_COLLECTION_PATH)));
        m_multiplexer.register_handler(TestEndpoint::UPtr(new TestEndpoint(Routes::SYSTEM_PATH)));
        m_multiplexer.register_handler(TestEndpoint::UPtr(new TestEndpoint(Routes::PROCESSORS_COLLECTION_PATH)));
        m_multiplexer.register_handler(TestEndpoint::UPtr(new TestEndpoint(Routes::PROCESSOR_PATH)));
        m_multiplexer.register_handler(TestEndpoint::UPtr(new TestEndpoint(Routes::TASK_COLLECTION_PATH)));
        m_multiplexer.register_handler(TestEndpoint::UPtr(new TestEndpoint(Routes::TASK_PATH)));
    }


    ~HandlerTest();


    void SetUp() {
        DatabaseTester::drop_all();
    }


    void reset_id_policy() {
        static_cast<handler::ManagerHandler*>(handler::HandlerManager::get_instance()->get_handler(
            enums::Component::Manager))->m_id_policy.reset();
        static_cast<handler::SystemHandler*>(handler::HandlerManager::get_instance()->get_handler(
            enums::Component::System))->m_id_policy.reset();
        static_cast<handler::ProcessorHandler*>(handler::HandlerManager::get_instance()->get_handler(
            enums::Component::Processor))->m_id_policy.reset();
        static_cast<handler::TaskHandler*>(handler::HandlerManager::get_instance()->get_handler(
            enums::Component::Task))->m_id_policy.reset();
    }


    void TearDown() {
        auto agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
        agent->clear();
        agent_framework::module::get_manager<agent_framework::model::Manager>().clear_entries();
        agent_framework::module::get_manager<agent_framework::model::System>().clear_entries();
        agent_framework::module::get_manager<agent_framework::model::Processor>().clear_entries();
        agent_framework::module::get_manager<agent_framework::model::Task>().clear_entries();

        reset_id_policy();

        SubscriptionManager::get_instance()->clear();
    }


    void ProcessNotification(const EventData& event) {
        auto agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
        auto handler = handler::HandlerManager::get_instance()->get_handler(event.get_type());
        psme::rest::eventing::EventVec northbound_events;
        handler->handle(agent, event, northbound_events);
        SubscriptionManager::get_instance()->notify(northbound_events);
    }


    void BuildTreeWithAddEvent(bool full = true) {
        auto agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");

        if (full) {
            agent->m_responses = {
                Manager1,
                // System collection
                R"([
                                    {"subcomponent": "system_1_uuid"},
                                    {"subcomponent": "system_2_uuid"}
                                ])",
                System1,
                System2,
                // Processor collection
                R"([
                                    {"subcomponent": "processor_1_uuid"},
                                    {"subcomponent": "processor_2_uuid"}]
                                )",
                Processor1,
                Processor2,

                Manager2,
                // Systems collection
                R"([{
                                    "subcomponent": "system_3_uuid"
                                }])",
                System3,
                // Processor collection
                R"([{"subcomponent": "processor_1_system_3_uuid"}])",
                Processor1OnSystem3
            };
        }
        else {
            agent->m_responses = {
                Manager1,
                // System collection
                R"([
                                    {"subcomponent": "system_1_uuid"},
                                    {"subcomponent": "system_2_uuid"}
                                ])",
                System1,
                System2,
                // Processor collection
                R"([
                                    {"subcomponent": "processor_1_uuid"},
                                    {"subcomponent": "processor_2_uuid"}]
                                )",
                Processor1,
                Processor2,

                Manager2,
                // Systems collection
                R"([])",
            };
        }

        EventData event;
        event.set_type(enums::Component::Manager);
        event.set_notification(Notification::Add);
        event.set_parent("");
        event.set_component("manager_1_uuid");
        ProcessNotification(event);

        event.set_type(enums::Component::Manager);
        event.set_notification(Notification::Add);
        event.set_parent("");
        event.set_component("manager_2_uuid");
        ProcessNotification(event);
    }


    void BuildTreeWithDifferentCollectionNames() {
        auto agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
        agent->m_responses = {
            Manager3,
            // FastSystem collection
            R"([
                            {"subcomponent": "system_5_uuid"}
                        ])",
            System5,
            R"([
                            {"subcomponent": "processor_1_uuid"}
                        ])",
            Processor1,
            R"([
                            {"subcomponent": "processor_2_uuid"}
                        ])",
            Processor2,
            // SlowSystem collection
            R"([
                            {"subcomponent": "system_6_uuid"}
                        ])",
            System6,
            R"([
                            {"subcomponent": "processor_3_uuid"}
                        ])",
            Processor2,
            R"([
                            {"subcomponent": "processor_4_uuid"}
                        ])",
            Processor1,
        };

        EventData event;
        event.set_type(enums::Component::Manager);
        event.set_notification(Notification::Add);
        event.set_parent("");
        event.set_component("manager_3_uuid");
        ProcessNotification(event);
    }


    auto get_params(const std::string& path, const std::string& path_template) {
        return m_multiplexer.get_params(path, path_template);
    }


    server::Multiplexer m_multiplexer{};
};


HandlerTest::~HandlerTest() {}


#define CHECK_REQUESTS \
    for (size_t i = 0; i < std::min(expectedReq.size(), agent->m_requests.size()); i++) { \
        ASSERT_EQ(expectedReq[i], agent->m_requests[i]) << "Request[" << i << "] is incorrect"; \
    } \
    ASSERT_EQ(expectedReq.size(), agent->m_requests.size()) << "Invalid number of request";

#define CHECK_EVENTS \
    {\
        auto _events = SubscriptionManager::get_instance()->m_event_arrays; \
        ASSERT_EQ(expectedEvents.size(), _events.size()) << "Invalid number of events"; \
        for (size_t i = 0; i < _events.size(); ++i) { \
            const auto& events_vec = _events[i].get_events(); \
            ASSERT_EQ(expectedEvents[i].size(), events_vec.size()); \
            for (size_t j = 0; j < events_vec.size(); ++j) { \
                ASSERT_EQ(expectedEvents[i][j].first, events_vec[j].get_type()) \
                    << "Event[" << i << "][" << j << "] type is incorrect"; \
                ASSERT_EQ(expectedEvents[i][j].second, events_vec[j].get_origin_of_condition()) \
                    << "Event[" << i << "][" << j <<"] 'origin of condition' is incorrect"; \
            } \
        } \
    }

#define CHECK_COLLECTIONS \
    {\
        for (size_t i = 0; i < std::min(expectedCollections.size(), agent->m_collections.size()); i++) { \
            ASSERT_EQ(expectedCollections[i], agent->m_collections[i]) << "Collection request[" << i << "] was not expected"; \
        } \
        ASSERT_EQ(expectedCollections.size(), agent->m_collections.size()) << "Invalid number of collections"; \
    }

TEST_F(HandlerTest, AddEverything) {
    BuildTreeWithAddEvent();

    auto agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    auto expectedReq = std::vector<std::string>{"manager_1_uuid",
                                                "manager_1_uuid",
                                                "system_1_uuid",
                                                "system_2_uuid",
                                                "system_2_uuid",
                                                "processor_1_uuid", "processor_2_uuid",
                                                "manager_2_uuid",
                                                "manager_2_uuid",
                                                "system_3_uuid",
                                                "system_3_uuid",
                                                "processor_1_system_3_uuid"};

    CHECK_REQUESTS;

    constexpr const char* MANAGER_1 = "/redfish/v1/Managers/1";
    constexpr const char* MANAGER_2 = "/redfish/v1/Managers/2";
    constexpr const char* SYSTEM_1 = "/redfish/v1/Systems/1";
    constexpr const char* SYSTEM_2 = "/redfish/v1/Systems/2";
    constexpr const char* SYSTEM_3 = "/redfish/v1/Systems/3";
    constexpr const char* PROCESSOR_1_S2 = "/redfish/v1/Systems/2/Processors/1";
    constexpr const char* PROCESSOR_2_S2 = "/redfish/v1/Systems/2/Processors/2";
    constexpr const char* PROCESSOR_1_S3 = "/redfish/v1/Systems/3/Processors/1";

    ExpectedEvents expectedEvents{
        {
            {EventType::ResourceAdded, MANAGER_1},
            {EventType::ResourceAdded, SYSTEM_1},
            {EventType::ResourceAdded, SYSTEM_2},
            {EventType::ResourceAdded, PROCESSOR_1_S2},
            {EventType::ResourceAdded, PROCESSOR_2_S2}
        },
        {
            {EventType::ResourceAdded, MANAGER_2},
            {EventType::ResourceAdded, SYSTEM_3},
            {EventType::ResourceAdded, PROCESSOR_1_S3}
        }
    };
    CHECK_EVENTS;

    ASSERT_EQ("system_1_uuid",
              find<agent_framework::model::System>(get_params(SYSTEM_1, Routes::SYSTEM_PATH)).get_uuid());
    ASSERT_EQ("system_2_uuid",
              find<agent_framework::model::System>(get_params(SYSTEM_2, Routes::SYSTEM_PATH)).get_uuid());
    ASSERT_EQ("system_3_uuid",
              find<agent_framework::model::System>(get_params(SYSTEM_3, Routes::SYSTEM_PATH)).get_uuid());
    ASSERT_EQ("processor_1_uuid", (find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_1_S2, Routes::PROCESSOR_PATH)).get_uuid()));
    ASSERT_EQ("processor_2_uuid", (find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_2_S2, Routes::PROCESSOR_PATH)).get_uuid()));
    ASSERT_EQ("processor_1_system_3_uuid", (find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_1_S3, Routes::PROCESSOR_PATH)).get_uuid()));

    ASSERT_EQ(agent_framework::model::enums::Component::Manager, find<agent_framework::model::System>(
        get_params(SYSTEM_1, Routes::SYSTEM_PATH)).get_one()->get_parent_type());
    ASSERT_EQ(agent_framework::model::enums::Component::Manager, find<agent_framework::model::System>(
        get_params(SYSTEM_2, Routes::SYSTEM_PATH)).get_one()->get_parent_type());
    ASSERT_EQ(agent_framework::model::enums::Component::Manager, find<agent_framework::model::System>(
        get_params(SYSTEM_3, Routes::SYSTEM_PATH)).get_one()->get_parent_type());
    ASSERT_EQ(agent_framework::model::enums::Component::System,
              (find<agent_framework::model::System, agent_framework::model::Processor>(
                  get_params(PROCESSOR_1_S2, Routes::PROCESSOR_PATH)).get_one()->get_parent_type()));
    ASSERT_EQ(agent_framework::model::enums::Component::System,
              (find<agent_framework::model::System, agent_framework::model::Processor>(
                  get_params(PROCESSOR_2_S2, Routes::PROCESSOR_PATH)).get_one()->get_parent_type()));
    ASSERT_EQ(agent_framework::model::enums::Component::System,
              (find<agent_framework::model::System, agent_framework::model::Processor>(
                  get_params(PROCESSOR_1_S3, Routes::PROCESSOR_PATH)).get_one()->get_parent_type()));

}

// This test should prove that we can handle ResourceAdded event
// and set parent component type correctly, despite the fact that
// it is not part of the event
TEST_F(HandlerTest, AddAssetInTheMiddleOfTreeWhenParentIsKnown) {
    BuildTreeWithAddEvent();

    //first remove system2/processor2
    auto handler = handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::Processor);
    handler->remove("processor_2_uuid");
    SubscriptionManager::get_instance()->clear();

    // now add again
    auto agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    agent->clear();
    agent->m_responses = {
        Processor2
    };

    EventData event;
    event.set_type(enums::Component::Processor);
    event.set_notification(Notification::Add);
    event.set_parent("system_2_uuid");
    event.set_component("processor_2_uuid");
    ProcessNotification(event);

    ExpectedEvents expectedEvents{{{EventType::ResourceAdded, "/redfish/v1/Systems/2/Processors/2"}}};
    CHECK_EVENTS;
}


TEST_F(HandlerTest, RemoveLeaf) {
    BuildTreeWithAddEvent();

    auto agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");

    constexpr const char* PROCESSOR_1_S2 = "/redfish/v1/Systems/2/Processors/1";
    constexpr const char* PROCESSOR_2_S2 = "/redfish/v1/Systems/2/Processors/2";

    ASSERT_EQ("processor_1_uuid", (find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_1_S2, Routes::PROCESSOR_PATH)).get_uuid()));
    ASSERT_EQ("processor_2_uuid", (find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_2_S2, Routes::PROCESSOR_PATH)).get_uuid()));

    SubscriptionManager::get_instance()->clear();

    EventData event;
    event.set_type(enums::Component::Processor);
    event.set_notification(Notification::Remove);
    event.set_parent("system_2_uuid");
    event.set_component("processor_1_uuid");
    ProcessNotification(event);

    ASSERT_THROW((find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_1_S2, Routes::PROCESSOR_PATH)).get_uuid()), agent_framework::exceptions::NotFound);
    ASSERT_EQ("processor_2_uuid", (find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_2_S2, Routes::PROCESSOR_PATH)).get_uuid()));

    ExpectedEvents expectedEvents{{{EventType::ResourceRemoved, PROCESSOR_1_S2}}};
    CHECK_EVENTS;
}


TEST_F(HandlerTest, RemoveInnerNode) {
    BuildTreeWithAddEvent();

    constexpr const char* SYSTEM_1 = "/redfish/v1/Systems/1";
    constexpr const char* SYSTEM_2 = "/redfish/v1/Systems/2";
    constexpr const char* PROCESSOR_1_S2 = "/redfish/v1/Systems/2/Processors/1";
    constexpr const char* PROCESSOR_2_S2 = "/redfish/v1/Systems/2/Processors/2";

    ASSERT_EQ(2, get_manager<agent_framework::model::Manager>().get_entry_count());
    ASSERT_EQ(3, get_manager<agent_framework::model::System>().get_entry_count());
    ASSERT_EQ(3, get_manager<agent_framework::model::Processor>().get_entry_count());

    ASSERT_EQ("system_1_uuid",
              find<agent_framework::model::System>(get_params(SYSTEM_1, Routes::SYSTEM_PATH)).get_uuid());
    ASSERT_EQ("system_2_uuid",
              find<agent_framework::model::System>(get_params(SYSTEM_2, Routes::SYSTEM_PATH)).get_uuid());

    SubscriptionManager::get_instance()->clear();

    EventData event;
    event.set_type(enums::Component::System);
    event.set_notification(Notification::Remove);
    event.set_parent("manager_1_uuid");
    event.set_component("system_2_uuid");
    ProcessNotification(event);

    ASSERT_EQ("system_1_uuid",
              find<agent_framework::model::System>(get_params(SYSTEM_1, Routes::SYSTEM_PATH)).get_uuid());
    ASSERT_THROW(find<agent_framework::model::System>(get_params(SYSTEM_2, Routes::SYSTEM_PATH)).get_uuid(),
                 agent_framework::exceptions::NotFound);
    ASSERT_THROW((find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_1_S2, Routes::PROCESSOR_PATH)).get_uuid()), agent_framework::exceptions::NotFound);
    ASSERT_THROW((find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_2_S2, Routes::PROCESSOR_PATH)).get_uuid()), agent_framework::exceptions::NotFound);

    ASSERT_EQ(2, get_manager<agent_framework::model::Manager>().get_entry_count());
    ASSERT_EQ(3 - 1, get_manager<agent_framework::model::System>().get_entry_count());
    ASSERT_EQ(3 - 2, get_manager<agent_framework::model::Processor>().get_entry_count());

    ExpectedEvents expectedEvents{
        {
            {EventType::ResourceRemoved, PROCESSOR_1_S2},
            {EventType::ResourceRemoved, PROCESSOR_2_S2},
            {EventType::ResourceRemoved, SYSTEM_2}
        }
    };
    CHECK_EVENTS;
}


TEST_F(HandlerTest, TestProperIdsAfterAgentReconnection) {
    BuildTreeWithAddEvent();

    constexpr const char* SYSTEM_1 = "/redfish/v1/Systems/1";
    constexpr const char* SYSTEM_2 = "/redfish/v1/Systems/2";
    constexpr const char* SYSTEM_3 = "/redfish/v1/Systems/3";
    constexpr const char* PROCESSOR_1_S2 = "/redfish/v1/Systems/2/Processors/1";
    constexpr const char* PROCESSOR_2_S2 = "/redfish/v1/Systems/2/Processors/2";
    constexpr const char* PROCESSOR_1_S3 = "/redfish/v1/Systems/3/Processors/1";

    const auto agent_gami_id = psme::core::agent::AgentManager::get_instance()->get_agent("anything")->get_gami_id();

    // check if all resources have been added to the model
    ASSERT_EQ(2, get_manager<agent_framework::model::Manager>().get_entry_count());
    ASSERT_EQ(3, get_manager<agent_framework::model::System>().get_entry_count());
    ASSERT_EQ(3, get_manager<agent_framework::model::Processor>().get_entry_count());

    ASSERT_EQ("system_1_uuid",
              find<agent_framework::model::System>(get_params(SYSTEM_1, Routes::SYSTEM_PATH)).get_uuid());
    ASSERT_EQ("system_2_uuid",
              find<agent_framework::model::System>(get_params(SYSTEM_2, Routes::SYSTEM_PATH)).get_uuid());
    ASSERT_EQ("system_3_uuid",
              find<agent_framework::model::System>(get_params(SYSTEM_3, Routes::SYSTEM_PATH)).get_uuid());
    ASSERT_EQ("processor_1_uuid", (find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_1_S2, Routes::PROCESSOR_PATH)).get_uuid()));
    ASSERT_EQ("processor_2_uuid", (find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_2_S2, Routes::PROCESSOR_PATH)).get_uuid()));
    ASSERT_EQ("processor_1_system_3_uuid", (find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_1_S3, Routes::PROCESSOR_PATH)).get_uuid()));

    // remove agent to simulate connection error
    handler::HandlerManager::get_instance()->remove_agent_data(agent_gami_id);
    psme::core::agent::AgentManager::get_instance()->remove_agent(agent_gami_id);

    // check if all resources have been removed from the model
    ASSERT_EQ(0, get_manager<agent_framework::model::Manager>().get_entry_count());
    ASSERT_EQ(0, get_manager<agent_framework::model::System>().get_entry_count());
    ASSERT_EQ(0, get_manager<agent_framework::model::Processor>().get_entry_count());
    ASSERT_THROW(find<agent_framework::model::System>(get_params(SYSTEM_1, Routes::SYSTEM_PATH)).get_uuid(),
                 agent_framework::exceptions::NotFound);
    ASSERT_THROW(find<agent_framework::model::System>(get_params(SYSTEM_2, Routes::SYSTEM_PATH)).get_uuid(),
                 agent_framework::exceptions::NotFound);
    ASSERT_THROW(find<agent_framework::model::System>(get_params(SYSTEM_3, Routes::SYSTEM_PATH)).get_uuid(),
                 agent_framework::exceptions::NotFound);
    ASSERT_THROW((find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_1_S2, Routes::PROCESSOR_PATH)).get_uuid()), agent_framework::exceptions::NotFound);
    ASSERT_THROW((find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_2_S2, Routes::PROCESSOR_PATH)).get_uuid()), agent_framework::exceptions::NotFound);

    // reconnect agent
    BuildTreeWithAddEvent();

    // check if all resources have been added to the model with updated ids
    ASSERT_EQ(2, get_manager<agent_framework::model::Manager>().get_entry_count());
    ASSERT_EQ(3, get_manager<agent_framework::model::System>().get_entry_count());
    ASSERT_EQ(3, get_manager<agent_framework::model::Processor>().get_entry_count());
    ASSERT_EQ("system_1_uuid",
              find<agent_framework::model::System>(get_params(SYSTEM_1, Routes::SYSTEM_PATH)).get_uuid());
    ASSERT_EQ("system_2_uuid",
              find<agent_framework::model::System>(get_params(SYSTEM_2, Routes::SYSTEM_PATH)).get_uuid());
    ASSERT_EQ("system_3_uuid",
              find<agent_framework::model::System>(get_params(SYSTEM_3, Routes::SYSTEM_PATH)).get_uuid());
    ASSERT_EQ("processor_1_uuid", (find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_1_S2, Routes::PROCESSOR_PATH)).get_uuid()));
    ASSERT_EQ("processor_2_uuid", (find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_2_S2, Routes::PROCESSOR_PATH)).get_uuid()));
}


TEST_F(HandlerTest, LoadLeaf) {
    BuildTreeWithAddEvent();
    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    constexpr const char* PROCESSOR_1_S3 = "/redfish/v1/Systems/3/Processors/1";

    auto agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    auto handler = handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::Processor);
    agent->m_responses = {
        Processor1OnSystem3Modified
    };
    auto rest_id = handler->load(agent, "system_3_uuid", agent_framework::model::enums::Component::System,
                                 "processor_1_system_3_uuid");
    EXPECT_EQ(1, rest_id); // should retain rest id

    auto expectedReq = std::vector<std::string>{"processor_1_system_3_uuid"};
    CHECK_REQUESTS;

    auto proc = find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_1_S3, Routes::PROCESSOR_PATH)).get_one();

    EXPECT_EQ("processor_1_system_3_uuid", proc->get_uuid());
    EXPECT_EQ(enums::State::Enabled, proc->get_status().get_state());

    ExpectedEvents expectedEvents{
        {
            {EventType::StatusChange, PROCESSOR_1_S3},
            {EventType::ResourceUpdated, PROCESSOR_1_S3}
        }
    };
    CHECK_EVENTS;
}


TEST_F(HandlerTest, LoadInternalNonRecursively) {
    BuildTreeWithAddEvent();
    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    constexpr const char* SYSTEM_2 = "/redfish/v1/Systems/2";

    auto agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    auto handler = handler::HandlerManager::get_instance()
        ->get_handler(agent_framework::model::enums::Component::System);
    agent->m_responses = {
        System2Modified
    };
    auto rest_id = handler->load(agent, "manager_1_uuid", agent_framework::model::enums::Component::Manager,
                                 "system_2_uuid");
    EXPECT_EQ(2, rest_id); // should retain rest id

    auto expectedReq = std::vector<std::string>{"system_2_uuid"};
    CHECK_REQUESTS;

    auto system = find<agent_framework::model::System>(get_params(SYSTEM_2, Routes::SYSTEM_PATH)).get_one();
    EXPECT_EQ("system_2_uuid", system->get_uuid());
    EXPECT_EQ("B20F21_A0" /* new value from updated system */, system->get_bios_version());

    ExpectedEvents expectedEvents{{{EventType::ResourceUpdated, SYSTEM_2}}};
    CHECK_EVENTS;
}


TEST_F(HandlerTest, LoadInternalRecursively) {
    BuildTreeWithAddEvent();
    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    constexpr const char* SYSTEM_2 = "/redfish/v1/Systems/2";
    constexpr const char* PROCESSOR_2_S2 = "/redfish/v1/Systems/2/Processors/2";

    auto agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    auto handler = handler::HandlerManager::get_instance()
        ->get_handler(agent_framework::model::enums::Component::System);
    agent->m_responses = {
        System2Modified,
        R"([
            {"subcomponent": "processor_1_uuid"},
            {"subcomponent": "processor_2_uuid"}]
        )",
        Processor1,
        Processor2Modified
    };
    auto rest_id = handler->load(agent, "manager_1_uuid", agent_framework::model::enums::Component::Manager,
                                 "system_2_uuid", true /* recursively */);
    EXPECT_EQ(2, rest_id); // should retain rest id

    auto expectedReq = std::vector<std::string>{"system_2_uuid", "system_2_uuid", "processor_1_uuid",
                                                "processor_2_uuid"};
    CHECK_REQUESTS;

    auto system = find<agent_framework::model::System>(get_params(SYSTEM_2, Routes::SYSTEM_PATH)).get_one();
    EXPECT_EQ("system_2_uuid", system->get_uuid());
    EXPECT_EQ("B20F21_A0" /* new value from updated system */, system->get_bios_version());

    auto processor2 = find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_2_S2, Routes::PROCESSOR_PATH)).get_one();
    EXPECT_EQ("processor_2_uuid", processor2->get_uuid());
    EXPECT_EQ("666" /* new value from updated processor */, processor2->get_socket());

    ExpectedEvents expectedEvents{
        {
            {EventType::ResourceUpdated, SYSTEM_2},
            {EventType::ResourceUpdated, PROCESSOR_2_S2}
        }
    };
    CHECK_EVENTS;
}


TEST_F(HandlerTest, TestUpdateEventIgnoredForUnknownResource) {
    BuildTreeWithAddEvent();
    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    auto agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    agent->m_responses = {
    };

    EventData event;
    event.set_type(enums::Component::System);
    event.set_notification(Notification::Update);
    event.set_parent("manager_1_uuid");
    event.set_component("unknown_system_uuid");
    ProcessNotification(event);

    auto expectedReq = std::vector<std::string>{};
    CHECK_REQUESTS;

    ExpectedEvents expectedEvents{};
    CHECK_EVENTS;
}


TEST_F(HandlerTest, TestUpdateEventNotForwardedWhenNoChange) {
    BuildTreeWithAddEvent();
    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    auto agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    agent->m_responses = {
        System2 // identical to original one
    };

    EventData event;
    event.set_type(enums::Component::System);
    event.set_notification(Notification::Update);
    event.set_parent("manager_1_uuid");
    event.set_component("system_2_uuid");
    ProcessNotification(event);

    auto expectedReq = std::vector<std::string>{"system_2_uuid"};
    CHECK_REQUESTS;

    ExpectedEvents expectedEvents{};
    CHECK_EVENTS;
}


TEST_F(HandlerTest, TestSystemRefreshedAfterUpdateEvent) {
    BuildTreeWithAddEvent();
    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    constexpr const char* SYSTEM_2 = "/redfish/v1/Systems/2";

    auto agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    agent->m_responses = {
        System2Modified
    };

    EventData event;
    event.set_type(enums::Component::System);
    event.set_notification(Notification::Update);
    event.set_parent("manager_1_uuid");
    event.set_component("system_2_uuid");
    ProcessNotification(event);

    auto expectedReq = std::vector<std::string>{"system_2_uuid"};
    CHECK_REQUESTS;

    auto system = find<agent_framework::model::System>(get_params(SYSTEM_2, Routes::SYSTEM_PATH)).get_one();
    EXPECT_EQ("system_2_uuid", system->get_uuid());
    EXPECT_EQ("B20F21_A0" /* new value from updated system */, system->get_bios_version());

    ExpectedEvents expectedEvents{{{EventType::ResourceUpdated, SYSTEM_2}}};
    CHECK_EVENTS;
}


TEST_F(HandlerTest, TestProcessorRefreshedAfterUpdateEvent) {
    BuildTreeWithAddEvent();
    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    constexpr const char* PROCESSOR_1_S3 = "/redfish/v1/Systems/3/Processors/1";

    auto agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    agent->m_responses = {
        Processor1OnSystem3Modified
    };

    EventData event;
    event.set_type(enums::Component::Processor);
    event.set_notification(Notification::Update);
    event.set_parent("system_3_uuid");
    event.set_component("processor_1_system_3_uuid");
    ProcessNotification(event);

    auto expectedReq = std::vector<std::string>{"processor_1_system_3_uuid"};
    CHECK_REQUESTS;

    auto proc = find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_1_S3, Routes::PROCESSOR_PATH)).get_one();
    EXPECT_EQ("processor_1_system_3_uuid", proc->get_uuid());
    EXPECT_EQ(enums::State::Enabled, proc->get_status().get_state());

    ExpectedEvents expectedEvents{
        {
            {EventType::StatusChange, PROCESSOR_1_S3},
            {EventType::ResourceUpdated, PROCESSOR_1_S3}
        }
    };
    CHECK_EVENTS;
}


TEST_F(HandlerTest, PollingNoUpdate) {
    BuildTreeWithAddEvent();
    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    psme::core::agent::JsonAgentSPtr agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    auto handler = handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::System);
    agent->m_responses = {
        // Manager 1
        System2,
        // Processor Collection
        R"([
                    {"subcomponent": "processor_1_uuid"},
                    {"subcomponent": "processor_2_uuid"}
               ])",
        Processor1,
        Processor2
    };
    handler->poll(agent, "manager_1_uuid", agent_framework::model::enums::Component::Manager, "system_2_uuid");

    ExpectedEvents expectedEvents{};
    CHECK_EVENTS;
}


TEST_F(HandlerTest, PollingDetectsLeafRemoved) {
    BuildTreeWithAddEvent();
    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    psme::core::agent::JsonAgentSPtr agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    auto handler = handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::System);
    agent->m_responses = {
        System2,
        // Processor collection
        R"([
                {"subcomponent": "processor_1_uuid"}
          ])",
        //processor 2 removed (also from collection)
        Processor1
    };
    handler->poll(agent, "manager_1_uuid", agent_framework::model::enums::Component::Manager, "system_2_uuid");

    ExpectedEvents expectedEvents{{{EventType::ResourceRemoved, "/redfish/v1/Systems/2/Processors/2"}}};
    CHECK_EVENTS;
}


TEST_F(HandlerTest, PollingDetectsInnerNodeRemoved) {
    BuildTreeWithAddEvent();

    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    psme::core::agent::JsonAgentSPtr agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    auto handler = handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::Manager);
    agent->m_responses = {
        Manager1,
        // System collection
        R"([
                    {"subcomponent": "system_1_uuid"}
                ])",
        System1,
        //System2 removed
    };

    handler->poll(agent, "", agent_framework::model::enums::Component::None, "manager_1_uuid");

    ExpectedEvents expectedEvents{
        {
            {EventType::ResourceRemoved, "/redfish/v1/Systems/2/Processors/1"},
            {EventType::ResourceRemoved, "/redfish/v1/Systems/2/Processors/2"},
            {EventType::ResourceRemoved, "/redfish/v1/Systems/2"}
        }
    };
    CHECK_EVENTS;
}


TEST_F(HandlerTest, PollingDetectsInnerAndLeafNodesRemoved) {
    BuildTreeWithAddEvent();

    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    psme::core::agent::JsonAgentSPtr agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    auto handler = handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::Manager);
    agent->m_responses = {
        Manager1,
        // System collection
        R"([
               ])"
        //System1 removed
        //System2 removed
    };

    handler->poll(agent, "", agent_framework::model::enums::Component::None, "manager_1_uuid");

    ExpectedEvents expectedEvents{
        {
            {EventType::ResourceRemoved, "/redfish/v1/Systems/1"},
            {EventType::ResourceRemoved, "/redfish/v1/Systems/2/Processors/1"},
            {EventType::ResourceRemoved, "/redfish/v1/Systems/2/Processors/2"},
            {EventType::ResourceRemoved, "/redfish/v1/Systems/2"}
        }
    };
    CHECK_EVENTS;
}


TEST_F(HandlerTest, PollingDetectsUpdate) {
    BuildTreeWithAddEvent();
    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    psme::core::agent::JsonAgentSPtr agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    auto handler = handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::System);
    agent->m_responses = {
        // Manager 1
        System2Modified,
        // Processor Collection
        R"([
                        {"subcomponent": "processor_1_uuid"},
                        {"subcomponent": "processor_2_uuid"}
                   ])",
        Processor1,
        Processor2
    };
    handler->poll(agent, "manager_1_uuid", agent_framework::model::enums::Component::Manager, "system_2_uuid");

    ExpectedEvents expectedEvents{{{EventType::ResourceUpdated, "/redfish/v1/Systems/2"}}};
    CHECK_EVENTS;
}


TEST_F(HandlerTest, PollingDetectsSubtreeAdded) {
    BuildTreeWithAddEvent(false /*only first manager*/);
    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    psme::core::agent::JsonAgentSPtr agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    auto handler = handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::Manager);
    agent->m_responses = {
        Manager2,
        // Systems collection
        R"([
                {"subcomponent": "system_3_uuid"},
                {"subcomponent": "system_4_uuid"}
        ])",
        System3,
        // Processor collection
        R"([{"subcomponent": "processor_1_system_3_uuid"}])",
        Processor1OnSystem3,
        System4,
        // Processor collection
        R"([{"subcomponent": "processor_1_system_4_uuid"}])",
        Processor1OnSystem4
    };

    handler->poll(agent, "", agent_framework::model::enums::Component::None, "manager_2_uuid");

    ExpectedEvents expectedEvents{
        {
            {EventType::ResourceAdded, "/redfish/v1/Systems/3"},
            {EventType::ResourceAdded, "/redfish/v1/Systems/3/Processors/1"},
            {EventType::ResourceAdded, "/redfish/v1/Systems/4"},
            {EventType::ResourceAdded, "/redfish/v1/Systems/4/Processors/1"}
        }
    };
    CHECK_EVENTS;
}


TEST_F(HandlerTest, PollingQuitsAfterRpcException) {
    BuildTreeWithAddEvent();
    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    psme::core::agent::JsonAgentSPtr agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    auto handler = handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::Manager);
    agent->m_responses = {
        Manager1,
        // System collection
        R"([
                                    {"subcomponent": "system_1_uuid"},
                                    {"subcomponent": "system_2_uuid"}
                                ])",
        System1,
        "[JsonRpcException]"
    };
    handler->poll(agent, "", agent_framework::model::enums::Component::None, "manager_1_uuid");

    auto expectedReq = std::vector<std::string>{"manager_1_uuid",
                                                "manager_1_uuid",
                                                "system_1_uuid",
                                                "system_2_uuid"
    };

    CHECK_REQUESTS;

    ExpectedEvents expectedEvents{};
    CHECK_EVENTS;
}


TEST_F(HandlerTest, PollingKeepsStableIds) {
    BuildTreeWithAddEvent();
    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    psme::core::agent::JsonAgentSPtr agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    auto handler = handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::System);

    agent->m_responses = {
        System2,
        // Processors collection
        R"([
                {"subcomponent": "processor_2_uuid"}
            ])",
        Processor2
    };

    handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::Processor)->remove("processor_1_uuid");
    ASSERT_EQ(2, get_manager<agent_framework::model::Processor>().get_entry("processor_2_uuid").get_id());
    handler->poll(agent, "manager_1_uuid", agent_framework::model::enums::Component::Manager, "system_2_uuid");
    ASSERT_EQ(2, get_manager<agent_framework::model::Processor>().get_entry("processor_2_uuid").get_id());

    ExpectedEvents expectedEvents{{{EventType::ResourceRemoved, "/redfish/v1/Systems/2/Processors/1"}}};
    CHECK_EVENTS;
}


TEST_F(HandlerTest, NothingAddedIfJsonRpcExeptionDuringAddEventHandling) {
    BuildTreeWithAddEvent(false /*only first manager*/);
    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    constexpr const char* SYSTEM_3 = "/redfish/v1/Systems/3";

    psme::core::agent::JsonAgentSPtr agent = psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    agent->m_responses = {
        System3,
        // Processor collection
        R"([{"subcomponent": "processor_1_system_3_uuid"}])",
        "[JsonRpcException]"
    };

    EventData event;
    event.set_type(enums::Component::System);
    event.set_notification(Notification::Add);
    event.set_parent("manager_2_uuid");
    event.set_component("system_3_uuid");
    ProcessNotification(event);

    auto expectedReq = std::vector<std::string>{"system_3_uuid",
                                                "system_3_uuid",
                                                "processor_1_system_3_uuid"
    };

    CHECK_REQUESTS;

    // events about added cpus should not be sent
    ExpectedEvents expectedEvents{};
    CHECK_EVENTS;

    ASSERT_EQ(2, get_manager<agent_framework::model::Manager>().get_entry_count());
    ASSERT_EQ(2, get_manager<agent_framework::model::System>().get_entry_count());
    ASSERT_EQ(2, get_manager<agent_framework::model::Processor>().get_entry_count());
    ASSERT_THROW((find<agent_framework::model::System>(get_params(SYSTEM_3, Routes::SYSTEM_PATH)).get_uuid()),
                 agent_framework::exceptions::NotFound);
}


TEST_F(HandlerTest, DifferentCollectionNamesForSameResourceType) {
    psme::core::agent::JsonAgentSPtr agent =
        psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    agent->clear();
    SubscriptionManager::get_instance()->clear();
    BuildTreeWithDifferentCollectionNames();

    constexpr const char* SYSTEM_1 = "/redfish/v1/Systems/1";
    constexpr const char* SYSTEM_2 = "/redfish/v1/Systems/2";
    constexpr const char* PROCESSOR_1_S1 = "/redfish/v1/Systems/1/Processors/1";
    constexpr const char* PROCESSOR_2_S1 = "/redfish/v1/Systems/1/Processors/2";
    constexpr const char* PROCESSOR_1_S2 = "/redfish/v1/Systems/2/Processors/1";
    constexpr const char* PROCESSOR_2_S2 = "/redfish/v1/Systems/2/Processors/2";

    auto expectedReq = std::vector<std::string>{"manager_3_uuid",
                                                "manager_3_uuid",
                                                "system_5_uuid",
                                                "system_5_uuid",
                                                "processor_1_uuid",
                                                "system_5_uuid",
                                                "processor_2_uuid",
                                                "manager_3_uuid",
                                                "system_6_uuid",
                                                "system_6_uuid",
                                                "processor_3_uuid",
                                                "system_6_uuid",
                                                "processor_4_uuid",
    };

    CHECK_REQUESTS;

    auto expectedCollections = std::vector<std::pair<std::string, std::string>>
        {{"FastSystems",    "manager_3_uuid"},
         {"FastProcessors", "system_5_uuid"},
         {"SlowProcessors", "system_5_uuid"},
         {"SlowSystems",    "manager_3_uuid"},
         {"Anything",       "system_6_uuid"},
         {"whAtever",       "system_6_uuid"}
        };

    CHECK_COLLECTIONS;

    psme::core::agent::AgentManager::get_instance()->get_agent("anything")->clear();
    SubscriptionManager::get_instance()->clear();

    ASSERT_EQ("system_5_uuid",
              (find<agent_framework::model::System>(get_params(SYSTEM_1, Routes::SYSTEM_PATH)).get_uuid()));

    ASSERT_EQ("system_6_uuid",
              (find<agent_framework::model::System>(get_params(SYSTEM_2, Routes::SYSTEM_PATH)).get_uuid()));

    ASSERT_EQ("processor_1_uuid", (find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_1_S1, Routes::PROCESSOR_PATH)).get_uuid()));

    ASSERT_EQ("processor_2_uuid", (find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_2_S1, Routes::PROCESSOR_PATH)).get_uuid()));

    ASSERT_EQ("processor_3_uuid", (find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_1_S2, Routes::PROCESSOR_PATH)).get_uuid()));

    ASSERT_EQ("processor_4_uuid", (find<agent_framework::model::System, agent_framework::model::Processor>(
        get_params(PROCESSOR_2_S2, Routes::PROCESSOR_PATH)).get_uuid()));

    ExpectedEvents expectedEvents{};
    CHECK_EVENTS;
}


TEST_F(HandlerTest, LoadCollectionNonRecursive) {
    BuildTreeWithDifferentCollectionNames();
    psme::core::agent::JsonAgentSPtr agent =
        psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    agent->clear();
    SubscriptionManager::get_instance()->clear();

    agent->m_responses = {
        Manager3,
        // FastSystems collection
        R"([
            {"subcomponent": "system_5_uuid"}
        ])",
        System5,
        // SlowSystems collection
        R"([
            {"subcomponent": "system_6_uuid"}
        ])",
        System6,
    };
    auto handler = handler::HandlerManager::get_instance()
        ->get_handler(agent_framework::model::enums::Component::Manager);
    handler->load_collection(agent, "manager_3_uuid", agent_framework::model::enums::Component::Manager,
                             enums::CollectionType::Systems, false);
    auto expectedReq = std::vector<std::string>{
        "manager_3_uuid",
        "manager_3_uuid",
        "system_5_uuid",
        "manager_3_uuid",
        "system_6_uuid"
    };

    CHECK_REQUESTS;

    ExpectedEvents expectedEvents{};
    CHECK_EVENTS;
}


TEST_F(HandlerTest, LoadCollectionRecursive) {
    BuildTreeWithDifferentCollectionNames();
    psme::core::agent::JsonAgentSPtr agent =
        psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    agent->clear();
    SubscriptionManager::get_instance()->clear();
    agent->m_responses = {
        Manager3,
        // FastSystems collection
        R"([
            {"subcomponent": "system_5_uuid"}
        ])",
        System5,
        R"([
            {"subcomponent": "processor_1_uuid"}
        ])",
        Processor1,
        R"([
            {"subcomponent": "processor_2_uuid"}
        ])",
        Processor2,
        // SlowSystems collection
        R"([
            {"subcomponent": "system_6_uuid"}
        ])",
        System6,
        R"([
            {"subcomponent": "processor_3_uuid"}
        ])",
        Processor2,
        R"([
            {"subcomponent": "processor_4_uuid"}
        ])",
        Processor1,
    };

    auto handler = handler::HandlerManager::get_instance()
        ->get_handler(agent_framework::model::enums::Component::Manager);
    handler->load_collection(agent, "manager_3_uuid", agent_framework::model::enums::Component::Manager,
                             enums::CollectionType::Systems, true);

    auto expectedReq = std::vector<std::string>{
        "manager_3_uuid",
        "manager_3_uuid",
        "system_5_uuid",
        "system_5_uuid",
        "processor_1_uuid",
        "system_5_uuid",
        "processor_2_uuid",
        "manager_3_uuid",
        "system_6_uuid",
        "system_6_uuid",
        "processor_3_uuid",
        "system_6_uuid",
        "processor_4_uuid",
    };

    CHECK_REQUESTS;

    ExpectedEvents expectedEvents{};
    CHECK_EVENTS;
}


TEST_F(HandlerTest, LoadCollectionNonRecursiveAddDelete) {
    BuildTreeWithDifferentCollectionNames();
    psme::core::agent::JsonAgentSPtr agent =
        psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    agent->clear();
    SubscriptionManager::get_instance()->clear();

    // Moving System6 to the other collection with different uuid
    agent->m_responses = {
        Manager3,
        // FastSystems collection
        R"([
            {"subcomponent": "system_5_uuid"},
            {"subcomponent": "system_NEW_uuid"}
        ])",
        System5,
        System6,
        // SlowSystems collection
        R"([])",
    };
    auto handler = handler::HandlerManager::get_instance()
        ->get_handler(agent_framework::model::enums::Component::Manager);
    handler->load_collection(agent, "manager_3_uuid", agent_framework::model::enums::Component::Manager,
                             enums::CollectionType::Systems, false);

    auto expectedReq = std::vector<std::string>{
        "manager_3_uuid",
        "manager_3_uuid",
        "system_5_uuid",
        "system_NEW_uuid",
        "manager_3_uuid"
    };

    CHECK_REQUESTS;

    ExpectedEvents expectedEvents{
        {
            {EventType::ResourceAdded, "/redfish/v1/Systems/3"},
            {EventType::ResourceRemoved, "/redfish/v1/Systems/2/Processors/1"},
            {EventType::ResourceRemoved, "/redfish/v1/Systems/2/Processors/2"},
            {EventType::ResourceRemoved, "/redfish/v1/Systems/2"}
        }
    };
    CHECK_EVENTS;
}


TEST_F(HandlerTest, LoadCollectionRecursiveAddDelete) {
    BuildTreeWithDifferentCollectionNames();
    psme::core::agent::JsonAgentSPtr agent =
        psme::core::agent::AgentManager::get_instance()->get_agent("anything");
    agent->clear();
    SubscriptionManager::get_instance()->clear();

    // Moving System6 to the other collection with different uuid
    // Removing Processor2 and adding same processor, but with different uuid
    agent->m_responses = {
        Manager3,
        // FastSystems collection
        R"([
            {"subcomponent": "system_5_uuid"},
            {"subcomponent": "system_NEW_uuid"}
        ])",
        System5,
        R"([
            {"subcomponent": "processor_1_uuid"}
        ])",
        Processor1,
        R"([
            {"subcomponent": "processor_NEW_uuid"}
        ])",
        Processor2,
        System6,
        R"([
            {"subcomponent": "processor_5_uuid"}
        ])",
        Processor2,
        R"([
            {"subcomponent": "processor_6_uuid"}
        ])",
        Processor1,
        // SlowSystems collection
        R"([])"
    };

    auto handler = handler::HandlerManager::get_instance()
        ->get_handler(agent_framework::model::enums::Component::Manager);
    handler->load_collection(agent, "manager_3_uuid", agent_framework::model::enums::Component::Manager,
                             enums::CollectionType::Systems, true);

    auto expectedReq = std::vector<std::string>{
        "manager_3_uuid",
        "manager_3_uuid",
        "system_5_uuid",
        "system_5_uuid",
        "processor_1_uuid",
        "system_5_uuid",
        "processor_NEW_uuid",
        "system_NEW_uuid",
        "system_NEW_uuid",
        "processor_5_uuid",
        "system_NEW_uuid",
        "processor_6_uuid",
        "manager_3_uuid"
    };

    CHECK_REQUESTS;

    ExpectedEvents expectedEvents{
        {
            {EventType::ResourceAdded, "/redfish/v1/Systems/1/Processors/3"},
            {EventType::ResourceRemoved, "/redfish/v1/Systems/1/Processors/2"},
            {EventType::ResourceAdded, "/redfish/v1/Systems/3"},
            {EventType::ResourceAdded, "/redfish/v1/Systems/3/Processors/1"},
            {EventType::ResourceAdded, "/redfish/v1/Systems/3/Processors/2"},
            {EventType::ResourceRemoved, "/redfish/v1/Systems/2/Processors/1"},
            {EventType::ResourceRemoved, "/redfish/v1/Systems/2/Processors/2"},
            {EventType::ResourceRemoved, "/redfish/v1/Systems/2"}
        }
    };
    CHECK_EVENTS;
}


TEST_F(HandlerTest, LoadTaskWithServerException) {

    constexpr const char* TASK_1 = "/redfish/v1/TaskService/Tasks/1";

    psme::core::agent::JsonAgentSPtr agent =
        psme::core::agent::AgentManager::get_instance()->get_agent("anything");

    agent->m_responses = {
        // response to GetTaskInfo
        Task1,
        // Exception on GetTaskResult
        "[ServerException]"
    };

    auto handler = handler::HandlerManager::get_instance()
        ->get_handler(agent_framework::model::enums::Component::Task);
    auto rest_id = handler->load(agent, "", agent_framework::model::enums::Component::None, "task_1_uuid");

    // task got id=1
    EXPECT_EQ(1, rest_id);

    auto expectedReq = std::vector<std::string>{
        "task_1_uuid",
        // if the task is in state Exception, GetTaskResult is called
        "task_1_uuid"
    };

    CHECK_REQUESTS;

    ExpectedEvents expectedEvents{{{EventType::ResourceAdded, TASK_1}}};
    CHECK_EVENTS;

    Task task = find<agent_framework::model::Task>(get_params(TASK_1, Routes::TASK_PATH)).get();
    ASSERT_EQ("task_1_uuid", task.get_uuid());

    // Mock Task1 in resources.hpp had no messages. Now the task has one, with the unpacked exception from GetTaskResult
    ASSERT_EQ(task.get_messages().size(), 1);
}

}
}
}
}
