/*!
 * @brief Path multiplexer tests
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file multiplexer_test.cpp
 */

#include "psme/rest/server/multiplexer.hpp"
#include "psme/rest/constants/routes.hpp"
#include "psme/rest/constants/constants.hpp"

#include "gtest/gtest.h"



using namespace testing;
using namespace psme::rest::constants;

namespace psme {
namespace rest {
namespace server {

class TestEndpoint : public MethodsHandler {
public:
    explicit TestEndpoint(const std::string& path) : MethodsHandler(path) {}


    ~TestEndpoint();


    virtual void get(const Request& /* request */, Response& /*  response */) override {}


    virtual void patch(const Request& /* request */, Response& /*  response */) override {}


    virtual void post(const Request& /* request */, Response& /*  response */) override {}


    virtual void put(const Request& /* request */, Response& /*  response */) override {}


    virtual void del(const Request& /* request */, Response& /*  response */) override {}
};


TestEndpoint::~TestEndpoint() {}


class MultiplexerTest : public Test {
public:
    MultiplexerTest() {
        m_multiplexer.register_handler(TestEndpoint::UPtr(new TestEndpoint(Routes::REDFISH_PATH)));
        m_multiplexer.register_handler(TestEndpoint::UPtr(new TestEndpoint(Routes::ROOT_PATH)));
        m_multiplexer.register_handler(TestEndpoint::UPtr(new TestEndpoint(Routes::STORAGE_SERVICES_COLLECTION_PATH)));
        m_multiplexer.register_handler(TestEndpoint::UPtr(new TestEndpoint(Routes::STORAGE_SERVICE_PATH)));
        m_multiplexer.register_handler(TestEndpoint::UPtr(new TestEndpoint(Routes::VOLUME_COLLECTION_PATH)));
        m_multiplexer.register_handler(TestEndpoint::UPtr(new TestEndpoint(Routes::VOLUME_PATH)));
    }


    ~MultiplexerTest();


