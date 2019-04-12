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

#include "agent-framework/module/utils/optional_field.hpp"
#include "agent-framework/module/enum/enum_builder.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include "gtest/gtest.h"

#include <cstdint>
#include <limits>
#include <cmath>
#include <sstream>
#include <ostream>

using namespace std;

namespace  {
    constexpr char INT[] = "int";
    constexpr char UINT[] = "unsigned int";
    constexpr char DOUBLE[] = "double";
    constexpr char CSTRING[] = "const char*";
    constexpr char STRING[] = "std::string";
    constexpr char BOOL[] = "bool";
    constexpr char NULL_VALUE[] = "null";

    bool compare_double(double lhs, double rhs) {
        return abs(lhs - rhs) < numeric_limits<double>::epsilon();
    }

    ENUM(RushCollection, uint32_t, TomSawyer, Limelight, Xanadu);
}

class OptionalFieldTest : public ::testing::Test {
public:
    virtual ~OptionalFieldTest();

    virtual void SetUp() {
        int_value = 1;
        uint_value = 1u;
        int64_value = 1;
        uint64_value = 1u;
        double_value = 1.0;
        cstring_value = "Hello, world!";
        string_value = string("Hello, world!");
        bool_value = true;
    }

    virtual void TearDown() {}

    int int_value{};
    unsigned int uint_value{};
    int64_t int64_value{};
    uint64_t uint64_value{};
    double double_value{};
    const char* cstring_value{};
    string string_value{};
    bool bool_value{};
};

OptionalFieldTest::~OptionalFieldTest() {}

/*!
 * This test verifies if a default-initialized OptionalField class object
 * contains any value.
 * */
TEST_F(OptionalFieldTest, DefaultConstructionToNullValue) {
    /* Verifies that an object contains no value */
    ASSERT_FALSE(OptionalField<int>().has_value());
    ASSERT_FALSE(OptionalField<unsigned int>().has_value());
    ASSERT_FALSE(OptionalField<int64_t>().has_value());
    ASSERT_FALSE(OptionalField<uint64_t>().has_value());
    ASSERT_FALSE(OptionalField<double>().has_value());
    ASSERT_FALSE(OptionalField<const char*>().has_value());
    ASSERT_FALSE(OptionalField<string>().has_value());
    ASSERT_FALSE(OptionalField<bool>().has_value());

    /* Verifies that an exception is thrown while accessing empty object */
    ASSERT_THROW(OptionalField<int>().value(), experimental::bad_optional_access);
    ASSERT_THROW(OptionalField<unsigned int>().value(), experimental::bad_optional_access);
    ASSERT_THROW(OptionalField<int64_t>().value(), experimental::bad_optional_access);
    ASSERT_THROW(OptionalField<uint64_t>().value(), experimental::bad_optional_access);
    ASSERT_THROW(OptionalField<double>().value(), experimental::bad_optional_access);
    ASSERT_THROW(OptionalField<const char*>().value(), experimental::bad_optional_access);
    ASSERT_THROW(OptionalField<string>().value(), experimental::bad_optional_access);
    ASSERT_THROW(OptionalField<bool>().value(), experimental::bad_optional_access);
}

/*!
 * This test verifies that a lvalue-constructed OptionalField class object
 * contains a value identical to the constructior parameter.
 * */
TEST_F(OptionalFieldTest, ValueConstruction) {
    /* Verifies that the object contains a value */
    ASSERT_TRUE(OptionalField<int>(int_value).has_value());
    ASSERT_TRUE(OptionalField<unsigned int>(uint_value).has_value());
    ASSERT_TRUE(OptionalField<int64_t>(int64_value).has_value());
    ASSERT_TRUE(OptionalField<uint64_t>(uint64_value).has_value());
    ASSERT_TRUE(OptionalField<double>(double_value).has_value());
    ASSERT_TRUE(OptionalField<const char*>(cstring_value).has_value());
    ASSERT_TRUE(OptionalField<string>(cstring_value).has_value());
    ASSERT_TRUE(OptionalField<string>(string_value).has_value());
    ASSERT_TRUE(OptionalField<bool>(bool_value).has_value());

    /* Verifies that the value is equal to the constructor parameter */
    ASSERT_EQ(int_value, OptionalField<int>(int_value).value());
    ASSERT_EQ(uint_value, OptionalField<unsigned int>(uint_value).value());
    ASSERT_EQ(int64_value, OptionalField<int64_t>(int64_value).value());
    ASSERT_EQ(uint64_value, OptionalField<uint64_t>(uint64_value).value());
    ASSERT_TRUE(compare_double(double_value, OptionalField<double>(double_value).value()));
    ASSERT_EQ(string(cstring_value), string(OptionalField<const char*>(cstring_value).value()));
    ASSERT_EQ(string(cstring_value), OptionalField<string>(cstring_value).value());
    ASSERT_EQ(string_value, OptionalField<string>(string_value).value());
    ASSERT_EQ(bool_value, OptionalField<bool>(bool_value).value());
}

/*!
 * This test verifies that a move-constructed OptionalField class object
 * contains a value identical to the constructior parameter.
 * */
