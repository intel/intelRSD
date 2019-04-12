/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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

#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module/enum/enum_builder.hpp"
#include "agent-framework/module/constants/regular_expressions.hpp"

#include "json-wrapper/json-wrapper.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <cstdio>
#include <string>

using ::testing::Return;
using ::testing::Throw;

using namespace agent_framework::exceptions;
using namespace agent_framework::model::literals::regex;

namespace jsonrpc {

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




/*!
 * @brief Class to validate jsons according the schema
 *
 * Each tester is simple object to check if document is valid. No exception are
 * thrown. Class limits set of allowed operations to minimum and simplifies
 * the code of test cases.
 */
class ValidatorTester {
public:
    ValidatorTester(const ProcedureValidator& _procedure, const char* _json);
    ValidatorTester(const ProcedureValidator& _procedure, const json::Json& _json);

    /* validation methods, reformat message thrown during validation */
    bool valid();
    std::string wrong_field();

    const json::Json& operator[](const std::string& field);

private:
    const ProcedureValidator* procedure;
    json::Json value = json::Json();
    std::string json{};
    bool validated{false};
    std::string message{};
    std::string fields{};
};

#define VALIDATOR(name, s, ...) \
    ProcedureValidator p_##name{\
        #name,\
        PARAMS_BY_NAME,\
        __VA_ARGS__\
    };\
    ValidatorTester name{p_##name, s}


ValidatorTester::ValidatorTester(const ProcedureValidator& _procedure, const json::Json& _json):
    procedure(&_procedure),
    value(_json) {
}

ValidatorTester::ValidatorTester(const ProcedureValidator& _procedure, const char* _json):
    procedure(&_procedure),
    json(_json) {
}

bool ValidatorTester::valid() {
    if (!validated) {
        try {
            if (!json.empty()) {
                try {
                    message = "Cannot parse document.";
                    value = json::Json::parse(json);
                }
                catch (...) {
                    // message was already set
                }
            }
            message = "Cannot validate document.";
            procedure->validate(value);
            validated = true;
            message = "";
        }
        catch (const GamiException& ex) {
            message = ex.get_message();
            fields = InvalidField::get_field_name_from_json_data(ex.get_data());
        }
    }
    return message.empty();
}

std::string ValidatorTester::wrong_field()  {
    if (valid()) {
        return "";
    }
    return fields;
}

const json::Json& ValidatorTester::operator[](const std::string& field) {
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

TEST_F(ProcedureValidatorTest, JsonrpcNumbers) {
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
            "integer": 1.1
        })",
        "integer", VALID_JSON_INTEGER,
        nullptr
    );
    ASSERT_FALSE(float_as_int.valid());

    VALIDATOR(int_as_real,
        R"({
            "float": 1
        })",
        "float", VALID_JSON_REAL,
        nullptr
    );
    ASSERT_TRUE(int_as_real.valid());

    // leading zero is not allowed by JSON specification
    VALIDATOR(leading_zero,
        R"({
            "zero": 0232
        })",
        "zero", VALID_NUMERIC,
        nullptr
    );
    ASSERT_FALSE(leading_zero.valid());

    VALIDATOR(proper_int,
        R"({
            "simple": 231,
            "dot": 233.0,
            "exp": 234e0,
            "dot_exp": 2.35e2
        })",
        "simple", VALID_NUMERIC,
        "dot", VALID_NUMERIC,
        "exp", VALID_NUMERIC,
        "dot_exp", VALID_NUMERIC,
        nullptr
    );
    ASSERT_TRUE(proper_int.valid());
    ASSERT_EQ(231, proper_int["simple"]);
    ASSERT_EQ(233, proper_int["dot"]);
    ASSERT_EQ(234, proper_int["exp"]);
    ASSERT_EQ(235, proper_int["dot_exp"]);

    VALIDATOR(null_val,
        R"({
            "nulled": null
        })",
        "nulled", VALID_NUMERIC,
        nullptr
    );
    ASSERT_FALSE(null_val.valid());
    ASSERT_EQ(nullptr, null_val["nulled"]);

    VALIDATOR(false_val,
        R"({
            "false": false
        })",
        "false", VALID_NUMERIC,
        nullptr
    );
    ASSERT_FALSE(false_val.valid());
    ASSERT_EQ(false, false_val["false"]);

    VALIDATOR(true_val,
        R"({
            "true": true
        })",
        "true", VALID_NUMERIC,
        nullptr
    );
    ASSERT_FALSE(true_val.valid());
    ASSERT_EQ(true, true_val["true"]);

    VALIDATOR(string_val,
        R"({
            "string": "12"
        })",
        "string", VALID_NUMERIC,
        nullptr
    );
    ASSERT_FALSE(string_val.valid());
    ASSERT_ANY_THROW(string_val["string"].get<std::uint32_t>());
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
    ASSERT_EQ(10, conversions["just"]);
    ASSERT_EQ(-10, conversions["negative"]);
    ASSERT_EQ(12, conversions["dot"]);
    ASSERT_EQ(120, conversions["exp"]);
    ASSERT_EQ(123, conversions["dot_exp"]);

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
            "uint": -12
        })",
        "uint", VALID_NUMERIC_TYPED(UINT32),
        nullptr
    );
    ASSERT_FALSE(uint_from_signed.valid());
    ASSERT_EQ("uint", uint_from_signed.wrong_field());

    VALIDATOR(int64_ranges1,
        R"({
            "int64Max": 9223372036854775807,
            "posInt64": 2147483648,
            "int32Max": 2147483647,
            "posInt32": 1,
            "negInt32": -1,
            "int32Min": -2147483648,
            "negInt64": -2147483649,
            "int64Min": -9223372036854775808
        })",
        "int64Max", VALID_NUMERIC_TYPED(INT64),
        "posInt64", VALID_NUMERIC_TYPED(INT64),
        "int32Max", VALID_NUMERIC_TYPED(INT64),
        "posInt32", VALID_NUMERIC_TYPED(INT64),
        "negInt32", VALID_NUMERIC_TYPED(INT64),
        "int32Min", VALID_NUMERIC_TYPED(INT64),
        "negInt64", VALID_NUMERIC_TYPED(INT64),
        "int64Min", VALID_NUMERIC_TYPED(INT64),
        nullptr
    );
    ASSERT_TRUE(int64_ranges1.valid());

