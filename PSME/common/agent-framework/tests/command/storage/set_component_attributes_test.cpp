/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
 * */

#include "agent-framework/command-ref/storage_commands.hpp"
#include "json-wrapper/json-wrapper.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command_ref;

class TestComponentAttributes {
public:
    TestComponentAttributes() {}

    void set_name(const std::string& name) {
        m_name = name;
    }

    const std::string& get_name() const {
        return m_name;
    }

    void set_string_value(const std::string& value) {
        m_string_value = value;
    }

    const std::string& get_string_value() const {
        return m_string_value;
    }

    void set_number_value(double value) {
        m_number_value = value;
    }

    double get_number_value() const {
        return m_number_value;
    }

    void set_boolean_value(bool value) {
        m_boolean_value = value;
    }

    bool get_boolean_value() const {
        return m_boolean_value;
    }

    void set_array_string_value(const std::vector<std::string>& value) {
        m_array_string_value = value;
    }

    const std::vector<std::string>& get_array_string_value() const {
        return m_array_string_value;
    }

    void set_array_number_value(const std::vector<double>& value) {
        m_array_number_value = value;
    }

    const std::vector<double>& get_array_number_value() const {
        return m_array_number_value;
    }

    void set_array_boolean_value(const std::vector<bool>& value) {
        m_array_boolean_value = value;
    }

    const std::vector<bool>& get_array_boolean_value() const {
        return m_array_boolean_value;
    }

    virtual ~TestComponentAttributes();
private:
    std::string m_name;
    std::string m_string_value;
    double m_number_value;
    bool m_boolean_value;
    std::vector<std::string> m_array_string_value;
    std::vector<double> m_array_number_value;
    std::vector<bool> m_array_boolean_value;
};

TestComponentAttributes::~TestComponentAttributes() { }

class MySetComponentAttributes {
private:
    std::string m_component{};
    TestComponentAttributes* m_component_attributes;
public:
    MySetComponentAttributes(
        const std::string& component,
        TestComponentAttributes* component_attributes) {
        m_component = component;
        m_component_attributes = component_attributes;
        }

    void execute(const SetComponentAttributes::Request& request,
                 SetComponentAttributes::Response&) {
        auto component = request.get_component();
        auto attributes = request.get_attributes();

        if (component != m_component) {
            throw std::runtime_error("Not found");
        }

        for (const auto& json_value : attributes) {
          if (json_value.get_name() == "boolean")
                m_component_attributes->set_boolean_value(json_value.get_boolean_value());
          else if (json_value.get_name() == "string")
                m_component_attributes->set_string_value(json_value.get_string_value());
          else if (json_value.get_name() == "number")
                m_component_attributes->set_number_value(json_value.get_number_value());
          else if (json_value.get_name() == "arrayNumber")
                m_component_attributes->set_array_number_value(json_value.get_array_number_value());
          else if (json_value.get_name() == "arrayString")
                m_component_attributes->set_array_string_value(json_value.get_array_string_value());
          else if (json_value.get_name() == "arrayBoolean")
                m_component_attributes->set_array_boolean_value(json_value.get_array_boolean_value());
        }
    }
};

static constexpr char COMPONENT[] = "component";
static constexpr char ATTRIBUTES[] = "attributes";
static constexpr char BOOLEAN[] = "boolean";
static constexpr char STRING[] = "string";
static constexpr char NUMBER[] = "number";
static constexpr char ARRAY_BOOLEAN[] = "arrayBoolean";
static constexpr char ARRAY_STRING[] = "arrayString";
static constexpr char ARRAY_NUMBER[] = "arrayNumber";
static constexpr char NAME[] = "name";
static constexpr char VALUE[] = "value";

