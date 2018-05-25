/*!
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file jsonptr/tests/jsonptr_test.cpp
 */

#include "jsonptr/jsonptr.hpp"
#include <gtest/gtest.h>
#include <json/value.hpp>
#include <json/serializer.hpp>
#include <mutex>
#include <iostream>

using namespace jsonptr;

namespace json {
/*!
 * @brief These printing utils are needed by googletest to print error messages if tests fail
 */
std::ostream& operator<<(std::ostream& os, const Value& json) {
    Serializer serializer(json);
    return os << serializer;
}
std::ostream& operator<<(std::ostream& os, const Value::Type& type) {
    return os << int(type);
}
void PrintTo(const Value& json, std::ostream* os) {
    Serializer serializer(json);
    *os << serializer;
}
void PrintTo(const Value::Type& type, std::ostream* os) {
    *os << int(type);
}
}

/*!
 * @brief Initialization and getter for JSON object used in all tests for jsonrpc::find and jsonrpc::set_at
 */
const json::Value& get_object() {
    static bool initialized{false};
    static json::Value json = json::Value::Type::OBJECT;
    if (!initialized) {
        initialized = true;
        json["foo"] = json::Value::Type::ARRAY;
        json["foo"].push_back("bar");
        json["foo"].push_back("baz");
        json[""] = 0;
        json["a/b"] = 1;
        json["c%d"] = 2;
        json["e^f"] = 3;
        json["g|h"] = 4;
        json["i\\j"] = 5;
        json["k\"l"] = 6;
        json[" "] = 7;
        json["m~n"] = 8;
        json["0"] = 9;
        json["123"] = 10;
        json["18446744073709551616"] = 11;
        json["newKey"] = json::Value::Type::NIL;
        json["dummy"]["value"] = json::Value::Type::ARRAY;
        json::Value object;
        object["deep"] = 3;
        object["blue"] = "haze";
        json["dummy"]["value"].push_back(std::move(object));
        json["dummy"]["value"].push_back("22");
        json["dummy"]["value"].push_back(22);
        json::Value array = json::Value::Type::ARRAY;
        array.push_back("5");
        json["dummy"]["value"].push_back(array);
    };
    return json;
}

/*!
 * @brief Get copy of the JSON object
 */
json::Value get_copy() {
    return get_object();
}


/*****************/
/*   Test 1      */
/*****************/

struct SuccessfulFindTestCase {
    std::string jsonptr;
    const json::Value& expected_object;
};

static std::vector<SuccessfulFindTestCase> successful_test_cases = {
    {"", get_object()},
    {"/", get_object()[""]},
    {"/ ", get_object()[" "]},
    {"/0", get_object()["0"]},
    {"/123", get_object()["123"]},
    {"/18446744073709551616", get_object()["18446744073709551616"]},
    {"/a~1b", get_object()["a/b"]},
    {"/c%d", get_object()["c%d"]},
    {"/e^f", get_object()["e^f"]},
    {"/g|h", get_object()["g|h"]},
    {"/i\\j", get_object()["i\\j"]},
    {"/k\"l", get_object()["k\"l"]},
    {"/m~0n", get_object()["m~n"]},
    {"/foo", get_object()["foo"]},
    {"/foo/0", get_object()["foo"][0]},
    {"/foo/1", get_object()["foo"][1]},
    {"/dummy/value", get_object()["dummy"]["value"]},
    {"/dummy/value/0", get_object()["dummy"]["value"][0]},
    {"/dummy/value/0/deep", get_object()["dummy"]["value"][0]["deep"]},
    {"/dummy/value/1", get_object()["dummy"]["value"][1]},
    {"/dummy/value/2", get_object()["dummy"]["value"][2]},
    {"/dummy/value/3/0", get_object()["dummy"]["value"][3][0]}
};

class SuccessfulFindTest : public ::testing::Test, public ::testing::WithParamInterface<SuccessfulFindTestCase> {
protected:
    virtual ~SuccessfulFindTest();
};

SuccessfulFindTest::~SuccessfulFindTest() { }

TEST_P(SuccessfulFindTest, FindTest) {
    const auto test_case = GetParam();
    ASSERT_EQ(jsonptr::find(get_object(), test_case.jsonptr), test_case.expected_object);
}

INSTANTIATE_TEST_CASE_P(Default, SuccessfulFindTest, ::testing::ValuesIn(successful_test_cases));

/*****************/
/*   Test 2      */
/*****************/

struct NonexistentValueTestCase {
    std::string jsonptr;
    std::string message;
};

static std::vector<NonexistentValueTestCase> nonexistent_value_test_cases {
    {"/bar", "Nonexistent value in JSON: Did not find key bar in JSON object."},
    {"/a~0b", "Nonexistent value in JSON: Did not find key a~b in JSON object."},
    {"/m~1n", "Nonexistent value in JSON: Did not find key m/n in JSON object."},
    {"/dummy/", "Nonexistent value in JSON: Did not find key  in JSON object."},
    {"/foo/1/more", "Nonexistent value in JSON: Could not access key more because JSON value is not an object."},
    {"/foo/2", "Nonexistent value in JSON: JSON array does not contain element at index 2."}
};


class NonexistentValueTest : public ::testing::Test, public ::testing::WithParamInterface<NonexistentValueTestCase> {
protected:
    virtual ~NonexistentValueTest();
};

NonexistentValueTest::~NonexistentValueTest() { }

