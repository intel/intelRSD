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
#include "agent-framework/module/module.hpp"
#include "agent-framework/module/submodule.hpp"

#include "configuration/configuration.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using configuration::Configuration;

using namespace agent_framework::generic;
using namespace agent_framework::testing;
using ::testing::Return;
using ::testing::Throw;

class ModuleManagerTest : public ::testing::Test {
protected:
    std::unique_ptr<ModuleManager> m_manager{};

    std::unique_ptr<logger_cpp::Logger> m_logusrmock{};

    static constexpr char MODULES_CONFIGURATION[] = R"({
    "modules": [
        {
            "ipv4": "1.1.2.1",
            "username": "USERID",
            "password": "PASSW0RD",
            "port": 623,
            "gpio": {"model": "PCA9555", "bus": 3, "address": 32, "bank": 0, "pins": [0, 3], "inverted": false},
            "submodules": [
                { "port": 62000 },
                { "port": 62001 },
                { "port": 62002 },
                { "port": 62003 },
                { "port": 62004 },
                { "port": 62005 },
                { "port": 62006 },
                { "port": 62007 },
                { "port": 62008 },
                { "port": 62009 },
                { "port": 62010 },
                { "port": 62011 }
            ]
        },
        {
            "ipv4": "1.1.2.2",
            "username": "USERID",
            "password": "PASSW0RD",
            "port": 623,
            "gpio": {"model": "PCA9555", "bus": 3, "address": 32, "bank": 0, "pins": [1, 4], "inverted": false},
            "submodules": [ ]
        },
        {
            "ipv4": "1.1.2.3",
            "username": "USERID",
            "password": "PASSW0RD",
            "port": 623,
            "gpio": {"model": "PCA9555", "bus": 3, "address": 32, "bank": 0, "pins": [2, 5], "inverted": false},
            "submodules": [
                { "port": 62000 },
                { "port": 62001 },
                { "port": 62002 },
                { "port": 62003 },
                { "port": 62004 },
                { "port": 62005 },
                { "port": 62006 },
                { "port": 62007 },
                { "port": 62008 },
                { "port": 62009 },
                { "port": 62010 },
                { "port": 62011 }
            ]
        }
    ]
    })";
    static constexpr size_t MODULES_COUNT = 3;
    static constexpr char NONEXISTENT_UUID[] = "00000000-0000-0000-0000-000000000000";

    virtual void SetUp() {

        m_logusrmock.reset(new MockLogger);

        Configuration::get_instance()
            .set_default_configuration(MODULES_CONFIGURATION);

        m_manager.reset(new ModuleManager);
        m_manager->create();
    }

    virtual void TearDown() {
        m_manager->cleanup();
        m_manager.reset(nullptr);

        Configuration::cleanup();

        m_logusrmock.reset(nullptr);
    }

    virtual ~ModuleManagerTest();
};

constexpr char ModuleManagerTest::MODULES_CONFIGURATION[];
constexpr size_t ModuleManagerTest::MODULES_COUNT;
constexpr char ModuleManagerTest::NONEXISTENT_UUID[];

ModuleManagerTest::~ModuleManagerTest() {}

/* Positive. */

TEST_F(ModuleManagerTest, PositiveModuleManagerCreate) {
    ASSERT_NE(m_manager, nullptr);
}

TEST_F(ModuleManagerTest, PositiveModuleManager_GetModules) {
    ASSERT_EQ(m_manager->get_modules().size(), MODULES_COUNT);
}

TEST_F(ModuleManagerTest, PositiveModuleManager_GetModule) {

    auto& vec_modules = m_manager->get_modules();
    auto& module = vec_modules[0];
    auto mod_name = module->get_name();
    ASSERT_EQ(m_manager->get_module(mod_name), module.get());
}

TEST_F(ModuleManagerTest, PositiveModuleManager_GetSubmodule) {

    auto& vec_modules = m_manager->get_modules();
    auto& module = vec_modules[0];

    auto& vec_submodules = module->get_submodules();
    auto& submodule = vec_submodules[0];

    auto submod_name = submodule->get_name();

    ASSERT_EQ(m_manager->get_submodule(submod_name), submodule.get());
}

/* Negative */

TEST_F(ModuleManagerTest, NegativeModuleManager_GetModule) {
    ASSERT_EQ(m_manager->get_module(NONEXISTENT_UUID), nullptr);
}

TEST_F(ModuleManagerTest, NegativeModuleManager_GetSubmodule) {
    ASSERT_EQ(m_manager->get_submodule(NONEXISTENT_UUID), nullptr);
}

