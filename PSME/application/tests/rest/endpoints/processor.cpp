/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "psme/rest/endpoints/processor.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/server/request.hpp"
#include "psme/rest/server/response.hpp"
#include "psme/rest/mapper/mapper.hpp"
#include "psme/rest/cache/cache.hpp"
#include "json/json.hpp"
#include "utils.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace app::rest;

class ProcessorTest : public ::testing::Test {
public:
    Cache m_cache{};
    Mapper m_mapper{m_cache};
    Processor m_obj{m_mapper};

    void SetUp() {
        ::generate_model_data(m_mapper);
    }

    void TearDown() {
        ::clean_model_data();

    }

    ~ProcessorTest();
};

ProcessorTest::~ProcessorTest() {}

TEST_F(ProcessorTest, GetRequestTest) {
    server::Request req;
    req.set_destination("/redfish/v1/Drawers/1/ComputeModules/1/Blades/1/Processors/1");

    server::Response res;

    m_obj.get(res, req);

    json::Value json;
    json::Deserializer(res.to_buffer()) >> json;

    ASSERT_EQ(json[constants::Common::ODATA_ID].as_string(), req.get_url());
    ASSERT_EQ(json[constants::Common::ID].as_string(), req.params["processorID"]);
    ASSERT_EQ(json[constants::Processor::SOCKET].as_string(), "4");
    ASSERT_EQ(json[constants::Processor::MODEL].as_string(), "Unknown");
    ASSERT_EQ(json[constants::Processor::MODEL_NAME].as_string(),
                        "Multi-Core Intel(R) Xeon(R) processor 7xxx Series");
    ASSERT_EQ(json[constants::Processor::MANUFACTURER].as_string(), "Intel Corporation");
    ASSERT_EQ(json[constants::Processor::ARCHITECTURE].as_string(), "x86");
    ASSERT_EQ(json[constants::Processor::INSTRUCTION_SET].as_string(), "x64");

    ASSERT_EQ(json[constants::Processor::ID]
                  [constants::Processor::VENDOR_ID].as_string(), "GenuineIntel");
    ASSERT_EQ(json[constants::Processor::ID]
                  [constants::Processor::NUMERIC_ID].as_string(), "329442");
    ASSERT_EQ(json[constants::Processor::ID]
                  [constants::Processor::FAMILY].as_string(), "6");
    ASSERT_EQ(json[constants::Processor::ID]
                  [constants::Processor::MODEL].as_string(), "5");
    ASSERT_EQ(json[constants::Processor::ID]
                  [constants::Processor::STEP].as_string(), "2");
    ASSERT_EQ(json[constants::Processor::ID]
                  [constants::Processor::MICROCODE_INFO].as_string(), "11");

    ASSERT_EQ(json[constants::Processor::MAX_SPEED].as_uint(), 3700);
    ASSERT_EQ(json[constants::Processor::TOTAL_CORES].as_uint(), 8);
    ASSERT_EQ(json[constants::Processor::ENABLED_CORES].as_uint(), 4);
    ASSERT_EQ(json[constants::Processor::TOTAL_THREADS].as_uint(), 4);
    ASSERT_EQ(json[constants::Processor::ENABLED_THREADS].as_uint(), 2);

    ASSERT_EQ(json[constants::Common::LINKS]
                  [constants::ComputeModule::CONTAINED_BY]
                  [constants::Common::ODATA_ID].as_string(),
                  "/redfish/v1/Drawers/1/ComputeModules/1/Blades/1");

}

