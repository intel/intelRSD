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

#include <cstdio>

#include "configuration/schema_reader.hpp"
#include "configuration/schema_errors.hpp"
#include "configuration/schema_validator.hpp"
#include "configuration/utils.hpp"
#include "configuration/validators/validator.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "json-wrapper/json-wrapper.hpp"

using ::testing::Return;
using ::testing::Throw;

class SchemaReaderTest : public ::testing::Test {
protected:
    static constexpr char DEFAULT_VALIDATOR_JSON[] = R"({
        "logger" : {
            "rest" : {
                "level" : {
                    "validator" : true,
                    "type" : "string",
                    "anyof": ["DEBUG", "ERROR", "INFO"]
                }
            }
        },
        "eventing" : {
            "enabled" : {
                "validator" : true,
                "type" : "bool"
            },
            "poll-interval-sec" : {
                "validator" : true,
                "type" : "uint",
                "min" : 5
            }
        },
        "rest-server" : {
            "storage-service-mode" : {
                "validator" : true,
                "type" : "bool"
            }
        },
        "service" : {
            "validator" : true,
            "type" : "string"
        },
        "port" : {
            "validator": true,
            "max": 65535,
            "min": 0
        },
        "level": {
            "validator": true,
            "type": "string",
            "anyof": ["DEBUG", "ERROR", "INFO"]
        },
        "url": {
            "validator": true,
            "type": "string",
            "url": ""
        },
        "address": {
            "validator": true,
            "address": ""
        }
    })";
    virtual ~SchemaReaderTest();
};

constexpr char SchemaReaderTest::DEFAULT_VALIDATOR_JSON[];

SchemaReaderTest::~SchemaReaderTest() {}

/* Positive. */

TEST_F(SchemaReaderTest, Positive) {
    json::Json json_schema = json::Json();
    configuration::SchemaValidator validator;
    configuration::SchemaReader reader;
    configuration::string_to_json(DEFAULT_VALIDATOR_JSON, json_schema);
    ASSERT_EQ( reader.load_schema(json_schema, validator), true);
}
