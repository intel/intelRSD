/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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


#include "psme/rest-ref/model/finder.hpp"
#include "agent-framework/module-ref/managers/generic_manager.hpp"
#include <agent-framework/module-ref/model/manager.hpp>
#include <agent-framework/module-ref/model/dimm.hpp>
#include <agent-framework/module-ref/model/system.hpp>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <string>


using namespace agent_framework;
using namespace agent_framework::model;

namespace psme {
namespace rest {
namespace model {

using SystemManager = module::GenericManager<System>;
using DimmManager = module::GenericManager<Dimm>;

class FinderTest : public ::testing::Test {
public:
    FinderTest() :
            systems(agent_framework::module::ComputeManager::get_instance()->get_system_manager()),
            memories(agent_framework::module::ComputeManager::get_instance()->get_dimm_manager())
    { }

    ~FinderTest();

    void SetUp() {
        auto S1 = System("uuid_manager_M1"); S1.set_uuid("uuid_M1_system_S1"); S1.set_id(1); S1.set_agent_id("agent+_id"); systems.add_entry(S1);
        auto Mem1 = Dimm("uuid_M1_system_S1"); Mem1.set_uuid("uuid_M1_S1_memory_M1"); Mem1.set_id(1); Mem1.set_agent_id("agent+_id"); memories.add_entry(Mem1);
        auto Mem2 = Dimm("uuid_M1_system_S1"); Mem2.set_uuid("uuid_M1_S1_memory_M2"); Mem2.set_id(2); Mem2.set_agent_id("agent+_id"); memories.add_entry(Mem2);
        auto S2 = System("uuid_manager_M1"); S2.set_uuid("uuid_M1_system_S2"); S2.set_id(2); S2.set_agent_id("agent+_id"); systems.add_entry(S2);
        auto S2Mem1 = Dimm("uuid_M1_system_S2"); S2Mem1.set_uuid("uuid_M1_S2_memory_M1"); S2Mem1.set_id(1); S2Mem1.set_agent_id("agent+_id"); memories.add_entry(S2Mem1);
        auto S2Mem2 = Dimm("uuid_M1_system_S2"); S2Mem2.set_uuid("uuid_M1_S2_memory_M2"); S2Mem2.set_id(2); S2Mem2.set_agent_id("agent+_id"); memories.add_entry(S2Mem2);

        auto M2S1 = System("uuid_manager_M2"); M2S1.set_uuid("uuid_M2_system_S1"); M2S1.set_id(3); M2S1.set_agent_id("agent+_id"); systems.add_entry(M2S1);
        auto M2Mem1 = Dimm("uuid_M2_system_S1"); M2Mem1.set_uuid("uuid_M2_S1_memory_M1"); M2Mem1.set_id(1); M2Mem1.set_agent_id("agent+_id"); memories.add_entry(M2Mem1);
        auto M2Mem2 = Dimm("uuid_M2_system_S1"); M2Mem2.set_uuid("uuid_M2_S1_memory_M2"); M2Mem2.set_id(2); M2Mem2.set_agent_id("agent+_id"); memories.add_entry(M2Mem2);
        auto M2S2 = System("uuid_manager_M2"); M2S2.set_uuid("uuid_M2_system_S2"); M2S2.set_id(4); M2S2.set_agent_id("agent+_id"); systems.add_entry(M2S2);
        auto M2S2Mem1 = Dimm("uuid_M2_system_S2"); M2S2Mem1.set_uuid("uuid_M2_S2_memory_M1"); M2S2Mem1.set_id(1); M2S2Mem1.set_agent_id("agent+_id"); memories.add_entry(M2S2Mem1);
        auto M2S2Mem2 = Dimm("uuid_M2_system_S2"); M2S2Mem2.set_uuid("uuid_M2_S2_memory_M2"); M2S2Mem2.set_id(2); M2S2Mem2.set_agent_id("agent+_id"); memories.add_entry(M2S2Mem2);
    }

    void TearDown() {
        systems.clear_entries();
        memories.clear_entries();
    }

protected:
    SystemManager& systems;
    DimmManager& memories;
};

FinderTest::~FinderTest() { }

using namespace testing;

TEST_F(FinderTest, TestRawRestIdToUuid) {
    ASSERT_EQ("uuid_M1_system_S1", systems.rest_id_to_uuid(1, "uuid_manager_M1"));
    ASSERT_EQ("uuid_M1_system_S2", systems.rest_id_to_uuid(2, "uuid_manager_M1"));
    ASSERT_EQ("uuid_M2_system_S1", systems.rest_id_to_uuid(3, "uuid_manager_M2"));
    ASSERT_EQ("uuid_M2_system_S2", systems.rest_id_to_uuid(4, "uuid_manager_M2"));

    ASSERT_THROW(systems.rest_id_to_uuid(3, "incorrect-parent"), exceptions::NotFound);
}

TEST_F(FinderTest, TestFinderToFindDirectly) {
    ASSERT_THROW(Find<System>("10").get_one()->get_uuid(), exceptions::NotFound);
}

TEST_F(FinderTest, TestFinderToFindIndirectly) {
    ASSERT_EQ("uuid_M1_system_S1", Find<System>("1").get_one()->get_uuid());
    ASSERT_EQ("uuid_M2_S2_memory_M2", Find<Dimm>("2").via<System>("4").get_one()->get_uuid());
}

}
}
}