/*  No way of checking it right now
    VALIDATOR(int64_ranges2,
        R"({
            "negBigInt": -9223372036854775809
        })",
        "negBigInt", VALID_NUMERIC_TYPED(INT64),
        nullptr
    );
    ASSERT_FALSE(int64_ranges2.valid());

    VALIDATOR(int64_ranges8,
        R"({
            "posBigInt": 9223372036854775808
        })",
        "posBigInt", VALID_NUMERIC_TYPED(INT64),
        nullptr
    );
    ASSERT_FALSE(int64_ranges8.valid()); */

    VALIDATOR(int64_ranges3,
        R"({
            "veryNegativeBigInt": -10000000000000000000000
        })",
        "veryNegativeBigInt", VALID_NUMERIC_TYPED(INT64),
        nullptr
    );
    ASSERT_FALSE(int64_ranges3.valid());

    VALIDATOR(int64_ranges4,
        R"({
            "veryBigInt": 10000000000000000000000
        })",
        "veryBigInt", VALID_NUMERIC_TYPED(INT64),
        nullptr
    );
    ASSERT_FALSE(int64_ranges4.valid());

    VALIDATOR(int64_types1,
        R"({
            "int64": 0.5
        })",
        "int64", VALID_NUMERIC_TYPED(INT64),
        nullptr
    );
    ASSERT_FALSE(int64_types1.valid());

