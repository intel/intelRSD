/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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

#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module-ref/enum/enum_builder.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <cstdio>
#include <string>

using ::testing::Return;
using ::testing::Throw;

using namespace jsonrpc;
using namespace agent_framework::exceptions;


class ProcedureValidatorTest: public ::testing::Test {
protected:
    ProcedureValidatorTest();

    virtual void SetUp();

    virtual void TearDown();

    virtual ~ProcedureValidatorTest();
};

ProcedureValidatorTest::ProcedureValidatorTest() {}

void ProcedureValidatorTest::SetUp() {}

void ProcedureValidatorTest::TearDown() {}

ProcedureValidatorTest::~ProcedureValidatorTest() {}




class Validator {
private:
    const ProcedureValidator* procedure;
    Json::Value value{};
    std::string json;
    bool validated;
    std::string message;
    std::string fields;
public:
    Validator(const ProcedureValidator& _procedure, const std::string& json);

    /* validation methods, reformat message thrown during validation */
    bool valid();
    std::string err_descr();
    std::string wrong_field();

    const Json::Value& operator[](const std::string& field);
};

#define VALIDATOR(name, s, ...) \
    ProcedureValidator p_##name{\
        #name,\
        PARAMS_BY_NAME,\
        __VA_ARGS__\
    };\
    Validator name{p_##name, s}


Validator::Validator(const ProcedureValidator& _procedure, const std::string& _json):
    procedure(&_procedure),
    json(_json),
    validated(false),
    message(""),
    fields("")
{}

bool Validator::valid() {
    if (!validated) {
        try {
            static Json::Reader reader{};

            validated = true;
            message = "Cannot parse document";
            reader.parse(json, value);
            message = "Cannot validate document";
            procedure->validate(value);
            message = "";
        } catch (InvalidField fe) {
            message = fe.get_message();
            fields = fe.get_path();
        } catch (InvalidParameters px) {
            message = px.get_message();
            fields = "";
        }
    }
    return message.empty();
}

std::string Validator::wrong_field()  {
    if (valid()) {
        return "";
    }
    return fields;
}

const Json::Value& Validator::operator[](const std::string& field) {
    return value[field];
}


TEST_F(ProcedureValidatorTest, NullDocument) {
    constexpr const char nullDoc[] = "null";

    VALIDATOR(nothing, nullDoc,
        nullptr
    );
    ASSERT_TRUE(nothing.valid());

    VALIDATOR(optional, nullDoc,
        "field", VALID_OPTIONAL(VALID_JSON_STRING),
        nullptr
    );
    ASSERT_TRUE(optional.valid());

    VALIDATOR(not_optional, nullDoc,
        "field", VALID_JSON_STRING,
        nullptr
    );
    ASSERT_FALSE(not_optional.valid());
    ASSERT_EQ("field", not_optional.wrong_field());

    VALIDATOR(mixed, nullDoc,
        "optional", VALID_OPTIONAL(VALID_JSON_STRING),
        "field", VALID_JSON_STRING,
        "trailing", VALID_JSON_ANY,
        nullptr
    );
    ASSERT_FALSE(mixed.valid());
    ASSERT_EQ("field", mixed.wrong_field());
}

TEST_F(ProcedureValidatorTest, ValidationOrder) {
    VALIDATOR(check_order,
        R"({
            "zzz": 1,
            "aaa": 0
        })",
        "ccc", VALID_ALWAYS,
        "aaa", VALID_NEVER,
        "zzz", VALID_NEVER,
        nullptr
    );
    ASSERT_FALSE(check_order.valid());
    ASSERT_EQ("aaa", check_order.wrong_field());
}