TEST(SetComponentAttributesTest, PositiveExecuteBooleanValue) {
    TestComponentAttributes test_component_attributes;
    MySetComponentAttributes command{"TestModule", &test_component_attributes};
    SetComponentAttributes::Request request{""};
    SetComponentAttributes::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();
    json::Json attributes = json::Json();
    json::Json json_attribute = json::Json();
    json::Json boolean_value = json::Json();
    boolean_value[BOOLEAN] = true;
    json_attribute[NAME] = BOOLEAN;
    json_attribute[VALUE] = boolean_value;
    attributes.append(json_attribute);

    params[COMPONENT] = "TestModule";
    params[ATTRIBUTES] = attributes;

    EXPECT_NO_THROW(request = SetComponentAttributes::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_EQ(test_component_attributes.get_boolean_value(), true);
}

TEST(SetComponentAttributesTest, PositiveExecuteStringValue) {
    TestComponentAttributes test_component_attributes;
    MySetComponentAttributes command{"TestModule", &test_component_attributes};
    SetComponentAttributes::Request request{""};
    SetComponentAttributes::Response response{};

    json::Json params = json::Json();
    json::Json result = json::Json();
    json::Json attributes = json::Json();
    json::Json json_attribute = json::Json();
    json::Json string_value = json::Json();
    string_value[STRING] = "TestString";
    json_attribute[NAME] = STRING;
    json_attribute[VALUE] = string_value;
    attributes.append(json_attribute);

    params[COMPONENT] = "TestModule";
    params[ATTRIBUTES] = attributes;

    EXPECT_NO_THROW(request = SetComponentAttributes::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_EQ(test_component_attributes.get_string_value(), "TestString");
}

TEST(SetComponentAttributesTest, PositiveExecuteNumberValue) {
    TestComponentAttributes test_component_attributes;
    MySetComponentAttributes command{"TestModule", &test_component_attributes};
    SetComponentAttributes::Request request{""};
    SetComponentAttributes::Response response{};

    json::Json params = json::Json();
    json::Json result = json::Json();
    json::Json attributes = json::Json();
    json::Json json_attribute = json::Json();
    json::Json number_value = json::Json();
    number_value[NUMBER] = 1.0;
    json_attribute[NAME] = NUMBER;
    json_attribute[VALUE] = number_value;
    attributes.append(json_attribute);

    params[COMPONENT] = "TestModule";
    params[ATTRIBUTES] = attributes;

    EXPECT_NO_THROW(request = SetComponentAttributes::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_EQ(test_component_attributes.get_number_value(), 1.0);
}

TEST(SetComponentAttributesTest, PositiveExecuteArrayString) {
    TestComponentAttributes test_component_attributes;
    MySetComponentAttributes command{"TestModule", &test_component_attributes};
    SetComponentAttributes::Request request{""};
    SetComponentAttributes::Response response{};

    json::Json params = json::Json();
    json::Json result = json::Json();
    json::Json attributes = json::Json();
    json::Json json_attribute = json::Json();
    json::Json json_array = json::Json();
    json::Json array_string_value = json::Json();
    json_array[0] = "FirstTestString";
    json_array[1] = "SecondTestString";
    array_string_value[ARRAY_STRING] = json_array;
    json_attribute[NAME] = ARRAY_STRING;
    json_attribute[VALUE] = array_string_value;
    attributes.append(json_attribute);

    params[COMPONENT] = "TestModule";
    params[ATTRIBUTES] = attributes;

    EXPECT_NO_THROW(request = SetComponentAttributes::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_EQ(test_component_attributes.get_array_string_value().size(), 2);
    ASSERT_EQ(test_component_attributes.get_array_string_value()[0], "FirstTestString");
    ASSERT_EQ(test_component_attributes.get_array_string_value()[1], "SecondTestString");
}

TEST(SetComponentAttributesTest, PositiveExecuteArrayNumber) {
    TestComponentAttributes test_component_attributes;
    MySetComponentAttributes command{"TestModule", &test_component_attributes};
    SetComponentAttributes::Request request{""};
    SetComponentAttributes::Response response{};

    json::Json params = json::Json();
    json::Json result = json::Json();
    json::Json attributes = json::Json();
    json::Json json_attribute = json::Json();
    json::Json json_array = json::Json();
    json::Json array_number_value = json::Json();
    json_array[0] = 0.0;
    json_array[1] = 1.0;
    array_number_value[ARRAY_NUMBER] = json_array;
    json_attribute[NAME] = ARRAY_NUMBER;
    json_attribute[VALUE] = array_number_value;
    attributes.append(json_attribute);

    params[COMPONENT] = "TestModule";
    params[ATTRIBUTES] = attributes;

    EXPECT_NO_THROW(request = SetComponentAttributes::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_EQ(test_component_attributes.get_array_number_value().size(), 2);
    ASSERT_EQ(test_component_attributes.get_array_number_value()[0], 0.0);
    ASSERT_EQ(test_component_attributes.get_array_number_value()[1], 1.0);
}

TEST(SetComponentAttributesTest, PositiveExecuteArrayBoolean) {
    TestComponentAttributes test_component_attributes;
    MySetComponentAttributes command{"TestModule", &test_component_attributes};
    SetComponentAttributes::Request request{""};
    SetComponentAttributes::Response response{};

    json::Json params = json::Json();
    json::Json result = json::Json();
    json::Json attributes = json::Json();
    json::Json json_attribute = json::Json();
    json::Json json_array = json::Json();
    json::Json array_boolean_value = json::Json();
    json_array[0] = true;
    json_array[1] = false;
    array_boolean_value[ARRAY_BOOLEAN] = json_array;
    json_attribute[NAME] = ARRAY_BOOLEAN;
    json_attribute[VALUE] = array_boolean_value;
    attributes.append(json_attribute);

    params[COMPONENT] = "TestModule";
    params[ATTRIBUTES] = attributes;

    EXPECT_NO_THROW(request = SetComponentAttributes::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_EQ(test_component_attributes.get_array_boolean_value().size(), 2);
    ASSERT_EQ(test_component_attributes.get_array_boolean_value()[0], true);
    ASSERT_EQ(test_component_attributes.get_array_boolean_value()[1], false);
}

TEST(SetComponentAttributesTest, NegativeNotFound) {
    TestComponentAttributes test_component_attributes;
    MySetComponentAttributes command{"TestModule", &test_component_attributes};
    SetComponentAttributes::Request request{""};
    SetComponentAttributes::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();
    json::Json attributes = json::Json();

    params[COMPONENT] = "OtherTestModule";
    params[ATTRIBUTES] = attributes;

    EXPECT_NO_THROW(request = SetComponentAttributes::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
}