/*  No way of checking it right now
    VALIDATOR(int64_types2,
        R"({
            "overInt64": 9223372036854775807.1
        })",
        "overInt64", VALID_NUMERIC_TYPED(INT64),
        nullptr
    );
    ASSERT_FALSE(int64_types2.valid());

    VALIDATOR(int64_types3,
        R"({
            "underInt64": -9223372036854775808.1
        })",
        "underInt64", VALID_NUMERIC_TYPED(INT64),
        nullptr
    );
    ASSERT_FALSE(int64_types3.valid());*/

    VALIDATOR(uint64_ranges1,
        R"({
            "uint64Max": 18446744073709551615,
            "int64Max": 9223372036854775807,
            "posInt64": 2147483648,
            "int32Max": 2147483647,
            "posInt32": 1
        })",
        "uint64Max", VALID_NUMERIC_TYPED(UINT64),
        "int64Max", VALID_NUMERIC_TYPED(UINT64),
        "posInt64", VALID_NUMERIC_TYPED(UINT64),
        "int32Max", VALID_NUMERIC_TYPED(UINT64),
        "posInt32", VALID_NUMERIC_TYPED(UINT64),
        nullptr
    );
    ASSERT_TRUE(uint64_ranges1.valid());

    VALIDATOR(uint64_ranges2,
        R"({
            "negInt32": -1
        })",
        "negInt32", VALID_NUMERIC_TYPED(UINT64),
        nullptr
    );
    ASSERT_FALSE(uint64_ranges2.valid());

    VALIDATOR(uint64_ranges3,
        R"({
            "int32Min": -2147483648
        })",
        "int32Min", VALID_NUMERIC_TYPED(UINT64),
        nullptr
    );
    ASSERT_FALSE(uint64_ranges3.valid());

    VALIDATOR(uint64_ranges4,
        R"({
            "negInt64": -2147483649
        })",
        "negInt64", VALID_NUMERIC_TYPED(UINT64),
        nullptr
    );
    ASSERT_FALSE(uint64_ranges4.valid());

    VALIDATOR(uint64_ranges5,
        R"({
            "int64Min": -9223372036854775808
        })",
        "int64Min", VALID_NUMERIC_TYPED(UINT64),
        nullptr
    );
    ASSERT_FALSE(uint64_ranges5.valid());

    VALIDATOR(uint64_ranges6,
        R"({
            "negBigInt": -9223372036854775809
        })",
        "negBigInt", VALID_NUMERIC_TYPED(UINT64),
        nullptr
    );
    ASSERT_FALSE(uint64_ranges6.valid());

/*  No way of checking it right now
    VALIDATOR(uint64_ranges7,
        R"({
            "posBigInt": 18446744073709551616
        })",
        "posBigInt", VALID_NUMERIC_TYPED(UINT64),
        nullptr
    );
    ASSERT_FALSE(uint64_ranges7.valid());*/

    VALIDATOR(uint64_ranges8,
        R"({
            "veryBigUInt": 10000000000000000000000
        })",
        "veryBigUInt", VALID_NUMERIC_TYPED(UINT64),
        nullptr
    );
    ASSERT_FALSE(uint64_ranges8.valid());

    VALIDATOR(uint64_types1,
        R"({
            "underUInt64": -0.1
        })",
        "underUInt64", VALID_NUMERIC_TYPED(UINT64),
        nullptr
    );
    ASSERT_FALSE(uint64_types1.valid());

/*  No way of checking it right now
    VALIDATOR(uint64_types2,
        R"({
            "overUInt64": 18446744073709551615.1
        })",
        "overUInt64", VALID_NUMERIC_TYPED(UINT64),
        nullptr
    );
    ASSERT_FALSE(uint64_types2.valid()); */

    VALIDATOR(uint64_types3,
        R"({
            "UInt64": 0.1
        })",
        "UInt64", VALID_NUMERIC_TYPED(UINT64),
        nullptr
    );
    ASSERT_FALSE(uint64_types3.valid());
}

