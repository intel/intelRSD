/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */


#include "gtest/gtest.h"

#include "loader/compute_loader.hpp"
#include "configuration_full.hpp"
#include "json-wrapper/json-wrapper.hpp"


using namespace agent::compute::loader;

class ComputeLoaderConfigFieldValidationTest: public ::testing::Test {
protected:
    ComputeLoader loader{};
    json::Json config = json::Json();
public:
    void SetUp() {
    }

    virtual ~ComputeLoaderConfigFieldValidationTest();
};

// To avoid vtable warning.
ComputeLoaderConfigFieldValidationTest::~ComputeLoaderConfigFieldValidationTest() { }

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadFullConfiguration_LoadShouldBePositive) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    bool is_loaded = loader.load(config);

    ASSERT_TRUE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutAgent_LoadShouldBeNegative) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    config["agent"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutAgentVendor_LoadShouldBeNegative) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    config["agent"]["vendor"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutAgentCapabilities_LoadShouldBeNegative) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    config["agent"]["capabilities"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutServer_LoadShouldBeNegative) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    config["server"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutServerPort_LoadShouldBeNegative) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    config["server"]["port"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutRegistration_LoadShouldBeNegative) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    config["registration"]= json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutRegistrationIpv4_LoadShouldBeNegative) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    config["registration"]["ipv4"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutRegistrationPort_LoadShouldBeNegative) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    config["registration"]["port"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutRegistrationInterval_LoadShouldBeNegative) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    config["registration"]["interval"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithManagerWithoutSlot_LoadShouldBePositive) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    config["logger"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_TRUE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutManagers_LoadShouldBeNegative) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    config["managers"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithZeroManagers_LoadShouldBeNegative) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    config["managers"] = json::Json::value_t::array;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithManagerWithoutIpv4_LoadShouldBeNegative) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    // Remove ipv4 field from first manager.
    config["managers"].front()["ipv4"] = json::Json::value_t::array;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithManagerWithoutUsername_LoadShouldBeNegative) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    // Remove username field from first manager.
    config["managers"].front()["username"] = json::Json::value_t::array;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithManagerWithoutPassword_LoadShouldBeNegative) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    // Remove password field from first manager.
    config["managers"].front()["password"] = json::Json::value_t::array;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithManagerWithoutPort_LoadShouldBeNegative) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    // Remove port field from first manager.
    config["managers"].front()["port"] = json::Json::value_t::array;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithManagerWithoutSlot_LoadShouldBeNegative) {
    config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);

    // Remove slot field from first manager.
    config["managers"].front()["slot"] = json::Json::value_t::array;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}
