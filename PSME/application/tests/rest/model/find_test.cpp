/*!
 * @brief find function tests
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file find_test.cpp
 */

#include "psme/rest/server/multiplexer.hpp"
#include "psme/rest/constants/routes.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/model/find.hpp"
#include "psme/rest/model/try_find.hpp"
#include "agent-framework/module/managers/generic_manager.hpp"


#include "gtest/gtest.h"

using namespace testing;
using namespace psme::rest::constants;

namespace psme {
namespace rest {
namespace server {

using StorageServiceManager = agent_framework::module::GenericManager<agent_framework::model::StorageService>;
using VolumeManager = agent_framework::module::GenericManager<agent_framework::model::Volume>;

class TestEndpointForFindApi : public MethodsHandler {
public:
    explicit TestEndpointForFindApi(const std::string& path) : MethodsHandler(path) {}
    ~TestEndpointForFindApi();
    virtual void get(const Request&, Response&) override {}
    virtual void patch(const Request&, Response&) override {}
    virtual void post(const Request&, Response&) override {}
    virtual void put(const Request&, Response&) override {}
    virtual void del(const Request&, Response&) override {}
};


TestEndpointForFindApi::~TestEndpointForFindApi() {}


class FindTest : public Test {
public:
    FindTest() :
            m_storage_services(agent_framework::module::StorageComponents::get_instance()->get_storage_service_manager()),
            m_volumes(agent_framework::module::StorageComponents::get_instance()->get_volume_manager()) {

        m_multiplexer.register_handler(TestEndpointForFindApi::UPtr(new TestEndpointForFindApi(Routes::REDFISH_PATH)));
        m_multiplexer.register_handler(TestEndpointForFindApi::UPtr(new TestEndpointForFindApi(Routes::ROOT_PATH)));
        m_multiplexer.register_handler(TestEndpointForFindApi::UPtr(new TestEndpointForFindApi(Routes::STORAGE_SERVICES_COLLECTION_PATH)));
        m_multiplexer.register_handler(TestEndpointForFindApi::UPtr(new TestEndpointForFindApi(Routes::STORAGE_SERVICE_PATH)));
        m_multiplexer.register_handler(TestEndpointForFindApi::UPtr(new TestEndpointForFindApi(Routes::VOLUME_COLLECTION_PATH)));
        m_multiplexer.register_handler(TestEndpointForFindApi::UPtr(new TestEndpointForFindApi(Routes::VOLUME_PATH)));
    }
    ~FindTest();

    void SetUp() override {
        auto S1 = agent_framework::model::StorageService("uuid_manager_M1");
        S1.set_uuid("uuid_M1_storage_service_S1"); S1.set_id(1); S1.set_agent_id("agent+_id");
        m_storage_services.add_entry(S1);

        auto S1Vol1 = agent_framework::model::Volume("uuid_M1_storage_service_S1");
        S1Vol1.set_uuid("uuid_M1_S1_volume_M1"); S1Vol1.set_id(1); S1Vol1.set_agent_id("agent+_id");
        m_volumes.add_entry(S1Vol1);

        auto S1Vol2 = agent_framework::model::Volume("uuid_M1_storage_service_S1");
        S1Vol2.set_uuid("uuid_M1_S1_volume_M2"); S1Vol2.set_id(2); S1Vol2.set_agent_id("agent+_id");
        m_volumes.add_entry(S1Vol2);

        auto S2 = agent_framework::model::StorageService("uuid_manager_M2");
        S2.set_uuid("uuid_M1_storage_service_S2"); S2.set_id(2); S2.set_agent_id("agent+_id");
        m_storage_services.add_entry(S2);

        auto S2Vol1 = agent_framework::model::Volume("uuid_M1_storage_service_S2");
        S2Vol1.set_uuid("uuid_M1_S2_volume_M1"); S2Vol1.set_id(1); S2Vol1.set_agent_id("agent+_id");
        m_volumes.add_entry(S2Vol1);

        auto S2Vol2 = agent_framework::model::Volume("uuid_M1_storage_service_S2");
        S2Vol2.set_uuid("uuid_M1_S2_volume_M2"); S2Vol2.set_id(2); S2Vol2.set_agent_id("agent+_id");
        m_volumes.add_entry(S2Vol2);
    }