TEST_F(ProcedureValidatorTest, NumbersFromBoolean) {
    VALIDATOR(int_from_boolean,
        R"({ "int": true })",
        "int", VALID_NUMERIC_TYPED(INT32),
        nullptr
    );
    ASSERT_FALSE(int_from_boolean.valid());

    VALIDATOR(uint_from_boolean,
        R"({ "uint": true })",
        "uint", VALID_NUMERIC_TYPED(UINT32),
        nullptr
    );
    ASSERT_FALSE(uint_from_boolean.valid());

    VALIDATOR(double_from_boolean,
        R"({ "double": true })",
        "double", VALID_NUMERIC_TYPED(DOUBLE),
        nullptr
    );
    ASSERT_FALSE(double_from_boolean.valid());

    VALIDATOR(from_number_boolean,
        R"({
            "int": 1,
            "uint": 1,
            "double": 1
        })",
        "int", VALID_NUMERIC_TYPED(INT32),
        "uint", VALID_NUMERIC_TYPED(UINT32),
        "double", VALID_NUMERIC_TYPED(DOUBLE),
        nullptr
    );
    ASSERT_TRUE(from_number_boolean.valid());
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
            "a": null,
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

TEST_F(ProcedureValidatorTest, Regex) {
   VALIDATOR(valid_target,
              R"({
            "string":"iqn.2019-02.com.vmware.comp:name1"
        })",
              "string", VALID_REGEX(RemoteTarget::TARGET_IQN),
              nullptr
    );
    ASSERT_TRUE(valid_target.valid());

    VALIDATOR(invalid_target_marks_and_big_letter,
              R"({
            "string":"iqn.s+Da]/'}="
        })",
              "string", VALID_REGEX(RemoteTarget::TARGET_IQN),
              nullptr
    );
    ASSERT_FALSE(invalid_target_marks_and_big_letter.valid());

    VALIDATOR(invalid_target_empty_name,
              R"({
            "string":"iqn."
        })",
              "string", VALID_REGEX(RemoteTarget::TARGET_IQN),
              nullptr
    );
    ASSERT_FALSE(invalid_target_empty_name.valid());

    VALIDATOR(invalid_target_no_start_prefix,
        R"({
            "string":"asdasdasdasd"
        })",
        "string", VALID_REGEX(RemoteTarget::TARGET_IQN),
         nullptr
    );
    ASSERT_FALSE(invalid_target_no_start_prefix.valid());

    VALIDATOR(invalid_target_space,
        R"({
            "string":"as dasd"
        })",
        "string", VALID_REGEX(RemoteTarget::TARGET_IQN),
        nullptr
    );
    ASSERT_FALSE(invalid_target_space.valid());

    VALIDATOR(valid_initiator,
              R"({
            "string":"iqn.2019-02.com.vmware.comp:name1"
        })",
              "string", VALID_REGEX(RemoteTarget::INITIATOR_IQN),
              nullptr
    );
    ASSERT_TRUE(valid_initiator.valid());

    VALIDATOR(invalid_initiator_marks_and_big_letter,
              R"({
            "string":"iqn.s+Da]/'}="
        })",
              "string", VALID_REGEX(RemoteTarget::INITIATOR_IQN),
              nullptr
    );
    ASSERT_FALSE(invalid_initiator_marks_and_big_letter.valid());

    VALIDATOR(invalid_initiator_empty_name,
              R"({
            "string":"iqn."
        })",
              "string", VALID_REGEX(RemoteTarget::INITIATOR_IQN),
              nullptr
    );
    ASSERT_FALSE(invalid_initiator_empty_name.valid());

    VALIDATOR(invalid_initiator_no_start_prefix,
              R"({
            "string":"asdasdasdasd"
        })",
              "string", VALID_REGEX(RemoteTarget::INITIATOR_IQN),
              nullptr
    );
    ASSERT_FALSE(invalid_initiator_no_start_prefix.valid());

    VALIDATOR(invalid_initiator_space,
              R"({
            "string":"as dasd"
        })",
              "string", VALID_REGEX(RemoteTarget::INITIATOR_IQN),
              nullptr
    );
    ASSERT_FALSE(invalid_initiator_space.valid());

    VALIDATOR(valid_ip,
        R"({
            "string":"123.123.123.123"
        })",
        "string", VALID_REGEX(IPAddresses::ADDRESS),
        nullptr
    );
    ASSERT_TRUE(valid_ip.valid());

    VALIDATOR(invalid_ip,
        R"({
            "string":"123.123.123"
        })",
        "string", VALID_REGEX(IPAddresses::ADDRESS),
        nullptr
    );
    ASSERT_FALSE(invalid_ip.valid());

    VALIDATOR(invalid_ip2,
	    R"({
	        "string":"999.999.999.999"
	    })",
	    "string", VALID_REGEX(IPAddresses::ADDRESS),
	    nullptr
    );
    ASSERT_FALSE(invalid_ip2.valid());

    VALIDATOR(valid_mac,
        R"({
            "string":"af:af:99:af:00:af"
        })",
        "string", VALID_REGEX(EthernetInterface::MAC_ADDRESS),
        nullptr
    );
    ASSERT_TRUE(valid_mac.valid());

    VALIDATOR(invalid_mac,
        R"({
            "string":"af:af:99:xx:00:af"
        })",
        "string", VALID_REGEX(EthernetInterface::MAC_ADDRESS),
        nullptr
    );
    ASSERT_FALSE(invalid_mac.valid());

    VALIDATOR(valid_date_time,
        R"({
            "string":"+18:59"
        })",
        "string", VALID_REGEX(Common::DATE_TIME_LOCAL_OFFSET),
        nullptr
    );
    ASSERT_TRUE(valid_date_time.valid());

    VALIDATOR(invalid_date_time,
        R"({
            "string":"-28:59"
        })",
        "string", VALID_REGEX(Common::DATE_TIME_LOCAL_OFFSET),
        nullptr
    );
    ASSERT_FALSE(invalid_date_time.valid());

    VALIDATOR(valid_redfish,
        R"({
            "string":"4.4.4"
        })",
        "string", VALID_REGEX(ServiceRoot::REDFISH_VERSION),
        nullptr
    );
    ASSERT_TRUE(valid_redfish.valid());

    VALIDATOR(invalid_redfish,
        R"({
            "string":"12.21"
        })",
        "string", VALID_REGEX(ServiceRoot::REDFISH_VERSION),
        nullptr
    );
    ASSERT_FALSE(invalid_redfish.valid());

    VALIDATOR(valid_device_id,
        R"({
            "string":"0x32ff"
        })",
        "string", VALID_REGEX(Common::DEVICE_ID),
        nullptr
    );
    ASSERT_TRUE(valid_device_id.valid());

    VALIDATOR(invalid_device_id,
        R"({
            "string":"0xXXXX"
        })",
        "string", VALID_REGEX(Common::DEVICE_ID),
        nullptr
    );
    ASSERT_FALSE(invalid_device_id.valid());
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

    VALIDATOR(too_short_length,
        R"({
            "uuid": "12-12-12-12-12"
        })",
        "uuid", VALID_UUID,
        nullptr
    );
    ASSERT_FALSE(too_short_length.valid());

    VALIDATOR(too_long_length,
        R"({
            "uuid": "12345678-1234-1234-1234-1234567812345678"
        })",
        "uuid", VALID_UUID,
        nullptr
    );
    ASSERT_FALSE(too_long_length.valid());

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
    ASSERT_EQ("array/1", wrong_types.wrong_field());
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
}