TEST_F(OptionalFieldTest, MoveConstruction) {
    /* Move-construct objects */
    OptionalField<int> int_opfield = OptionalField<int>(int_value);
    OptionalField<unsigned int> uint_opfield = OptionalField<unsigned int>(uint_value);
    OptionalField<int64_t> int64_opfield = OptionalField<int64_t>(int64_value);
    OptionalField<uint64_t> uint64_opfield = OptionalField<uint64_t>(uint64_value);
    OptionalField<double> double_opfield = OptionalField<double>(double_value);
    OptionalField<const char*> cstring_opfield = OptionalField<const char*>(cstring_value);
    OptionalField<string> string_opfield = OptionalField<string>(string_value);
    OptionalField<bool> bool_opfield = OptionalField<bool>(bool_value);

    /* Verify that an object contains a value */
    ASSERT_TRUE(int_opfield.has_value());
    ASSERT_TRUE(uint_opfield.has_value());
    ASSERT_TRUE(int64_opfield.has_value());
    ASSERT_TRUE(uint64_opfield.has_value());
    ASSERT_TRUE(double_opfield.has_value());
    ASSERT_TRUE(cstring_opfield.has_value());
    ASSERT_TRUE(string_opfield.has_value());
    ASSERT_TRUE(bool_opfield.has_value());

    /* Verify that the value is equal to the constructor parameter */
    ASSERT_EQ(int_value, int_opfield.value());
    ASSERT_EQ(uint_value, uint_opfield.value());
    ASSERT_EQ(int64_value, int64_opfield.value());
    ASSERT_EQ(uint64_value, uint64_opfield.value());
    ASSERT_TRUE(compare_double(double_value, double_opfield.value()));
    ASSERT_EQ(string_value, cstring_opfield.value());
    ASSERT_EQ(string_value, string_opfield.value());
    ASSERT_EQ(bool_value, bool_opfield.value());
}

/*!
 * This test verifies that a copy-constructed OptionalField class object
 * contains a value identical to the one contained by the object used for
 * copy-construction
 * */
TEST_F(OptionalFieldTest, CopyConstruction) {
    /* Construct object to be copied from */
    OptionalField<int> int_rhs_opfield(int_value);
    OptionalField<unsigned int> uint_rhs_opfield(uint_value);
    OptionalField<int64_t> int64_rhs_opfield(int64_value);
    OptionalField<uint64_t> uint64_rhs_opfield(uint64_value);
    OptionalField<double> double_rhs_opfield(double_value);
    OptionalField<const char*> cstring_rhs_opfield(cstring_value);
    OptionalField<string> string_rhs_opfield(string_value);
    OptionalField<bool> bool_rhs_opfield(bool_value);

    /* Copy-construct objects */
    OptionalField<int> int_opfield = int_rhs_opfield;
    OptionalField<unsigned int> uint_opfield = uint_rhs_opfield;
    OptionalField<int64_t> int64_opfield = int64_rhs_opfield;
    OptionalField<uint64_t> uint64_opfield = uint64_rhs_opfield;
    OptionalField<double> double_opfield = double_rhs_opfield;
    OptionalField<const char*> cstring_opfield = cstring_rhs_opfield;
    OptionalField<string> string_opfield = string_rhs_opfield;
    OptionalField<bool> bool_opfield = bool_rhs_opfield;

    /* Verifies that a copy-constructed object contains a value */
    ASSERT_TRUE(int_opfield.has_value());
    ASSERT_TRUE(uint_opfield.has_value());
    ASSERT_TRUE(int64_opfield.has_value());
    ASSERT_TRUE(uint64_opfield.has_value());
    ASSERT_TRUE(double_opfield.has_value());
    ASSERT_TRUE(cstring_opfield.has_value());
    ASSERT_TRUE(string_opfield.has_value());
    ASSERT_TRUE(bool_opfield.has_value());

    /* Verifies that the value is equal to the value held by the object constructed from */
    ASSERT_EQ(int_rhs_opfield.value(), int_opfield.value());
    ASSERT_EQ(uint_rhs_opfield.value(), uint_opfield.value());
    ASSERT_EQ(int64_rhs_opfield.value(), int64_opfield.value());
    ASSERT_EQ(uint64_rhs_opfield.value(), uint64_opfield.value());
    ASSERT_TRUE(compare_double(double_rhs_opfield.value(), double_opfield.value()));
    ASSERT_EQ(cstring_rhs_opfield.value(), cstring_opfield.value());
    ASSERT_EQ(string_rhs_opfield.value(), string_opfield.value());
    ASSERT_EQ(bool_rhs_opfield.value(), bool_opfield.value());
}

/*!
 * This test verifies that an OptionalField class constructed from a json::Json
 * object contains a value identical to that held by the json::Json object
 * */
TEST_F(OptionalFieldTest, RpcJsonValueConstruction) {
    /* Construct an empty json::Json object */
    json::Json json = json::Json();

    /* Construct and assign OptionalField objects to json::Json */
    json[INT] = OptionalField<int>(int_value);
    json[UINT] = OptionalField<unsigned int>(uint_value);
    json[DOUBLE] = OptionalField<double>(double_value);
    json[CSTRING] = OptionalField<const char*>(cstring_value);
    json[STRING] = OptionalField<string>(string_value);
    json[BOOL] = OptionalField<bool>(bool_value);

    /* Verify that the json::Json object contains proper values */
    ASSERT_EQ(int_value, json[INT]);
    ASSERT_EQ(uint_value, json[UINT]);
    ASSERT_TRUE(compare_double(double_value, json[DOUBLE]));
    ASSERT_EQ(string(cstring_value), json.value(CSTRING, string{}));
    ASSERT_EQ(string_value, json.value(STRING, std::string{}));
    ASSERT_EQ(bool_value, json[BOOL]);
}

