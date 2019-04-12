/*!
 * @brief Implementation of validation tests.
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
 * @file loader_validation_test.cpp
 */

#include "gtest/gtest.h"

#include "loader/loader.hpp"
#include "configuration_full.hpp"
#include "json-wrapper/json-wrapper.hpp"


using namespace agent::nvme::loader;

class NvmeLoaderConfigFieldValidationTest: public ::testing::Test {
protected:
    NvmeLoader loader{};
    json::Json config = json::Json();
public:
    void SetUp() {
        config = json::Json::parse(NVME_FULL_CONFIGURATION);
    }

    virtual ~NvmeLoaderConfigFieldValidationTest();
};

NvmeLoaderConfigFieldValidationTest::~NvmeLoaderConfigFieldValidationTest() { }

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadFullConfiguration_LoadShouldBePositive) {

    bool is_loaded = loader.load(config);

    ASSERT_TRUE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutAgent_LoadShouldBeNegative) {
    config["agent"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutAgentVendor_LoadShouldBeNegative) {
    config["agent"]["vendor"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutAgentCapabilities_LoadShouldBeNegative) {
    config["agent"]["capabilities"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutServer_LoadShouldBeNegative) {
    config["server"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutServerPort_LoadShouldBeNegative) {
    config["server"]["port"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutRegistration_LoadShouldBeNegative) {
    config["registration"]= json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutRegistrationIpv4_LoadShouldBeNegative) {
    config["registration"]["ipv4"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutRegistrationPort_LoadShouldBeNegative) {
    config["registration"]["port"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutRegistrationInterval_LoadShouldBeNegative) {
    config["registration"]["interval"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutLogger_LoadShouldBePositive) {
    config["logger"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_TRUE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutManagers_LoadShouldBeNegative) {
    config["managers"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithZeroManagers_LoadShouldBeNegative) {
    config["managers"] = json::Json::value_t::array;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutIpv4_LoadShouldBeNegative) {
    config["managers"].front()["ipv4"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutlocation_LoadShouldBeNegative) {
    config["managers"].front()["locationOffset"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutParent_LoadShouldBeNegative) {
    config["managers"].front()["parentId"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutNicDriver_LoadShouldBeNegative) {
    config["managers"].front()["nic-drivers"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}