    void TearDown() override {
        m_storage_services.clear_entries();
        m_volumes.clear_entries();
    }

    Multiplexer m_multiplexer{};
    StorageServiceManager& m_storage_services;
    VolumeManager& m_volumes;
};


FindTest::~FindTest() {}


TEST_F(FindTest, TestFindDirectly_ThrowAfterNotFound) {
    const auto path = "/redfish/v1/StorageServices/2/Volumes/1";
    const auto params = m_multiplexer.get_params(path, constants::Routes::VOLUME_PATH);

    ASSERT_THROW(model::find<agent_framework::model::System>(params).get_uuid(), agent_framework::exceptions::NotFound);
}

TEST_F(FindTest, TestFindDirectly_NoThrowAfterFound) {
    const auto path = "/redfish/v1/StorageServices/2/Volumes/1";
    const auto params = m_multiplexer.get_params(path, constants::Routes::VOLUME_PATH);

    ASSERT_NO_THROW(model::find<agent_framework::model::Volume>(params).get_uuid());
    ASSERT_NO_THROW(
        (model::find<agent_framework::model::StorageService, agent_framework::model::Volume>(params).get_uuid()));
}

TEST_F(FindTest, TestFindIndirectly) {
    const auto path = "/redfish/v1/StorageServices/2/Volumes/2";
    const auto params = m_multiplexer.get_params(path, constants::Routes::VOLUME_PATH);

    ASSERT_EQ("uuid_M1_storage_service_S2", model::find<agent_framework::model::StorageService>(params).get_uuid());
    ASSERT_EQ("uuid_M1_S2_volume_M2",
              (model::find<agent_framework::model::StorageService, agent_framework::model::Volume>(params).get_uuid()));
}



TEST_F(FindTest, TestFindNoexceptDirectly_NoThrowAfterNotFound) {
    const auto path = "/redfish/v1/StorageServices/2/Volumes/1";
    const auto params = m_multiplexer.get_params(path, constants::Routes::VOLUME_PATH);

    ASSERT_NO_THROW(model::try_find<agent_framework::model::System>(params).get_uuid());
}

TEST_F(FindTest, TestFindNoexceptDirectly_NoThrowAfterFound) {
    const auto path = "/redfish/v1/StorageServices/2/Volumes/1";
    const auto params = m_multiplexer.get_params(path, constants::Routes::VOLUME_PATH);

    ASSERT_NO_THROW(model::try_find<agent_framework::model::Volume>(params).get_uuid());
    ASSERT_NO_THROW(
        (model::try_find<agent_framework::model::StorageService, agent_framework::model::Volume>(params).get_uuid()));
}

TEST_F(FindTest, TestFindNoexceptIndirectly) {
    const auto path = "/redfish/v1/StorageServices/2/Volumes/2";
    const auto params = m_multiplexer.get_params(path, constants::Routes::VOLUME_PATH);

    auto opt_uuid = model::try_find<agent_framework::model::StorageService>(params).get_uuid();
    ASSERT_EQ("uuid_M1_storage_service_S2", opt_uuid.value());
    opt_uuid =
        model::try_find<agent_framework::model::StorageService, agent_framework::model::Volume>(params).get_uuid();
    ASSERT_EQ("uuid_M1_S2_volume_M2", opt_uuid.value());
}

TEST_F(FindTest, TestFindNoexceptToBool) {
    const auto true_path = "/redfish/v1/StorageServices/2/Volumes/2";
    const auto false_path = "/redfish/v1/StorageServices/2/Volumes/3";
    const auto true_params = m_multiplexer.get_params(true_path, constants::Routes::VOLUME_PATH);
    const auto false_params = m_multiplexer.get_params(false_path, constants::Routes::VOLUME_PATH);

    ASSERT_EQ(true, (model::try_find<agent_framework::model::StorageService, agent_framework::model::Volume>(true_params)));
    ASSERT_EQ(false, (model::try_find<agent_framework::model::StorageService, agent_framework::model::Volume>(false_params)));
}


}
}
}