/*!
 * This test verifies that an OptionalField class constructed from a json::Json
 * object from JsonCxx library contains a value identical to that held by the
 * json::Json object
 * */
TEST_F(OptionalFieldTest, JsonCXXValueConstruction) {
    /* Construct an empty json::Json object */
    json::Json json = json::Json();

    /* Construct and assign OptionalField objects to json::Json */
    json[INT] = OptionalField<int>(int_value);
    json[UINT] = OptionalField<unsigned int>(uint_value);
    json[DOUBLE] = OptionalField<double>(double_value);
    json[CSTRING] = OptionalField<const char*>(cstring_value);
    json[STRING] = OptionalField<string>(string_value);
    json[BOOL] = OptionalField<bool>(bool_value);
    json[NULL_VALUE] = OptionalField<int>();

    /* Verify that the json::Json object contains proper values */
    ASSERT_EQ(int_value, json.value(INT, int{}));
    ASSERT_EQ(uint_value, json.value(UINT, std::uint16_t{}));
    ASSERT_TRUE(compare_double(double_value, json.value(DOUBLE, double{})));
    ASSERT_EQ(string(cstring_value), string(json.value(CSTRING, std::string{})));
    ASSERT_EQ(string_value, json.value(STRING, std::string{}));
    ASSERT_EQ(bool_value, json.value(BOOL, bool{}));
    ASSERT_TRUE(json.value(NULL_VALUE, json::Json()).is_null());
}

/*!
 * This test verifies that assignment operator for OptionalValue class
 * works correctly
 * */
TEST_F(OptionalFieldTest, TemplateParameterValueAssignment) {
    /* Construct empty OptionalField objects */
    OptionalField<int> int_opfield = OptionalField<int>();
    OptionalField<unsigned int> uint_opfield = OptionalField<unsigned int>();
    OptionalField<int64_t> int64_opfield = OptionalField<int64_t>();
    OptionalField<uint64_t> uint64_opfield = OptionalField<uint64_t>();
    OptionalField<double> double_opfield = OptionalField<double>();
    OptionalField<const char*> cstring_opfield = OptionalField<const char*>();
    OptionalField<string> string_opfield = OptionalField<string>();
    OptionalField<string> string_opfield_from_cstring = OptionalField<string>();
    OptionalField<bool> bool_opfield = OptionalField<bool>();

    /* Assign values to them */
    int_opfield = int_value;
    uint_opfield = uint_value;
    int64_opfield = int64_value;
    uint64_opfield = uint64_value;
    double_opfield = double_value;
    cstring_opfield = cstring_value;
    string_opfield = string_value;
    string_opfield_from_cstring = cstring_value;
    bool_opfield = bool_value;

    /* Verifies that an object contains a value */
    ASSERT_TRUE(int_opfield.has_value());
    ASSERT_TRUE(uint_opfield.has_value());
    ASSERT_TRUE(int64_opfield.has_value());
    ASSERT_TRUE(uint64_opfield.has_value());
    ASSERT_TRUE(double_opfield.has_value());
    ASSERT_TRUE(cstring_opfield.has_value());
    ASSERT_TRUE(string_opfield.has_value());
    ASSERT_TRUE(bool_opfield.has_value());

    /* Verifies that the value is equal to the constructor parameter */
    ASSERT_EQ(int_value, int_opfield.value());
    ASSERT_EQ(uint_value, uint_opfield.value());
    ASSERT_EQ(int64_value, int64_opfield.value());
    ASSERT_EQ(uint64_value, uint64_opfield.value());
    ASSERT_TRUE(compare_double(double_value, double_opfield.value()));
    ASSERT_EQ(string(cstring_value), string(cstring_opfield.value()));
    ASSERT_EQ(string(cstring_value), string_opfield_from_cstring.value());
    ASSERT_EQ(string_value, string_opfield.value());
    ASSERT_EQ(bool_value, bool_opfield.value());
}

/*!
 * This test verifies that that an uninitialized OptionalField object converts
 * properly to json::Json object from rpc json library containing a null value.
 * */
TEST_F(OptionalFieldTest, RpcJsonNullValueConversion) {
    json::Json json = json::Json();

    OptionalField<int> int_opfield = json[INT];
    OptionalField<unsigned int> uint_opfield = json[UINT];
    OptionalField<double> double_opfield = json[DOUBLE];
    OptionalField<const char*> cstring_opfield = json[CSTRING];
    OptionalField<string> string_opfield = json[STRING];
    OptionalField<bool> bool_opfield = json[BOOL];

    /* Verify that null-constructed OptionalField objects have no value */
    ASSERT_THROW(int_opfield.value(), experimental::bad_optional_access);
    ASSERT_THROW(uint_opfield.value(), experimental::bad_optional_access);
    ASSERT_THROW(double_opfield.value(), experimental::bad_optional_access);
    ASSERT_THROW(cstring_opfield.value(), experimental::bad_optional_access);
    ASSERT_THROW(string_opfield.value(), experimental::bad_optional_access);
    ASSERT_THROW(bool_opfield.value(), experimental::bad_optional_access);
}