/*! @brief Attribute "interface" implementation */
class AttributeExample {
public:
    static const ProcedureValidator& get_procedure() {
        static ProcedureValidator procedure{
            "example",
            PARAMS_BY_NAME,
            "int", VALID_NUMERIC,
            "optional_int", VALID_OPTIONAL(VALID_NUMERIC_TYPED(INT32)),
            "nullable_string", VALID_NULLABLE(VALID_JSON_STRING),
            nullptr
        };
        return procedure;
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
    ASSERT_EQ("object/nullable_string", wrong_attr.wrong_field());
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

class ObjValidator {
public:
    static const ProcedureValidator& get_procedure() {
        static ProcedureValidator procedure{
            "obj_validator",
            PARAMS_BY_NAME,
            "str", VALID_JSON_STRING,
            "int", VALID_NUMERIC_EQGT(INT32, 1),
            "optional", VALID_OPTIONAL(VALID_JSON_INTEGER),
            nullptr
        };
        return procedure;
    }
};

TEST_F(ProcedureValidatorTest, Conversion) {
    json::Json val = json::Json();
    val["int"] = -10;
    val["uint"] = 10;
    val["float"] = 11.1;
    val["str"] = "String";
    val["array"][0] = 0;
    val["array"][1] = "first";
    val["obj"]["str"] = "first";
    val["obj"]["int"] = 2;

    VALIDATOR(converted, val,
        "int", VALID_NUMERIC_TYPED(INT32),
        "uint", VALID_NUMERIC_TYPED(UINT32),
        "float", VALID_NUMERIC_TYPED(DOUBLE),
        "str", VALID_JSON_STRING,
        "array", VALID_JSON_ARRAY, // content is not checked
        "obj", VALID_ATTRIBUTE(ObjValidator),
        nullptr
    );
    ASSERT_TRUE(converted.valid());

    VALIDATOR(converted2, val,
        "int", VALID_JSON_ANY,
        "uint", VALID_JSON_ANY,
        "float", VALID_JSON_ANY,
        "str", VALID_JSON_ANY,
        "array", VALID_JSON_ANY,
        "obj", VALID_JSON_ANY,
        "new", VALID_JSON_ANY,
        nullptr
    );
    ASSERT_FALSE(converted2.valid());
}


TEST_F(ProcedureValidatorTest, ValidNever) {
    VALIDATOR(empty_object,
        R"({
             "emptyObject": { },
        })",
        "emptyObject", VALID_NEVER,
        nullptr
    );
    ASSERT_FALSE(empty_object.valid());