TEST_F(ProcedureValidatorTest, DumbFields) {
    VALIDATOR(never_valid1,
        R"({
            "field": "invalid"
        })",
        "field", VALID_NEVER,
        nullptr
    );
    ASSERT_FALSE(never_valid1.valid());

    VALIDATOR(never_valid3,
        R"({
            "string": "invalid",
            "null": null,
            "array": [],
            "object": {
                "obj_field": true
            }
        })",
        "string", VALID_ALWAYS,
        "null", VALID_ALWAYS,
        "array", VALID_ALWAYS,
        "object", VALID_ALWAYS, // obj_field internally is not validated at all!
        "missing", VALID_ALWAYS,
        nullptr
    );
    ASSERT_TRUE(never_valid3.valid());
}

TEST_F(ProcedureValidatorTest, NumberOfFields) {
    // very tricky case, if two fields with same name are in the document
    // It shouldn't validate.. but it does. There's no workaround for it.
    VALIDATOR(duplicates,
        R"({
            "field": 1.2,
            "field": 1.3
        })",
        "field", VALID_NUMERIC_TYPED(DOUBLE),
        nullptr
    );
    EXPECT_TRUE(duplicates.valid());
}

TEST_F(ProcedureValidatorTest, JsonrpcNubers) {
    VALIDATOR(proper_number,
        R"({
            "integer": 1,
            "float": 1.1,
            "number1": 1.3,
            "number2": 1,
            "zero": 0,
            "one": 1
        })",
        "integer", VALID_JSON_INTEGER,
        "float", VALID_JSON_REAL,
        "number1", VALID_NUMERIC,
        "number2", VALID_NUMERIC,
        "zero", VALID_NUMERIC,
        "one", VALID_NUMERIC,
        nullptr
    );
    ASSERT_TRUE(proper_number.valid());

    VALIDATOR(float_as_int,
        R"({
            "integer": 1.1,
        })",
        "integer", VALID_JSON_INTEGER,
        nullptr
    );
    ASSERT_FALSE(float_as_int.valid());

    // real MUST have '.' or 'e' phrase
    VALIDATOR(int_as_real,
        R"({
            "float": 1,
        })",
        "float", VALID_JSON_REAL,
        nullptr
    );
    ASSERT_FALSE(int_as_real.valid());

    VALIDATOR(proper_int,
        R"({
            "simple": 231,
            "zero": 0232,
            "dot": 233.0,
            "exp": 234e0,
            "dot_exp": 2.35e2
        })",
        "simple", VALID_NUMERIC,
        "zero", VALID_NUMERIC,
        "dot", VALID_NUMERIC,
        "exp", VALID_NUMERIC,
        "dot_exp", VALID_NUMERIC,
        nullptr
    );
    ASSERT_TRUE(proper_int.valid());
    ASSERT_EQ(231, proper_int["simple"].asInt());
    ASSERT_EQ(232, proper_int["zero"].asInt());
    ASSERT_EQ(233, proper_int["dot"].asInt());
    ASSERT_EQ(234, proper_int["exp"].asInt());
    ASSERT_EQ(235, proper_int["dot_exp"].asInt());

    VALIDATOR(null_val,
        R"({
            "nulled": null
        })",
        "nulled", VALID_NUMERIC,
        nullptr
    );
    ASSERT_FALSE(null_val.valid());
    ASSERT_EQ(0, null_val["nulled"].asInt());

    VALIDATOR(false_val,
        R"({
            "false": false
        })",
        "false", VALID_NUMERIC,
        nullptr
    );
    ASSERT_FALSE(false_val.valid());
    ASSERT_EQ(0, false_val["false"].asInt());

    VALIDATOR(true_val,
        R"({
            "true": true
        })",
        "true", VALID_NUMERIC,
        nullptr
    );
    ASSERT_FALSE(true_val.valid());
    ASSERT_EQ(1, true_val["true"].asInt());


    VALIDATOR(string_val,
        R"({
            "string": "12"
        })",
        "string", VALID_NUMERIC,
        nullptr
    );
    ASSERT_FALSE(string_val.valid());
    ASSERT_ANY_THROW(string_val["string"].asInt());
}