/*!
 * This test verifies that that an initialized OptionalField object converts
 * properly to json::Json object from rpc json library containing a non-null
 * value.
 * */
TEST_F(OptionalFieldTest, RpcJsonValueConversion) {
    json::Json json = json::Json();

    OptionalField<int> int_opfield = (json[INT] = int_value);
    OptionalField<unsigned int> uint_opfield = (json[UINT] = uint_value);
    OptionalField<double> double_opfield = (json[DOUBLE] = double_value);
    OptionalField<string> string_opfield = (json[STRING] = string_value);
    OptionalField<bool> bool_opfield = (json[BOOL] = bool_value);
    OptionalField<const char*> cstring_opfield = (json[CSTRING] = cstring_value);

    /* Verify that null-constructed OptionalField objects have no value */
    ASSERT_TRUE(int_opfield.has_value());
    ASSERT_TRUE(uint_opfield.has_value());
    ASSERT_TRUE(double_opfield.has_value());
    ASSERT_TRUE(cstring_opfield.has_value());
    ASSERT_TRUE(string_opfield.has_value());
    ASSERT_TRUE(bool_opfield.has_value());

    ASSERT_EQ(int_value, int_opfield.value());
    ASSERT_EQ(uint_value, uint_opfield.value());
    ASSERT_TRUE(compare_double(double_value, double_opfield.value()));
    ASSERT_EQ(string_value, cstring_opfield.value());
    ASSERT_EQ(string_value, string_opfield.value());
    ASSERT_EQ(bool_value, bool_opfield.value());
}

/*!
 * This test verifies that an uninitialized OptionalField object converts
 * properly to a json::Json object from JsonCXX library containing a null
 * value.
 * */
TEST_F(OptionalFieldTest, JsonCXXNullValueConversion) {
    json::Json json = json::Json::value_t::null;

    OptionalField<int> int_opfield = json[INT];
    OptionalField<unsigned int> uint_opfield = json[UINT];
    OptionalField<double> double_opfield = json[DOUBLE];
    OptionalField<const char*> cstring_opfield = json[CSTRING];
    OptionalField<string> string_opfield = json[STRING];
    OptionalField<bool> bool_opfield = json[BOOL];

    /* Verify that null-constructed OptionalField objects have no value */
    ASSERT_THROW(int_opfield.value(), experimental::bad_optional_access);
    ASSERT_THROW(uint_opfield.value(), experimental::bad_optional_access);
    ASSERT_THROW(double_opfield.value(), experimental::bad_optional_access);
    ASSERT_THROW(cstring_opfield.value(), experimental::bad_optional_access);
    ASSERT_THROW(string_opfield.value(), experimental::bad_optional_access);
    ASSERT_THROW(bool_opfield.value(), experimental::bad_optional_access);
}

/*!
 * This test verifies that an initialized OptionalField objects converts
 * properly to a json::Json object from JsonCXX library containing a non-null
 * value.
 * */
TEST_F(OptionalFieldTest, JsonCXXValueConversion) {
    json::Json json = json::Json::value_t::null;

    OptionalField<int> int_opfield = (json[INT] = int_value );
    OptionalField<unsigned int> uint_opfield = (json[UINT] = uint_value);
    OptionalField<double> double_opfield = (json[DOUBLE] = double_value);
    auto json_cstring = (json[CSTRING] = cstring_value);
    OptionalField<const char*> cstring_opfield = json_cstring;
    OptionalField<string> string_opfield = (json[STRING] = string_value);
    OptionalField<bool> bool_opfield = (json[BOOL] = bool_value);

    /* Verify that null-constructed OptionalField objects have no value */
    ASSERT_TRUE(int_opfield.has_value());
    ASSERT_TRUE(uint_opfield.has_value());
    ASSERT_TRUE(double_opfield.has_value());
    ASSERT_TRUE(cstring_opfield.has_value());
    ASSERT_TRUE(string_opfield.has_value());
    ASSERT_TRUE(bool_opfield.has_value());

    ASSERT_EQ(int_value, int_opfield.value());
    ASSERT_EQ(uint_value, uint_opfield.value());
    ASSERT_TRUE(compare_double(double_value, double_opfield.value()));
    ASSERT_EQ(string_value, cstring_opfield.value());
    ASSERT_EQ(string_value, string_opfield.value());
    ASSERT_EQ(bool_value, bool_opfield.value());
}

/*!
 * This test verifies that OptionalField object can handle macro-defined enums
 * form agent_framework component.
 * */