    VALIDATOR(empty_array,
        R"({
             "emptyArray": [ ],
        })",
         "emptyArray", VALID_NEVER,
         nullptr
    );
    ASSERT_FALSE(empty_array.valid());

    VALIDATOR(object,
        R"({
             "object": { "property": "a something" },
        })",
        "object", VALID_NEVER,
        nullptr
    );
    ASSERT_FALSE(object.valid());

    VALIDATOR(array,
        R"({
             "array": [ "item1" ],
        })",
        "array", VALID_NEVER,
        nullptr
    );
    ASSERT_FALSE(array.valid());

    VALIDATOR(number,
        R"({
             "number": 123,
        })",
        "number", VALID_NEVER,
        nullptr
    );
    ASSERT_FALSE(number.valid());

    VALIDATOR(boolean,
        R"({
             "boolean": true,
        })",
        "boolean", VALID_NEVER,
        nullptr
    );
    ASSERT_FALSE(boolean.valid());

    VALIDATOR(null,
        R"({
             "null": null,
        })",
        "null", VALID_NEVER,
        nullptr
    );
    ASSERT_FALSE(null.valid());

    VALIDATOR(float_number,
        R"({
             "float": 123.321,
        })",
        "float", VALID_NEVER,
        nullptr
    );
    ASSERT_FALSE(float_number.valid());

    VALIDATOR(string,
        R"({
             "string": "qwerty"
        })",
        "string", VALID_NEVER,
        nullptr
    );
    ASSERT_FALSE(string.valid());

    // If exists should fail
    VALIDATOR(null_or_fail,
              R"({ "property": null })",
              "property", VALID_NULLABLE(VALID_NEVER),
              nullptr
    );
    ASSERT_TRUE(null_or_fail.valid());
    VALIDATOR(nullable_fail,
              R"({ "property": "notNull" })",
              "property", VALID_NULLABLE(VALID_NEVER),
              nullptr
    );
    ASSERT_FALSE(nullable_fail.valid());

}

}
