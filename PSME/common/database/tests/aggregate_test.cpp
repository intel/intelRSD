/*!
 * @brief Test for PersistentAttributes aggregate type.
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
 * @file aggregate_test.cpp
 */

#include <gmock/gmock.h>
#include "database/aggregate.hpp"
#include "database/persistent_attributes.hpp"

#include <iostream>

using namespace testing;

namespace {

class AggregateTest: public Test {
public:
    virtual ~AggregateTest() = default;
};

class MockPersistentAttributes {
public:
    using UuidAttrPair = std::pair<std::string, std::string>;
    using UuidAttrVector = std::vector<UuidAttrPair>;

    MOCK_METHOD3(get_value, bool(const std::string&, const std::string&, std::string&));
    MOCK_METHOD3(set_value, void(const std::string&, const std::string&, const std::string&));
    MOCK_METHOD0(get_persistent_settings, UuidAttrVector());
};

}

namespace database {

TEST_F(AggregateTest, VersionTest) {
    MockPersistentAttributes pa;
    EXPECT_CALL(pa, get_value("uuid", "name", _)).WillOnce(DoAll(SetArgReferee<2>("1.1"), Return(true))).RetiresOnSaturation();
    auto create = [&]() {Aggregate<MockPersistentAttributes> aggr {pa, "uuid", "name"};};
    EXPECT_THROW(create(), std::runtime_error);

    EXPECT_CALL(pa, get_value("uuid", "name", _)).WillOnce(DoAll(SetArgReferee<2>("0.1"), Return(true))).RetiresOnSaturation();
    EXPECT_THROW(create(), std::runtime_error);

    EXPECT_CALL(pa, get_value("uuid", "name", _)).WillOnce(DoAll(SetArgReferee<2>("0.0"), Return(true))).RetiresOnSaturation();
    EXPECT_THROW(create(), std::runtime_error);

}

TEST_F(AggregateTest, ParsingTest) {
    MockPersistentAttributes pa;
    EXPECT_CALL(pa, get_value("uuid", "name", _)).WillOnce(
        DoAll(SetArgReferee<2>("1.0\037key1=value1\037key2=value2\037MULTIPLE=aggregate\037"
            "aggregate1=value3\037aggregate_2=value4\037MULTIPLE=composite"), Return(true)));
    Aggregate<MockPersistentAttributes> aggr{pa, "uuid", "name"};

    EXPECT_STREQ("value1", aggr.get("key1").c_str());
    EXPECT_STREQ("value2", aggr.get("key2").c_str());
    EXPECT_THROW(aggr.get("key3"), std::out_of_range);
    EXPECT_THROW(aggr.get("aggregate"), std::out_of_range);
    EXPECT_THROW(aggr.get("composite"), std::out_of_range);
}

TEST_F(AggregateTest, DumpTest) {
    MockPersistentAttributes pa;
    EXPECT_CALL(pa, get_value("uuid", "name", _)).WillOnce(
        DoAll(SetArgReferee<2>("1.0\037key1=valu=e1\037key2=value2\037MULTIPLE=aggregate\037"
            "aggregate1=value3\037aggregate_2=value4"), Return(true))).RetiresOnSaturation();
    Aggregate<MockPersistentAttributes> aggr{pa, "uuid", "name"};

    EXPECT_STREQ("valu=e1", aggr.get("key1").c_str());
    EXPECT_STREQ("value2", aggr.get("key2").c_str());
    aggr.put("manual", "mvalue");
    aggr.put(std::initializer_list<Aggregate<MockPersistentAttributes>::SinglesContainer::value_type>{{"listk1",
        "listval1"}, {"listk2", "listval2"}});
    aggr.append("custom_bucket", "cb=1");
    aggr.append("custom_bucket", "cb2");

    // negative tests
    EXPECT_THROW(aggr.append("MULTIPLE", "foo"), std::runtime_error);
    EXPECT_THROW(aggr.append("asMULTIPLEdf", "foo"), std::runtime_error);
    EXPECT_THROW(aggr.append("=MULTIPLEdf", "foo"), std::runtime_error);
    EXPECT_THROW(aggr.put("MULTIPLE", "mvalue"), std::runtime_error);
    EXPECT_THROW(aggr.put("invalid_key=", "mvalue"), std::runtime_error);
    EXPECT_THROW(aggr.put(std::initializer_list<Aggregate<MockPersistentAttributes>::SinglesContainer::value_type>{{
        "MULT", "listval1"}, {"dfMULTIPLEas", "listval2"}}), std::runtime_error);

    std::string out;
    EXPECT_CALL(pa, set_value("uuid", "name", _)).WillOnce(SaveArg<2>(&out));
    aggr.dump();

    // dump, reparse and check values
    EXPECT_CALL(pa, get_value("uuid", "name", _)).WillOnce(DoAll(SetArgReferee<2>(out), Return(true)));
    Aggregate<MockPersistentAttributes> aggr2{pa, "uuid", "name"};

    EXPECT_STREQ("valu=e1", aggr.get("key1").c_str());
    EXPECT_STREQ("value2", aggr.get("key2").c_str());
    EXPECT_STREQ("mvalue", aggr.get("manual").c_str());
    EXPECT_STREQ("listval1", aggr.get("listk1").c_str());
    EXPECT_STREQ("listval2", aggr.get("listk2").c_str());
    EXPECT_THROW(aggr.get("custom_bucket_2"), std::out_of_range);
}

TEST_F(AggregateTest, GetAggregatesTest) {
    MockPersistentAttributes pa;
    MockPersistentAttributes::UuidAttrVector attrVec;
    attrVec.emplace_back("uuid", "name");
    attrVec.emplace_back("uuid2", "name");
    attrVec.emplace_back("uuid3", "name1");

    EXPECT_CALL(pa, get_value("uuid", "name", _)).WillOnce(
        DoAll(SetArgReferee<2>("1.0\037key1=valu=e1\037key2=value2\037MULTIPLE=aggregate\037"
            "aggregate1=value3\037aggregate_2=value4"), Return(true))).RetiresOnSaturation();
    EXPECT_CALL(pa, get_value("uuid2", "name", _)).WillOnce(
        DoAll(SetArgReferee<2>("1.0\037key1=valu=e1\037key2=value2\037MULTIPLE=aggregate\037"
            "aggregate1=value3\037aggregate_2=value4"), Return(true))).RetiresOnSaturation();
    EXPECT_CALL(pa, get_persistent_settings()).WillOnce(Return(attrVec));
    auto ret = Aggregate<MockPersistentAttributes>::get_aggregates(pa, "name");
    EXPECT_EQ(2, ret.size());
    for (const auto& entry : ret) {
        EXPECT_STREQ("name", entry.get_name().c_str());
    }
}

TEST_F(AggregateTest, MultipleRetrievalTest) {
    MockPersistentAttributes pa;
    EXPECT_CALL(pa, get_value("uuid", "name", _)).WillOnce(
        DoAll(SetArgReferee<2>("1.0\037key1=valu=e1\037key2=value2\037MULTIPLE=aggregate\037"
            "aggregate1=value3\037aggregate_2=value4"), Return(true))).RetiresOnSaturation();
    Aggregate<MockPersistentAttributes> aggr{pa, "uuid", "name"};
    aggr.put("manual", "mvalue");
    aggr.put(std::initializer_list<Aggregate<MockPersistentAttributes>::SinglesContainer::value_type>{{"listk1",
        "listval1"}, {"listk2", "listval2"}});
    aggr.append("custom_bucket", "cb=1");
    aggr.append("custom_bucket", "cb2");

    auto values = aggr.get_multiple_values("custom_bucket");
    EXPECT_EQ(2, values.size());
    EXPECT_TRUE(std::find(std::begin(values), std::end(values), "cb=1") != std::end(values));
    EXPECT_TRUE(std::find(std::begin(values), std::end(values), "cb2") != std::end(values));

    // delete by value for multiples
    EXPECT_EQ(1, aggr.del("cb=1"));
    EXPECT_EQ(1, aggr.del("cb2"));
    values = aggr.get_multiple_values("custom_bucket");
    EXPECT_EQ(0, values.size());

    auto no_values = aggr.get_multiple_values("invalid_bucked");
    EXPECT_EQ(0, no_values.size());
}

}