TEST_F(OptionalFieldTest, FrameworkEnumsToJson) {
    json::Json rpc_json = json::Json();
    json::Json jsonCXX = json::Json::value_t::null;

    rpc_json["TomSawyer"] = OptionalField<RushCollection>(RushCollection::TomSawyer);
    rpc_json["Limelight"] = OptionalField<RushCollection>(RushCollection::Limelight);
    rpc_json["Xanadu"] = OptionalField<RushCollection>(RushCollection::Xanadu);
    rpc_json["null"] = OptionalField<RushCollection>();

    jsonCXX["TomSawyer"] = OptionalField<RushCollection>(RushCollection::TomSawyer);
    jsonCXX["Limelight"] = OptionalField<RushCollection>(RushCollection::Limelight);
    jsonCXX["Xanadu"] = OptionalField<RushCollection>(RushCollection::Xanadu);
    jsonCXX["null"] = OptionalField<RushCollection>();

    ASSERT_EQ(rpc_json["TomSawyer"], string("TomSawyer"));
    ASSERT_EQ(rpc_json["Limelight"], string("Limelight"));
    ASSERT_EQ(rpc_json["Xanadu"], string("Xanadu"));
    ASSERT_TRUE(rpc_json.value("null", json::Json()).is_null());

    ASSERT_EQ(jsonCXX["TomSawyer"], string("TomSawyer"));
    ASSERT_EQ(jsonCXX["Limelight"], string("Limelight"));
    ASSERT_EQ(jsonCXX["Xanadu"], string("Xanadu"));
    ASSERT_TRUE(json::Json::value_t::null == jsonCXX["null"].type());
}

/*!
 * This test verifies that OptionalField object can handle macro-defined enums
 * form agent_framework component.
 * */
TEST_F(OptionalFieldTest, FrameworkEnumsFromJson) {
    json::Json rpc_json = json::Json();
    json::Json jsonCXX = json::Json::value_t::null;

    rpc_json["TomSawyer"] = "TomSawyer";
    rpc_json["Limelight"] = "Limelight";
    rpc_json["Xanadu"] = "Xanadu";
    rpc_json["null"] = json::Json{};

    jsonCXX["TomSawyer"] = "TomSawyer";
    jsonCXX["Limelight"] = "Limelight";
    jsonCXX["Xanadu"] = "Xanadu";
    jsonCXX["null"] = json::Json::value_t::null;

    ASSERT_EQ(OptionalField<RushCollection>(RushCollection::TomSawyer), OptionalField<RushCollection>(rpc_json["TomSawyer"]));
    ASSERT_EQ(OptionalField<RushCollection>(RushCollection::Limelight), OptionalField<RushCollection>(rpc_json["Limelight"]));
    ASSERT_NE(OptionalField<RushCollection>(RushCollection::Xanadu), OptionalField<RushCollection>(rpc_json["TomSawyer"]));
    ASSERT_FALSE(OptionalField<RushCollection>(rpc_json["null"]).has_value());

    ASSERT_EQ(OptionalField<RushCollection>(RushCollection::TomSawyer), OptionalField<RushCollection>(jsonCXX["TomSawyer"]));
    ASSERT_EQ(OptionalField<RushCollection>(RushCollection::Limelight), OptionalField<RushCollection>(jsonCXX["Limelight"]));
    ASSERT_NE(OptionalField<RushCollection>(RushCollection::Xanadu), OptionalField<RushCollection>(jsonCXX["TomSawyer"]));
    ASSERT_FALSE(OptionalField<RushCollection>(jsonCXX["null"]).has_value());
}

/*!
 * This test verifies that an OptionalField object converts properly to an
 * object of type used as template parameter.
 * */
TEST_F(OptionalFieldTest, TemplateParameterConversion) {
    ASSERT_EQ(int_value, static_cast<int>(OptionalField<int>(int_value)));
    ASSERT_EQ(uint_value, static_cast<unsigned int>(OptionalField<unsigned int>(uint_value)) );
    ASSERT_EQ(int64_value, static_cast<int64_t>(OptionalField<int64_t>(int64_value)));
    ASSERT_EQ(uint64_value, static_cast<uint64_t>(OptionalField<uint64_t>(uint64_value)));
    ASSERT_TRUE(compare_double(double_value, static_cast<double>(OptionalField<double>(double_value))));
    ASSERT_EQ(string(cstring_value), string(static_cast<const char*>(OptionalField<const char*>(cstring_value))) );
    ASSERT_EQ(string_value, static_cast<string>(OptionalField<string>(string_value)));
    ASSERT_EQ(bool_value, static_cast<bool>(OptionalField<bool>(bool_value)));
}

/*!
 * This test verifies that OptionalField<std::string> comparison operators
 * are working properly.
 * */
TEST_F(OptionalFieldTest, StringComparison) {
    string different_string_value = "2112";
    const char* different_cstring_value = "2112";

    /* Tests if operator equals returns true for identical strings */
    ASSERT_TRUE(OptionalField<string>(string_value) == OptionalField<string>(string_value));
    ASSERT_TRUE(OptionalField<string>(string_value) == cstring_value);
    ASSERT_TRUE(cstring_value == OptionalField<string>(string_value));

    /* Tests if operator equals returns false for different strings */
    ASSERT_FALSE(OptionalField<string>(string_value) == OptionalField<string>(different_string_value));
    ASSERT_FALSE(OptionalField<string>(string_value) == different_cstring_value);
    ASSERT_FALSE(different_cstring_value == OptionalField<string>(string_value));

    /* Tests if operator not equals returns true for different strings */
    ASSERT_TRUE(OptionalField<string>(string_value) != OptionalField<string>(different_string_value));
    ASSERT_TRUE(OptionalField<string>(string_value) != different_cstring_value);
    ASSERT_TRUE(different_cstring_value != OptionalField<string>(string_value));

    /* Tests if operator not equals returns false for identical strings */
    ASSERT_FALSE(OptionalField<string>(string_value) != OptionalField<string>(string_value));
    ASSERT_FALSE(OptionalField<string>(string_value) != cstring_value);
    ASSERT_FALSE(cstring_value != OptionalField<string>(string_value));

    /* deeper tests, checked operators for 'empty' optionals */
    const std::string FIRST{"first"};
    const std::string SECOND{"second"};
    const std::string EMPTY{""};

    OptionalField<std::string> first{FIRST};
    OptionalField<std::string> second{SECOND};
    OptionalField<std::string> empty{EMPTY};
    OptionalField<std::string> not_set{};

    ASSERT_EQ(first, first);
    ASSERT_EQ(first, FIRST.c_str());
    ASSERT_EQ(FIRST.c_str(), first);
    ASSERT_EQ(first, FIRST);
    ASSERT_EQ(FIRST, first);

    ASSERT_EQ(empty, empty);
    ASSERT_EQ(empty, EMPTY);
    ASSERT_EQ(empty, EMPTY.c_str());

    ASSERT_EQ(not_set, not_set);
    ASSERT_EQ(not_set, nullptr);
    ASSERT_EQ(nullptr, not_set);

    ASSERT_NE(not_set, empty);
    ASSERT_NE(empty, not_set);

    ASSERT_NE(first, not_set);
    ASSERT_NE(first, empty);
    ASSERT_NE(first, second);
}