    Multiplexer m_multiplexer{};
};


MultiplexerTest::~MultiplexerTest() {}


TEST_F(MultiplexerTest, AllowableURL) {
    ASSERT_TRUE(m_multiplexer.is_correct_endpoint_url("/redfish"));
    ASSERT_TRUE(m_multiplexer.is_correct_endpoint_url("/redfish/v1"));
    ASSERT_TRUE(m_multiplexer.is_correct_endpoint_url("/redfish/v1/StorageServices"));
    ASSERT_TRUE(m_multiplexer.is_correct_endpoint_url("/redfish/v1/StorageServices/100"));
    ASSERT_TRUE(m_multiplexer.is_correct_endpoint_url("/redfish/v1/StorageServices/100/Volumes"));
    ASSERT_TRUE(m_multiplexer.is_correct_endpoint_url("/redfish/v1/StorageServices/100/Volumes/500"));
}


TEST_F(MultiplexerTest, NotAllowableURL) {
    ASSERT_FALSE(m_multiplexer.is_correct_endpoint_url("/deadfish/v1"));
    ASSERT_FALSE(m_multiplexer.is_correct_endpoint_url("/redfish/v2"));
    ASSERT_FALSE(m_multiplexer.is_correct_endpoint_url("/redfish/v1/Service"));
    ASSERT_FALSE(m_multiplexer.is_correct_endpoint_url("/redfish/v1/Servicess"));
    ASSERT_FALSE(m_multiplexer.is_correct_endpoint_url("/redfish/v1/Services/*"));
    ASSERT_FALSE(m_multiplexer.is_correct_endpoint_url("/redfish/v1/Services/-12"));
    ASSERT_FALSE(m_multiplexer.is_correct_endpoint_url("/redfish/v1/Services/10^10"));
    ASSERT_FALSE(m_multiplexer.is_correct_endpoint_url("/redfish/v1/......"));
    ASSERT_FALSE(m_multiplexer.is_correct_endpoint_url("/redfish/v1/żółć"));
    ASSERT_FALSE(m_multiplexer.is_correct_endpoint_url("/redfish/v1/Services/1/2"));
    ASSERT_FALSE(m_multiplexer.is_correct_endpoint_url("/redfish/v1/Services/Volumes/100"));
    ASSERT_FALSE(m_multiplexer.is_correct_endpoint_url("/redfish/v1/Volumes/500/Services/100"));
    ASSERT_FALSE(m_multiplexer.is_correct_endpoint_url("/redfish/v1/Services/100/Volumes/500/Volumes/500"));
}


TEST_F(MultiplexerTest, TestGetParams) {
    const auto path1 = "/redfish/v1/StorageServices/3";
    const auto output1 = m_multiplexer.get_params(path1, constants::Routes::STORAGE_SERVICE_PATH);

    ASSERT_EQ(output1[PathParam::SERVICE_ID], "3");

    const auto path2 = "/redfish/v1/StorageServices/2/Volumes/4";
    const auto output2 = m_multiplexer.get_params(path2, constants::Routes::VOLUME_PATH);

    ASSERT_EQ(output2[PathParam::VOLUME_ID], "4");
    ASSERT_EQ(output2[PathParam::SERVICE_ID], "2");

    std::vector<std::pair<std::string, std::string>> wrong_path_cases = {
        std::make_pair("/redfish/v1/Volumes/4/Services/2", constants::Routes::VOLUME_PATH),
        std::make_pair("/redfish/v1/Services/2/Volumes/4", constants::Routes::STORAGE_SERVICE_PATH),
        std::make_pair("/redfish/v1/Services/2", constants::Routes::VOLUME_PATH),
        std::make_pair("/redfish/v1/Services/abc", constants::Routes::STORAGE_SERVICE_PATH),
        std::make_pair("/redfish/v1/Servicess/2", constants::Routes::STORAGE_SERVICE_PATH),
    };
    for (const auto& wrong_path : wrong_path_cases) {
        ASSERT_THROW(m_multiplexer.get_params(wrong_path.first, wrong_path.second),
                     agent_framework::exceptions::InvalidValue);
    }
}


TEST_F(MultiplexerTest, TestGetParamsGamiNothrow) {
    const auto path1 = "/redfish/v1/StorageServices/3";
    const auto output1 = m_multiplexer.try_get_params(path1, constants::Routes::STORAGE_SERVICE_PATH);

    ASSERT_EQ(output1[PathParam::SERVICE_ID], "3");

    const auto path2 = "/redfish/v1/StorageServices/2/Volumes/4";
    const auto output2 = m_multiplexer.try_get_params(path2, constants::Routes::VOLUME_PATH);

    ASSERT_EQ(output2[PathParam::VOLUME_ID], "4");
    ASSERT_EQ(output2[PathParam::SERVICE_ID], "2");

    std::vector<std::pair<std::string, std::string>> wrong_path_cases = {
        std::make_pair("/redfish/v1/Volumes/4/Services/2", constants::Routes::VOLUME_PATH),
        std::make_pair("/redfish/v1/Services/2/Volumes/4", constants::Routes::STORAGE_SERVICE_PATH),
        std::make_pair("/redfish/v1/Services/2", constants::Routes::VOLUME_PATH),
        std::make_pair("/redfish/v1/Services/abc", constants::Routes::STORAGE_SERVICE_PATH),
        std::make_pair("/redfish/v1/Servicess/2", constants::Routes::STORAGE_SERVICE_PATH),
    };
    Parameters params;
    for (const auto& wrong_path : wrong_path_cases) {
        ASSERT_NO_THROW(params = m_multiplexer.try_get_params(wrong_path.first, wrong_path.second));
        ASSERT_TRUE(params.begin() == params.end());
    }
}


TEST_F(MultiplexerTest, TestBadUrlTemplate) {
    const auto path = "/redfish/v1/Services/3";
    const auto path_template = "/redfish/v1/StorageServices/{storageServiceId:[0-9]+}";

    ASSERT_THROW(m_multiplexer.get_params(path, path_template), std::logic_error);
}

}
}
}