TEST_F(ProcedureValidatorTest, TypedNumbers) {
    VALIDATOR(int32,
        R"({
            "int": -10,
            "negative": -1,
            "natural": 0,
            "range": 5
        })",
        "int", VALID_NUMERIC_TYPED(INT32),
        "negative", VALID_NUMERIC_EQLT(INT32, -1),
        "natural", VALID_NUMERIC_EQGT(INT32, 0),
        "range", VALID_NUMERIC_RANGE(INT32, -5, 5),
        nullptr
    );
    ASSERT_TRUE(int32.valid());

    VALIDATOR(all_types,
        R"({
            "int": -10,
            "uint": 3,
            "double": 12.3
        })",
        "int", VALID_NUMERIC_TYPED(INT32),
        "uint", VALID_NUMERIC_TYPED(UINT32),
        "double", VALID_NUMERIC_TYPED(DOUBLE),
        nullptr
    );
    ASSERT_TRUE(all_types.valid());

    VALIDATOR(conversions,
        R"({
            "just": 10,
            "negative": -10,
            "dot": 12.0,
            "exp": 12e1,
            "dot_exp": 12.3e1
        })",
        "just", VALID_NUMERIC_TYPED(INT32),
        "negative", VALID_NUMERIC_TYPED(INT32),
        "dot", VALID_NUMERIC_TYPED(INT32),
        "exp", VALID_NUMERIC_TYPED(INT32),
        "dot_exp", VALID_NUMERIC_TYPED(INT32),
        nullptr
    );
    ASSERT_TRUE(conversions.valid());
    ASSERT_EQ(10, conversions["just"].asInt());
    ASSERT_EQ(-10, conversions["negative"].asInt());
    ASSERT_EQ(12, conversions["dot"].asInt());
    ASSERT_EQ(120, conversions["exp"].asInt());
    ASSERT_EQ(123, conversions["dot_exp"].asInt());

    VALIDATOR(int_from_double,
        R"({
            "int": 12.3
        })",
        "int", VALID_NUMERIC_TYPED(INT32),
        nullptr
    );
    ASSERT_FALSE(int_from_double.valid());

    VALIDATOR(uint_from_signed,
        R"({
            "zero": 0,
            "uint": -12
        })",
        "uint", VALID_NUMERIC_TYPED(UINT32),
        nullptr
    );
    ASSERT_FALSE(uint_from_signed.valid());
    ASSERT_EQ("uint", uint_from_signed.wrong_field());
}

TEST_F(ProcedureValidatorTest, EqLtNumbers) {
    VALIDATOR(eqlt_less,
        R"({
            "int": -11
        })",
        "int", VALID_NUMERIC_EQLT(INT32, -10),
        nullptr
    );
    ASSERT_TRUE(eqlt_less.valid());

    VALIDATOR(eqlt_equals,
        R"({
            "int": -10
        })",
        "int", VALID_NUMERIC_EQLT(INT32, -10),
        nullptr
    );
    ASSERT_TRUE(eqlt_equals.valid());

    VALIDATOR(eqlt_more,
        R"({
            "int": -9
        })",
        "int", VALID_NUMERIC_EQLT(INT32, -10),
        nullptr
    );
    ASSERT_FALSE(eqlt_more.valid());
}

TEST_F(ProcedureValidatorTest, AndCondition) {
    VALIDATOR(gt_and_lt,
        R"({
            "int": 5
        })",
        "int", VALID_NUMERIC_EQGT(INT32, -10),
        "int", VALID_NUMERIC_EQLT(INT32, 10),
        nullptr
    );
    ASSERT_TRUE(gt_and_lt.valid());

    VALIDATOR(separated_ranges,
            R"({
            "int": 11
        })",
            "int", VALID_NUMERIC_EQGT(INT32, 10),
            "int", VALID_NUMERIC_EQLT(INT32, -10),
            nullptr
    );
    ASSERT_FALSE(separated_ranges.valid());
}