/*!
 * This test verifies that OptionalField comparison operators for framework enums
 * are working properly.
 * */
TEST_F(OptionalFieldTest, EnumComparison){
    /* Comparison for same type of framework enums */
    /* operator== */
    ASSERT_TRUE(OptionalField<RushCollection>(RushCollection::TomSawyer) == OptionalField<RushCollection>(RushCollection::TomSawyer));
    ASSERT_TRUE(OptionalField<RushCollection>(RushCollection::TomSawyer) == RushCollection::TomSawyer);
    ASSERT_TRUE(RushCollection::TomSawyer == OptionalField<RushCollection>(RushCollection::TomSawyer));

    ASSERT_FALSE(OptionalField<RushCollection>(RushCollection::TomSawyer) == OptionalField<RushCollection>(RushCollection::Limelight));
    ASSERT_FALSE(OptionalField<RushCollection>(RushCollection::TomSawyer) == RushCollection::Limelight);
    ASSERT_FALSE(RushCollection::TomSawyer == OptionalField<RushCollection>(RushCollection::Limelight));

    /* operator!= */
    ASSERT_FALSE(OptionalField<RushCollection>(RushCollection::TomSawyer) != OptionalField<RushCollection>(RushCollection::TomSawyer));
    ASSERT_FALSE(OptionalField<RushCollection>(RushCollection::TomSawyer) != RushCollection::TomSawyer);
    ASSERT_FALSE(RushCollection::TomSawyer != OptionalField<RushCollection>(RushCollection::TomSawyer));
    //
    ASSERT_TRUE(OptionalField<RushCollection>(RushCollection::TomSawyer) != OptionalField<RushCollection>(RushCollection::Limelight));
    ASSERT_TRUE(OptionalField<RushCollection>(RushCollection::TomSawyer) != RushCollection::Limelight);
    ASSERT_TRUE(RushCollection::TomSawyer != OptionalField<RushCollection>(RushCollection::Limelight));

    /* operator!= for empty optional */
    ASSERT_TRUE(OptionalField<RushCollection>() != RushCollection::TomSawyer);
    ASSERT_TRUE(OptionalField<RushCollection>() != RushCollection::Limelight);
    ASSERT_TRUE(OptionalField<RushCollection>() != RushCollection::Xanadu);
    ASSERT_TRUE(RushCollection::TomSawyer != OptionalField<RushCollection>());
    ASSERT_TRUE(RushCollection::Limelight != OptionalField<RushCollection>());
    ASSERT_TRUE(RushCollection::Xanadu != OptionalField<RushCollection>());

    /* operator== for empty optional */
    ASSERT_TRUE(OptionalField<RushCollection>() == OptionalField<RushCollection>());
    ASSERT_FALSE(OptionalField<RushCollection>() == RushCollection::TomSawyer);
    ASSERT_FALSE(OptionalField<RushCollection>() == RushCollection::Limelight);
    ASSERT_FALSE(OptionalField<RushCollection>() == RushCollection::Xanadu);
    ASSERT_FALSE(RushCollection::TomSawyer == OptionalField<RushCollection>());
    ASSERT_FALSE(RushCollection::Limelight == OptionalField<RushCollection>());
    ASSERT_FALSE(RushCollection::Xanadu == OptionalField<RushCollection>());

    /* comparison operators with an enum object - not the underlying value */
    RushCollection xanadu(RushCollection::Xanadu);
    ASSERT_TRUE(OptionalField<RushCollection>() != xanadu);
    ASSERT_TRUE(OptionalField<RushCollection>(RushCollection::Limelight) != xanadu);
    ASSERT_TRUE(OptionalField<RushCollection>(RushCollection::Xanadu) == xanadu);
    ASSERT_TRUE(xanadu != OptionalField<RushCollection>());
    ASSERT_TRUE(xanadu != OptionalField<RushCollection>(RushCollection::Limelight));
    ASSERT_TRUE(xanadu == OptionalField<RushCollection>(RushCollection::Xanadu));
}

