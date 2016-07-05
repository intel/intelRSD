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
 * @section DESCRIPTION
 * */

#include <cstdio>

#include "../mocks/mock_logger_ext.hpp"

#include "agent-framework/module/module_manager.hpp"
#include "agent-framework/state_machine/state_machine.hpp"
#include "agent-framework/module/module.hpp"
#include "agent-framework/module/submodule.hpp"
#include "agent-framework/state_machine/module_state.hpp"

#include "configuration/configuration.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::generic;
using namespace agent_framework::state_machine;
using namespace agent_framework::status;
using namespace agent_framework::testing;
using ::testing::Return;
using ::testing::Throw;

using ModuleUniquePtr = Module::ModuleUniquePtr;
using module_vec_t = agent_framework::generic::ModuleManager::module_vec_t;

class ModuleTest : public ::testing::Test {
protected:
    module_vec_t m_modules{};
    std::unique_ptr<ModuleManager> m_manager{};
    ModuleUniquePtr m_module{};
    std::vector<Submodule*> m_submodules{};

    std::unique_ptr<logger_cpp::Logger> m_logusrmock{};

    static constexpr char TEST_IP_ADDRESS[] = "1.1.2.2";
    static constexpr size_t SUBMODULES_COUNT = 12;

    virtual void SetUp() {
        m_logusrmock.reset(new MockLogger);

        m_module.reset(new Module(TEST_IP_ADDRESS));
        m_manager.reset(new ModuleManager);

        for (size_t i = 0; i < SUBMODULES_COUNT; ++i) {
            m_submodules.push_back(new Submodule());
        }
    }

    virtual void TearDown() {
        m_module.reset(nullptr);
        m_manager.reset(nullptr);

        m_submodules.clear();

        m_logusrmock.reset(nullptr);
    }

    virtual ~ModuleTest();
};

constexpr char ModuleTest::TEST_IP_ADDRESS[];

ModuleTest::~ModuleTest() {}

/* Positive. */

TEST_F(ModuleTest, PositiveInitModule) {

    ASSERT_EQ(m_module->get_ip_address(), TEST_IP_ADDRESS);
    ASSERT_NE(m_module->get_name(), "");
}

TEST_F(ModuleTest, PositiveSubmodules) {

    for (auto& subm : m_submodules) {
        m_module->add_submodule(SubmoduleUniquePtr(subm));
    }

    ASSERT_EQ(m_module->get_submodules().size(), m_submodules.size());

    for (size_t subm_number = 0; subm_number < m_submodules.size(); ++subm_number) {
        ASSERT_EQ(m_module->get_submodule(subm_number)->get_name(),
                  m_submodules[subm_number]->get_name());
    }
}
