/*!
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
 * */

#include "utils.hpp"
#include "psme/rest-ref/endpoints/utils.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;

class RollupHealthTest : public ::testing::Test {
public:

    void SetUp() {
        ::generate_model_data();

        ComputeManager::get_instance()->
            get_system_manager().get_entry_reference("2111")->set_status({
                State::Enabled, Health::Warning });

        ComputeManager::get_instance()->
            get_system_manager().get_entry_reference("2222")->set_status({
                State::Enabled, Health::Critical });

        ComputeManager::get_instance()->
            get_processor_manager().get_entry_reference("3111")->set_status({
                State::Enabled, Health::Warning });
    }

    void TearDown() {
        ::clean_model_data();
    }

    ~RollupHealthTest();
};

RollupHealthTest::~RollupHealthTest() {}

TEST_F(RollupHealthTest, CorrectHealthRollup){
    auto manager = ComputeManager::get_instance()->
            get_module_manager().get_entry_reference("1111");
    Health children_rollup = endpoint::utils::rollup_children_health<Manager,System>(*manager);
    Health children_result{Health::Critical};
    ASSERT_EQ(children_rollup.to_string(), children_result.to_string());


    auto system = ComputeManager::get_instance()->
            get_system_manager().get_entry_reference("2111");
    Health rollup = endpoint::utils::rollup_health<System, Processor>(*system);
    Health result{Health::Warning};
    ASSERT_EQ(rollup.to_string(), result.to_string());

}