TEST_F(OptionalFieldTest, OperatorPlus) {
    /* Operator action between two OptionalField objects */
    /* Both operands are null-initialized */
    ASSERT_FALSE((OptionalField<int>() + OptionalField<int>()).has_value());

    /* One operand is null initialized */
    ASSERT_TRUE((OptionalField<int>(int_value) + OptionalField<int>()).has_value());
    ASSERT_TRUE((OptionalField<int>() + OptionalField<int>(int_value)).has_value());
    ASSERT_EQ(OptionalField<int>(int_value), (OptionalField<int>() + OptionalField<int>(int_value)));
    ASSERT_EQ(OptionalField<int>(int_value), (OptionalField<int>(int_value) + OptionalField<int>()));

    /* Both operands are non-null initialized */
    ASSERT_TRUE((OptionalField<int>(int_value) + OptionalField<int>(int_value)).has_value());
    ASSERT_EQ(OptionalField<int>(int_value + int_value), (OptionalField<int>(int_value) + OptionalField<int>(int_value)));

    /* Operator action for one OptionalField object and one template param. obj. */
    /* OptionalField object is null initialized */
    ASSERT_TRUE((int_value + OptionalField<int>()).has_value());
    ASSERT_TRUE((OptionalField<int>()+ int_value).has_value());

    ASSERT_EQ(OptionalField<int>(int_value), int_value + OptionalField<int>());
    ASSERT_EQ(OptionalField<int>(int_value), OptionalField<int>() + int_value);

    /* OptionalField object is non-null initialized */
    ASSERT_TRUE((int_value + OptionalField<int>(int_value)).has_value());
    ASSERT_TRUE((OptionalField<int>(int_value) + int_value).has_value());

    ASSERT_EQ(OptionalField<int>(int_value + int_value), int_value + OptionalField<int>(int_value));
    ASSERT_EQ(OptionalField<int>(int_value + int_value), OptionalField<int>(int_value) + int_value);
}

TEST_F(OptionalFieldTest, OperatorMinus) {
    /* Operator action between two OptionalField objects */
    /* Both operands are null-initialized */
    ASSERT_FALSE((OptionalField<int>() - OptionalField<int>()).has_value());

    /* One operand is null initialized */
    ASSERT_TRUE((OptionalField<int>(int_value) - OptionalField<int>()).has_value());
    ASSERT_TRUE((OptionalField<int>() - OptionalField<int>(int_value)).has_value());
    ASSERT_EQ(OptionalField<int>(-int_value), (OptionalField<int>() - OptionalField<int>(int_value)));
    ASSERT_EQ(OptionalField<int>(int_value), (OptionalField<int>(int_value) - OptionalField<int>()));

    /* Both operands are non-null initialized */
    ASSERT_TRUE((OptionalField<int>(int_value) - OptionalField<int>(int_value)).has_value());
    ASSERT_EQ(OptionalField<int>(int_value - int_value), (OptionalField<int>(int_value) - OptionalField<int>(int_value)));

    /* Operator action for one OptionalField object and one template param. obj. */
    /* OptionalField object is null initialized */
    ASSERT_TRUE((int_value - OptionalField<int>()).has_value());
    ASSERT_TRUE((OptionalField<int>() - int_value).has_value());

    ASSERT_EQ(OptionalField<int>(int_value), int_value - OptionalField<int>());
    ASSERT_EQ(OptionalField<int>(-int_value), OptionalField<int>() - int_value);

    /* OptionalField object is non-null initialized */
    ASSERT_TRUE((int_value - OptionalField<int>(int_value)).has_value());
    ASSERT_TRUE((OptionalField<int>(int_value) - int_value).has_value());

    ASSERT_EQ(OptionalField<int>(int_value - int_value), int_value - OptionalField<int>(int_value));
    ASSERT_EQ(OptionalField<int>(int_value - int_value), OptionalField<int>(int_value) - int_value);
}

TEST_F(OptionalFieldTest, OperatorStar) {
    /* Operator action between two OptionalField objects */
    /* Both operands are null-initialized */
    ASSERT_FALSE((OptionalField<int>() * OptionalField<int>()).has_value());

    /* One operand is null initialized */
    ASSERT_FALSE((OptionalField<int>(int_value) * OptionalField<int>()).has_value());
    ASSERT_FALSE((OptionalField<int>() * OptionalField<int>(int_value)).has_value());

    /* Both operands are non-null initialized */
    ASSERT_TRUE((OptionalField<int>(int_value) * OptionalField<int>(int_value)).has_value());
    ASSERT_EQ(OptionalField<int>(int_value * int_value), (OptionalField<int>(int_value) * OptionalField<int>(int_value)));

    /* Operator action for one OptionalField object and one template param. obj. */
    /* OptionalField object is null initialized */
    ASSERT_FALSE((int_value * OptionalField<int>()).has_value());
    ASSERT_FALSE((OptionalField<int>() * int_value).has_value());

    /* OptionalField object is non-null initialized */
    ASSERT_TRUE((int_value * OptionalField<int>(int_value)).has_value());
    ASSERT_TRUE((OptionalField<int>(int_value) * int_value).has_value());

    ASSERT_EQ(OptionalField<int>(int_value * int_value), int_value * OptionalField<int>(int_value));
    ASSERT_EQ(OptionalField<int>(int_value * int_value), OptionalField<int>(int_value) * int_value);
}