TEST_F(ProcedureValidatorTest, Optionals) {
    VALIDATOR(optionals,
        R"({
            "a": 5,
            "c": null
        })",
        "a", VALID_NUMERIC,
        "b", VALID_OPTIONAL(VALID_NUMERIC),
        "c", VALID_OPTIONAL(VALID_NUMERIC),
        nullptr
    );
    ASSERT_FALSE(optionals.valid());
    ASSERT_EQ("c", optionals.wrong_field());

    VALIDATOR(null_field,
        R"({
            "a": null
            "c": 12
        })",
        "c", VALID_OPTIONAL(VALID_NUMERIC),
        "a", VALID_NULLABLE(VALID_NUMERIC),
        "b", VALID_NULLABLE(VALID_NUMERIC), // invalid: is obligatory
        nullptr
    );
    ASSERT_FALSE(null_field.valid());
    ASSERT_EQ("b", null_field.wrong_field());
}

TEST_F(ProcedureValidatorTest, Uuid) {
    VALIDATOR(valid,
        R"({
            "uuid": "12345678-1234-1234-1234-1234567890ab"
        })",
        "uuid", VALID_UUID,
        nullptr
    );
    ASSERT_TRUE(valid.valid());

    VALIDATOR(mixed,
        R"({
            "uuid": "aAbBcCdD-eEfF-aAbB-cCdD-eEfF01234567"
        })",
        "uuid", VALID_UUID,
        nullptr
    );
    ASSERT_TRUE(mixed.valid());

    VALIDATOR(null_field,
      R"({
          "uuid": null
      })",
      "uuid", VALID_UUID,
      nullptr
    );
    ASSERT_FALSE(null_field.valid());

    VALIDATOR(wrong_len,
        R"({
            "uuid": "12345678-1234-1234-1234-12345678"
        })",
        "uuid", VALID_UUID,
        nullptr
    );
    ASSERT_FALSE(wrong_len.valid());

    VALIDATOR(wrong_digit,
        R"({
            "uuid": "12345678-1234-1234-1234-1234567890ag"
        })",
        "uuid", VALID_UUID,
        nullptr
    );
    ASSERT_FALSE(wrong_digit.valid());

    VALIDATOR(wrong_format,
        R"({
            "uuid": "12345678-1234-1234-12345678-12345678"
        })",
        "uuid", VALID_UUID,
        nullptr
    );
    ASSERT_FALSE(wrong_format.valid());
}

TEST_F(ProcedureValidatorTest, Arrays) {
    VALIDATOR(numbers,
        R"({
            "array": [
                1, 2, 3, 4, 5
             ]
        })",
        "array", VALID_ARRAY_OF(VALID_NUMERIC),
        nullptr
    );
    ASSERT_TRUE(numbers.valid());

    VALIDATOR(not_an_array,
        R"({
            "array": 21
        })",
        "array", VALID_ARRAY_OF(VALID_NUMERIC),
        nullptr
    );
    ASSERT_FALSE(not_an_array.valid());

    VALIDATOR(wrong_types,
        R"({
            "array": [
                1, "2", 3, false, 5
             ]
        })",
        "array", VALID_ARRAY_OF(VALID_NUMERIC),
        nullptr
    );
    ASSERT_FALSE(wrong_types.valid());
    ASSERT_EQ("array[1]", wrong_types.wrong_field());
}