TEST_P(NonexistentValueTest, NegativeTest) {
    const auto test_case = GetParam();
    ASSERT_THROW(jsonptr::find(get_object(), test_case.jsonptr), NonexistentValueError);
    try {
        jsonptr::find(get_object(), test_case.jsonptr);
    } catch(NonexistentValueError& ex) {
        ASSERT_EQ(test_case.message, ex.what());
    }
}

INSTANTIATE_TEST_CASE_P(Default, NonexistentValueTest, ::testing::ValuesIn(nonexistent_value_test_cases));


/*****************/
/*   Test 3      */
/*****************/

struct InvalidPtrTestCase {
    std::string jsonptr;
    std::string message;
};

static std::vector<InvalidPtrTestCase> invalid_ptr_test_cases {
    {"foo ", "Invalid pointer: JSON pointer should contain no text before the first '/'."},
    {"/a~", "Invalid pointer: The escape character '~' can only be used in sequences '~0' and '~1'."},
    {"/a~b", "Invalid pointer: The escape character '~' can only be used in sequences '~0' and '~1'."},
    {"/a~2b", "Invalid pointer: The escape character '~' can only be used in sequences '~0' and '~1'."},
    {"/foo/1 ", "Invalid pointer: Index in JSON array can only contain digits."},
    {"/foo/ 1", "Invalid pointer: Index in JSON array can only contain digits."},
    {"/foo/01", "Invalid pointer: No leading zeros are allowed in an index array."},
    {"/foo/-", "Invalid pointer: Pointing at the end of an array is not supported."},
    {"/foo/18446744073709551616", "Invalid pointer: JSON array index out of range."}
};


class InvalidPtrTest : public ::testing::Test, public ::testing::WithParamInterface<InvalidPtrTestCase> {
protected:
    virtual ~InvalidPtrTest();
};

InvalidPtrTest::~InvalidPtrTest() { }

TEST_P(InvalidPtrTest, NegativeTest) {
    const auto test_case = GetParam();
    ASSERT_THROW(jsonptr::find(get_object(), test_case.jsonptr), InvalidPointerError);
    try {
        jsonptr::find(get_object(), test_case.jsonptr);
    } catch(InvalidPointerError& ex) {
        ASSERT_EQ(test_case.message, ex.what());
    }
}

INSTANTIATE_TEST_CASE_P(Default, InvalidPtrTest, ::testing::ValuesIn(invalid_ptr_test_cases));


/*****************/
/*   Test 4      */
/*****************/

class SuccessfulSetAtTest : public ::testing::Test {
protected:
    virtual ~SuccessfulSetAtTest();
};

SuccessfulSetAtTest::~SuccessfulSetAtTest() { }

TEST_F(SuccessfulSetAtTest, SetWholeObjectTest) {
    // set whole object to 3
    const auto jsonptr = "";
    json::Value to_modify = get_copy();
    json::Value value_to_set{3};
    ASSERT_NO_THROW(jsonptr::set_at(to_modify, jsonptr, value_to_set));

    json::Value expected{3};
    ASSERT_EQ(to_modify, expected);
}

TEST_F(SuccessfulSetAtTest, SetAtEmptyKeyTest) {
    // set object value for key ""
    const auto jsonptr = "/";
    json::Value to_modify = get_copy();
    json::Value value_to_set = "pallo";
    ASSERT_NO_THROW(jsonptr::set_at(to_modify, jsonptr, value_to_set));

    json::Value expected = get_copy();
    expected[""] = "pallo";
    ASSERT_EQ(to_modify, expected);
}

TEST_F(SuccessfulSetAtTest, ReplaceNullWithObjectTest) {
    // set object value for key "newKey"
    const auto jsonptr = "/newKey";
    json::Value to_modify = get_copy();
    json::Value value_to_set = json::Value::Type::OBJECT;
    value_to_set["sample"] = "random";
    value_to_set["scratch"] = 333;
    ASSERT_NO_THROW(jsonptr::set_at(to_modify, jsonptr, value_to_set));

    json::Value expected = get_copy();
    expected["newKey"]["sample"] = "random";
    expected["newKey"]["scratch"] = 333;
    ASSERT_EQ(to_modify, expected);
}

TEST_F(SuccessfulSetAtTest, SetDeeplyNestedTest) {
    // set object value for array member
    const auto jsonptr = "/dummy/value/0/deep";
    json::Value to_modify = get_copy();
    json::Value value_to_set = 500;
    ASSERT_NO_THROW(jsonptr::set_at(to_modify, jsonptr, value_to_set));

    json::Value expected = get_copy();
    expected["dummy"]["value"][0]["deep"] = 500;
    ASSERT_EQ(to_modify, expected);
}

TEST_F(SuccessfulSetAtTest, SetAtEscapedKeyTest) {
    // set object value at key that needs escaping
    const auto jsonptr = "/a~1b";
    json::Value to_modify = get_copy();
    json::Value value_to_set = true;
    ASSERT_NO_THROW(jsonptr::set_at(to_modify, jsonptr, value_to_set));

    json::Value expected = get_copy();
    expected["a/b"] = true;
    ASSERT_EQ(to_modify, expected);
}

TEST_F(SuccessfulSetAtTest, ReplaceNumberWithObjectTest) {
    // replace number with object
    const auto jsonptr = "/e^f";
    json::Value to_modify = get_copy();
    json::Value value_to_set = json::Value::Type::OBJECT;
    value_to_set["sample"] = "random";
    value_to_set["scratch"] = 333;
    ASSERT_NO_THROW(jsonptr::set_at(to_modify, jsonptr, value_to_set));

    json::Value expected = get_copy();
    expected["e^f"] = value_to_set;
    ASSERT_EQ(to_modify, expected);
}
