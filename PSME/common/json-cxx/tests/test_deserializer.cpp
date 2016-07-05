/*!
 * @copyright
 * Copyright (c) 2015, Tymoteusz Blazejczyk
 *
 * @copyright
 * All rights reserved.
 *
 * @copyright
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * @copyright
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * @copyright
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * @copyright
 * * Neither the name of json-cxx nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * @copyright
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * */

#include "gtest/gtest.h"
#include "json/json.hpp"

#include <iostream>

using namespace json;

class DeserializerTest : public ::testing::Test {
protected:
    Deserializer m_deserializer;

    virtual void SetUp() {

    }

    virtual void TearDown() {
        if (m_deserializer.is_invalid()) {
            Deserializer::Error error = m_deserializer.get_error();
            std::cerr << "Parsing invalid " << error.decode() << " at line "
                << error.line << " in column  " << error.column << std::endl;
        }
    }

    ~DeserializerTest();
};

DeserializerTest::~DeserializerTest() { }

TEST_F(DeserializerTest, PositiveSimpleObject) {
    Value value;

    m_deserializer << R"({"key":"test"})" >> value;

    EXPECT_TRUE(value.is_object());
    EXPECT_EQ(value.size(), 1);
    EXPECT_EQ(value["key"], "test");
}

TEST_F(DeserializerTest, PositiveSimpleArray) {
    Value value;

    m_deserializer << R"([0, 1, 2])" >> value;

    EXPECT_TRUE(value.is_array());
    EXPECT_EQ(value.size(), 3);
    EXPECT_EQ(value[0], 0);
    EXPECT_EQ(value[1], 1);
    EXPECT_EQ(value[2], 2);
}

TEST_F(DeserializerTest, PositiveSimpleString) {
    Value value;

    m_deserializer << R"("test")" >> value;

    EXPECT_TRUE(value.is_string());
    EXPECT_EQ(value.size(), 0);
    EXPECT_EQ(value.as_string().size(), 4);
    EXPECT_EQ(value, "test");
}

TEST_F(DeserializerTest, PositiveSimpleNumberUnsignedInteger) {
    Value value;

    m_deserializer << R"(13)" >> value;

    EXPECT_TRUE(value.is_number());
    EXPECT_TRUE(value.is_uint());
    EXPECT_EQ(value.size(), 0);
    EXPECT_EQ(value, 13);
}

TEST_F(DeserializerTest, PositiveSimpleNumberSignedInteger) {
    Value value;

    m_deserializer << R"(-13)" >> value;

    EXPECT_TRUE(value.is_number());
    EXPECT_TRUE(value.is_int());
    EXPECT_EQ(value.size(), 0);
    EXPECT_EQ(value, -13);
}

TEST_F(DeserializerTest, PositiveSimpleNumberUnsignedDouble) {
    Value value;

    m_deserializer << R"(3.17)" >> value;

    EXPECT_TRUE(value.is_number());
    EXPECT_TRUE(value.is_double());
    EXPECT_EQ(value.size(), 0);
    EXPECT_DOUBLE_EQ(value.as_double(), 3.17);
}

TEST_F(DeserializerTest, PositiveSimpleNumberSignedDouble) {
    Value value;

    m_deserializer << R"(-3.17)" >> value;

    EXPECT_TRUE(value.is_number());
    EXPECT_TRUE(value.is_double());
    EXPECT_EQ(value.size(), 0);
    EXPECT_DOUBLE_EQ(value.as_double(), -3.17);
}

TEST_F(DeserializerTest, PositiveSimpleTrue) {
    Value value;

    m_deserializer << R"(true)" >> value;

    EXPECT_TRUE(value.is_boolean());
    EXPECT_EQ(value.size(), 0);
    EXPECT_EQ(value, true);
}

TEST_F(DeserializerTest, PositiveSimpleFalse) {
    Value value;

    m_deserializer << R"(false)" >> value;

    EXPECT_TRUE(value.is_boolean());
    EXPECT_EQ(value.size(), 0);
    EXPECT_EQ(value, false);
}

TEST_F(DeserializerTest, PositiveSimpleNull) {
    Value value;

    m_deserializer << R"(null)" >> value;

    EXPECT_TRUE(value.is_null());
    EXPECT_EQ(value.size(), 0);
    EXPECT_EQ(value, nullptr);
}

TEST_F(DeserializerTest, NegativeExtTruee) {
    Value value;

    m_deserializer << R"(truee)" >> value;

    EXPECT_TRUE(m_deserializer.is_invalid());
    EXPECT_EQ(value, nullptr);
}

TEST_F(DeserializerTest, NegativeExtFalsee) {
    Value value;

    m_deserializer << R"(falsee)" >> value;

    EXPECT_TRUE(m_deserializer.is_invalid());
    EXPECT_EQ(value, nullptr);
}

TEST_F(DeserializerTest, NegativeExtNulll) {
    Value value;

    m_deserializer << R"(nulll)" >> value;

    EXPECT_TRUE(m_deserializer.is_invalid());
    EXPECT_EQ(value, nullptr);
}