TEST_F(ProcedureValidatorTest, SizedArrays) {
    VALIDATOR(proper_arrays,
        R"({
            "sarray": [
                1, 2, 3, 4, 5
            ],
            "tarray": [
                1, 2, 3, 4, 5
            ]
        })",
        "sarray", VALID_ARRAY_SIZE(1, 5),
        "tarray", VALID_ARRAY_SIZE_OF(VALID_NUMERIC, 5, 5),
        nullptr
    );
    ASSERT_TRUE(proper_arrays.valid());

    VALIDATOR(empty_array,
        R"({
            "non_empty": [
            ]
        })",
        "non_empty", VALID_ARRAY_SIZE(1, 5),
        nullptr
    );
    ASSERT_FALSE(empty_array.valid());

    VALIDATOR(to_short,
            R"({
            "array": [
                1, 2
            ]
        })",
        "array", VALID_ARRAY_SIZE(3, 5),
        nullptr
    );
    ASSERT_FALSE(to_short.valid());

    VALIDATOR(to_long,
        R"({
            "array": [
                1, 2, 3, 4, 5
            ]
        })",
        "array", VALID_ARRAY_SIZE(1, 3),
        nullptr
    );
    ASSERT_FALSE(to_long.valid());

    VALIDATOR(wrong_def,
            R"({
            "array": [
                1, 2, 3, 4, 5
            ]
        })",
            "array", VALID_ARRAY_SIZE(10, 1),
            nullptr
    );
    ASSERT_FALSE(wrong_def.valid());
}


/*! @brief Attribute "interface" implementation */
class AttributeExample {
public:
    static const ProcedureValidator& get_procedure() {
        static ProcedureValidator proc{
            "example",
            PARAMS_BY_NAME,
            "int", VALID_NUMERIC,
            "optional_int", VALID_OPTIONAL(VALID_NUMERIC_TYPED(INT32)),
            "nullable_string", VALID_NULLABLE(VALID_JSON_STRING),
            nullptr
        };
        return proc;
    }
};

TEST_F(ProcedureValidatorTest, Attributes) {
    VALIDATOR(proper_attr,
        R"({
            "field": "example",
            "object": {
                "int": 10,
                "nullable_string": "something"
            }
        })",
        "field", VALID_JSON_STRING,
        "object", VALID_ATTRIBUTE(AttributeExample),
        nullptr
    );
    ASSERT_TRUE(proper_attr.valid());

    VALIDATOR(not_attr,
        R"({
            "field": "example",
            "object": 12
        })",
        "field", VALID_JSON_STRING,
        "object", VALID_ATTRIBUTE(AttributeExample),
        nullptr
    );
    ASSERT_FALSE(not_attr.valid()); // object is numeric
    ASSERT_EQ("object", not_attr.wrong_field());

    VALIDATOR(wrong_attr,
        R"({
            "field": "example",
            "object": {
                "int": 10,
                "nullable_string": 12
            }
        })",
        "field", VALID_JSON_STRING,
        "object", VALID_ATTRIBUTE(AttributeExample),
        nullptr
    );
    ASSERT_FALSE(wrong_attr.valid()); // nullable_string is numeric
    ASSERT_EQ("object.nullable_string", wrong_attr.wrong_field());
}


ENUM(TestEnum, uint32_t, Abc, BAc, CBA, abc);
TEST_F(ProcedureValidatorTest, Enumerations) {
    VALIDATOR(ok,
            R"({
            "e1": "Abc",
            "e2": "BAc",
            "e3": "abc"
        })",
        "e1", VALID_ENUM(TestEnum),
        "e2", VALID_ENUM(TestEnum),
        "e3", VALID_ENUM(TestEnum),
        nullptr
    );
    ASSERT_TRUE(ok.valid());

    VALIDATOR(null_value,
        R"({
            "e1": null
        })",
        "e1", VALID_ENUM(TestEnum),
        nullptr
    );
    ASSERT_FALSE(null_value.valid());

    VALIDATOR(number,
        R"({
            "e1": 12
        })",
        "e1", VALID_ENUM(TestEnum),
        nullptr
    );
    ASSERT_FALSE(number.valid());

    VALIDATOR(any_string,
        R"({
            "e1": "any string"
        })",
        "e1", VALID_ENUM(TestEnum),
        nullptr
    );
    ASSERT_FALSE(any_string.valid());
}
