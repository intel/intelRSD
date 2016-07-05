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

#include "psme/rest-ref/endpoints/compute_module_collection.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/server/request.hpp"
#include "psme/rest-ref/mapper/mapper.hpp"
#include "psme/rest-ref/cache/cache.hpp"
#include "json/json.hpp"
#include "utils.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace psme::rest;
using namespace app::rest::constants;
using namespace psme::rest::endpoint;
using namespace agent_framework::module;

class ComputeModuleCollectionTest : public ::testing::Test {
public:
    Cache m_cache{};
    Mapper m_mapper{m_cache};
    ComputeModuleCollection m_obj{m_mapper};

    void SetUp() {
        ::generate_model_data(m_mapper);
    }

    void TearDown() {
        ::clean_model_data();
    }

    ~ComputeModuleCollectionTest();
};

ComputeModuleCollectionTest::~ComputeModuleCollectionTest() {}

TEST_F(ComputeModuleCollectionTest, GetRequestTest) {
    server::Request req;
    server::Response res;
    req.set_destination("/redfish/v1/Drawers/1/ComputeModules");

    m_obj.get(res, req);

    json::Value json;
    json::Deserializer(res.to_buffer()) >> json;

    ASSERT_EQ(json[Common::ODATA_ID].as_string(), req.get_url());
    ASSERT_EQ(json[Common::LINKS][Collection::ODATA_COUNT].as_uint(), 3);

    std::size_t index = 0;
    for (const auto& link : json[Common::LINKS][Collection::MEMBERS]) {
        ASSERT_EQ(link[Common::ODATA_ID].as_string(),
                "/redfish/v1/Drawers/1/ComputeModules/" + std::to_string(++index));
    }
}
