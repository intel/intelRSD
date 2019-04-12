/*!
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
 * */

#include "utils.hpp"
#include "psme/rest/endpoints/utils.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace agent_framework::module;
using namespace agent_framework::model;

class QueryEntriesTest : public ::testing::Test {
public:

    void SetUp() {
        ::generate_model_data();
    }

    void TearDown() {
        ::clean_model_data();
    }

    ~QueryEntriesTest();
};

QueryEntriesTest::~QueryEntriesTest() {}

TEST_F(QueryEntriesTest, SuccessfullQuery) {
    auto get_cores = [](const Processor& proc){return proc.get_total_cores();};
    auto proc_cores = endpoint::utils::query_entries<Processor, std::uint32_t>("2111", get_cores);
    auto result = std::vector<std::uint32_t>{8,8,8};
    ASSERT_EQ(proc_cores, result);

    auto get_usb_devices_count = [](const System& s){return s.get_usb_devices().get_array().size();};
    int total_total_usb_devices{0};
    for(auto n : endpoint::utils::query_entries<System, int>("1111", get_usb_devices_count)){
        total_total_usb_devices += n;
    }
    ASSERT_EQ(total_total_usb_devices, 4);
}
