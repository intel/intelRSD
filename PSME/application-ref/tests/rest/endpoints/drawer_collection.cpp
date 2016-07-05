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

#include "psme/rest-ref/endpoints/drawer_collection.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/server/request.hpp"
#include "psme/rest-ref/mapper/mapper.hpp"
#include "psme/rest-ref/cache/cache.hpp"
#include "json/json.hpp"
#include "utils.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace psme::rest;
using namespace psme::rest::endpoint;

class DrawerCollectionTest : public ::testing::Test {
public:
    Cache m_cache{};
    Mapper m_mapper{m_cache};
    DrawerCollection m_obj{m_mapper};

    void SetUp() {

    }

    void TearDown() {
        ::clean_model_data();

    }

    ~DrawerCollectionTest();
};

DrawerCollectionTest::~DrawerCollectionTest() {}

TEST_F(DrawerCollectionTest, GetRequestTest) {

}