TEST_F(OptionalFieldTest, OperatorSlash) {
    /* Operator action between two OptionalField objects */
    /* Both operands are null-initialized */
    ASSERT_THROW((OptionalField<int>() / OptionalField<int>()), std::logic_error);

    /* One operand is null initialized */
    ASSERT_THROW(OptionalField<int>(int_value) / OptionalField<int>(), std::logic_error);
    ASSERT_FALSE((OptionalField<int>() / OptionalField<int>(int_value)).has_value());

    // /* Both operands are non-null initialized */
    ASSERT_TRUE((OptionalField<int>(int_value) / OptionalField<int>(int_value)).has_value());
    ASSERT_EQ(OptionalField<int>(int_value / int_value), (OptionalField<int>(int_value) / OptionalField<int>(int_value)));

    /* Operator action for one OptionalField object and one template param. obj. */
    /* OptionalField object is null initialized */
    ASSERT_THROW(int_value / OptionalField<int>(), std::logic_error);
    ASSERT_FALSE((OptionalField<int>() / int_value).has_value());

    /* OptionalField object is non-null initialized */
    ASSERT_TRUE((int_value / OptionalField<int>(int_value)).has_value());
    ASSERT_TRUE((OptionalField<int>(int_value) / int_value).has_value());

    ASSERT_EQ(OptionalField<int>(int_value / int_value), int_value / OptionalField<int>(int_value));
    ASSERT_EQ(OptionalField<int>(int_value / int_value), OptionalField<int>(int_value) / int_value);
}

/*!
 * This test verifies that OptionalField<std::string> objects can be
 * concatenated in the same way as std::String objects.
 * */
TEST_F(OptionalFieldTest, StringConcatenation) {
    string first_string = "first string";
    string second_string = "second string";
    const char* second_cstring = "second cstring";

    /* Test empty string concatenation */
    ASSERT_FALSE((OptionalField<string>() + OptionalField<string>()).has_value());
    ASSERT_FALSE((OptionalField<string>() + OptionalField<const char*>()).has_value());

    /* Test empty-nonempty string concatenation */
    ASSERT_EQ(OptionalField<string>(first_string), OptionalField<string>(first_string) + OptionalField<string>());
    ASSERT_EQ(OptionalField<string>(second_string), OptionalField<string>() + OptionalField<string>(second_string));
    ASSERT_EQ(OptionalField<string>(first_string), OptionalField<string>(first_string) + OptionalField<const char*>());
    ASSERT_EQ(OptionalField<string>(second_cstring), OptionalField<string>() + OptionalField<const char*>(second_cstring));

    /* Test non-empty string concatenation */
    ASSERT_EQ(OptionalField<string>(first_string + second_string), OptionalField<string>(first_string) + OptionalField<string>(second_string));
    ASSERT_EQ(OptionalField<string>(first_string + second_cstring), OptionalField<string>(first_string) + OptionalField<const char*>(second_cstring));

    /* Test concatenation with std::string */
    ASSERT_EQ(OptionalField<string>(first_string + second_string), first_string + OptionalField<string>(second_string));
    ASSERT_EQ(OptionalField<string>(first_string + second_string), OptionalField<string>(first_string) + second_string);
}

/*!
 * This test verifies that comparison operators for floating-point OptionalField
 * objects are safe to use.
 * */
TEST_F(OptionalFieldTest, ComparisonOperatorsForDouble) {
    double large_double = 100.0;
    double epsilon_double = double_value + numeric_limits<double>::epsilon();

    /* Logical operators for equal numbers */
    ASSERT_TRUE(OptionalField<double>(double_value) == OptionalField<double>(double_value));
    ASSERT_FALSE(OptionalField<double>(double_value) != OptionalField<double>(double_value));

    /* Logical operators for small difference between numbers */
    ASSERT_FALSE(OptionalField<double>(double_value + epsilon_double) == OptionalField<double>(double_value));
    ASSERT_TRUE(OptionalField<double>(double_value + epsilon_double) != OptionalField<double>(double_value));

    /* Logical operators for large difference between numbers */
    ASSERT_FALSE(OptionalField<double>(double_value + large_double) == OptionalField<double>(double_value));
    ASSERT_TRUE(OptionalField<double>(double_value + large_double) != OptionalField<double>(double_value));
}

/*!
 * This test verifies the validity of stream insertion operator for
 * OptionalField objects.
 * */
TEST_F(OptionalFieldTest, InsertToOstream) {
    ostringstream test_stream;

    OptionalField<int> int_opfield(int_value);
    OptionalField<string> string_opfield(string_value);
    OptionalField<RushCollection> collection_opfield(RushCollection::TomSawyer);

    /* Test stream contents for initialized OptionalFields */
    test_stream << int_opfield;
    ASSERT_EQ(to_string(int_opfield.value()), test_stream.str());
    test_stream.str(string());

    test_stream << string_opfield;
    ASSERT_EQ(string_opfield.value(), test_stream.str());
    test_stream.str(string());

    test_stream << collection_opfield;
    ASSERT_EQ(RushCollection(RushCollection::TomSawyer).to_string(), test_stream.str());
    test_stream.str(string());

    /* Test stream contents for uninitialized OptionalFields */
    test_stream << OptionalField<int>();
    ASSERT_EQ(string("Value undefined!"), test_stream.str());
}
