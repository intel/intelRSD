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

#include "json/json.hpp"
#include "json/formatter/pretty.hpp"

#include <functional>
#include <iostream>

using namespace std;

void loop(const json::Value& val);

void loop(const json::Value& val) {
    for (auto it = val.cbegin(); val.cend() != it; it++) {
        cout << "Key: " << it.key() << " type: " << int(it->get_type()) << endl;
        loop(*it);
    }
}

int main(void) {
    json::Value val("Test");

    cout << std::string(val) << endl;

    val = json::Value(true);

    cout << bool(val) << endl;
    val = nullptr;

    cout << (val != nullptr) << endl;

    val = -1;

    cout << json::Int(val.get_type()) << endl;
    cout << json::Int(val) << endl;
    cout << (val == 2) << endl;

    val = nullptr;

    val.push_back(nullptr);
    val.push_back(6);
    val.push_back("Hello");
    val.push_back(val);

    cout << val.size() << endl;
    cout << int32_t(val[0]["test"].size()) << endl;

    val = nullptr;
    val["key1"] = 4;
    val["key2"];
    val["key3"] = -2;
    val["key4"].push_back(json::Pair("subtest1", 5));
    val["key4"].push_back(json::Pair("subtest2", true));
    val["key5"]["inkey4"] = "Test1";
    val["key5"]["inkey5"] = "Test2";

    json::Value val2("aa", 2);
    val2["key1"] = 5;
    val2["key2"] = {1, 3, 5};
    val2["key3"] = -7;
    val2["key4"].push_back(json::Pair("subtest1", 5));
    val2["key4"].push_back(json::Pair("subtest2", true));
    val2["key5"]["inkey4"] = "Test1";
    val2["key5"]["inkey5"] = "Test2";
    val2["key6"][0] = 3;
    val2["key6"][1] = 2;
    val2["key6"][2] = 1;
    val2["key6"][3] = 0;
    using Pair = json::Pair;
    (val2["key7"] = {Pair("a", 2), Pair("b", 3), Pair("b", 4)})[0];
    val2["key8"] = json::Value(5, nullptr);
    val2["key8"].assign(5, true);

    json::Value val3 = {
        Pair{"key1", 1},
        Pair{"key1", "test"}, // override previous key:value
        Pair{"key2", {-7, 2, "Text a"}},
        Pair{"key3", {2, true, nullptr}}
    };

    cout << "Serializer: " << json::Serializer(val) << endl;
    cout << "Serializer: " << json::Serializer(val2) << endl;
    val.swap(val2);
    cout << "Serializer: " << json::Serializer(val) << endl;
    cout << "Serializer: " << json::Serializer(val2) << endl;

    json::formatter::Pretty pretty;
    cout << "Serializer: " << json::Serializer(val3, &pretty) << endl;

    json::Value val4;
    json::Value val5;
    json::Value val6;

    val6["test1"] = 5;

    const json::Value& test1 = val6;
    const json::Value& test2 = val5;

    cout << "Test1: " << (test1["test1"] != nullptr) << endl;
    cout << "Test2: " << (test2["test2"] != nullptr) << endl;
    cout << "Test3: " << (test1.is_member("test1")) << endl;
    cout << "Test4: " << (test2.is_member("test2")) << endl;

    json::Deserializer deserializer(R"({
        "key1":true, "key3":

        false  , "ke5": [ {"a": 4}, [], 5,

               [2, true, 6], true, false  ], "key7":0.3e+4 })");

    deserializer << R"({"ad":true})";
    deserializer << R"({"face1": "\uD83D\uDE02"})";
    deserializer << R"([2, 4, 5])";
    deserializer << R"([{"":5}])";
    deserializer << R"({"face2":"😂" })";
    deserializer << R"({"face3": "\u01EC"})";
    deserializer << R"(4)";
    deserializer << R"("Simple string")";

    if (deserializer.is_invalid()) {
        auto error = deserializer.get_error();
        cout << "Line: " << error.line << " column: " << error.column
             << " offset: " << error.offset
             << " size: " << error.size << std::endl;
        cout << "Error: " << error.decode() << std::endl;
    }

    deserializer << R"(
    {
        "aa":2,
        "key1":5,
        "key2": [3, 3, 5],
        "key3":-7,
        "key4":{
            "subtest1":5,
            "subtest2":true
        },
        "key5": {
            "inkey4":"Test1",
            "inkey5":"Test2"
        },
        "key6":[ 3, 2, 1, 0],
        "key7":{"a":2,"b":4,"a":3e9},
        "key7":3,
        "key8":[
            true,
            true,
            true,
            true,
            true
        ]
    }
)";

    deserializer << R"(
{ "test": [
    "JSON Test Pattern pass1",
    {"object with 1 member":["array with 1 element"]},
    {},
    [],
    -42,
    true,
    false,
    null,
    {
        "integer": 1234567890,
        "real": -9876.543210,
        "e": 0.123456789e-12,
        "E": 1.234567890E+19,
        "":  23456789012E66,
        "zero": 0,
        "one": 1,
        "space": " ",
        "quote": "\"",
        "backslash": "\\",
        "controls": "\b\f\n\r\t",
        "slash": "/ & \/",
        "alpha": "abcdefghijklmnopqrstuvwyz",
        "ALPHA": "ABCDEFGHIJKLMNOPQRSTUVWYZ",
        "digit": "0123456789",
        "0123456789": "digit",
        "special": "`1~!@#$%^&*()_+-={':[,]}|;.</>?",
        "hex": "\u0123\u4567\u89AB\uCDEF\uabcd\uef4A",
        "true": true,
        "false": false,
        "null": null,
        "array":[  ],
        "object":{  },
        "address": "50 St. James Street",
        "url": "http://www.JSON.org/",
        "comment": "// /* <!-- --",
        "# -- --> */": " ",
        " s p a c e d " :[1,2 , 3

,

4 , 5        ,          6           ,7        ],"compact":[1,2,3,4,5,6,7],
        "jsontext": "{\"object with 1 member\":[\"array with 1 element\"]}",
        "quotes": "&#34; \u0022 %22 0x22 034 &#x22;",
        "\/\\\"\uCAFE\uBABE\uAB98\uFCDE\ubcda\uef4A\b\f\n\r\t`1~!@#$%^&*()_+-=[]{}|;:',./<>?"
: "A key can be any string"
    },
    0.5 ,98.6
,
99.44
,

1066,
1e1,
0.1e1,
1e-1,
1e00,2e+00,2e-00
,"rosebud"]})";

    if (deserializer.is_invalid()) {
        auto error = deserializer.get_error();
        cout << "Line: " << error.line << " column: " << error.column
             << " offset: " << error.offset
             << " size: " << error.size << std::endl;
        cout << "Error: " << error.decode() << std::endl;
    }

    json::Deserializer deserializer_copy(deserializer);

    cout << "Deserializer: ";
    while (!deserializer.empty()) {
        deserializer >> val4;
        cout << json::Serializer(val4, &pretty) << endl;
    }

    while (!deserializer_copy.empty()) {
        deserializer_copy >> val4;
        cout << "Keys: " << endl;
        loop(val4);
    }

    json::Value v;
    R"({"key":"value","number":10})" >> v;

    v["number"] += 1;

    cout << v << json::Value({Pair("a", 2), Pair("s", 2)}) << endl;

    auto it1 = v.begin();
    auto it2 = v.cbegin();

    cout << (it1 == it2) << endl;

    return 0;
}
