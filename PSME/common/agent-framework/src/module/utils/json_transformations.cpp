/*!
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @file json_transformations.cpp
 */
#include "agent-framework/module/utils/json_transformations.hpp"
#include "generic/assertions.hpp"



json::Json agent_framework::model::utils::to_json_rpc(const json::Value& src) {
    switch (src.get_type()) {
        case json::Value::Type::NIL:
            return json::Json{};
        case json::Value::Type::OBJECT: {
            json::Json val = json::Json::object();
            for (const json::Pair& pair : src.as_object()) {
                val[pair.first] = to_json_rpc(pair.second);
            }
            return val;
        }
        case json::Value::Type::ARRAY: {
            json::Json val = json::Json::array();
            for (size_t i = 0; i < src.size(); i++) {
                val[static_cast<int>(i)] = to_json_rpc(src[i]);
            }
            return val;
        }
        case json::Value::Type::STRING:
            return json::Json(src.as_string());
        case json::Value::Type::NUMBER:
            if (src.is_int()) {
                return json::Json(src.as_int64());
            }
            if (src.is_uint()) {
                return json::Json(src.as_uint64());
            }
            return json::Json(src.as_double());
        case json::Value::Type::BOOLEAN:
            return json::Json(src.as_bool());

        default:
            assert(generic::FAIL("Unhandled jsoncxx value type"));
            return json::Json();
    }
}


json::Value agent_framework::model::utils::to_json_cxx(const json::Json& src) {
    switch (src.type()) {
        case json::Json::value_t::null:
            return json::Value();
        case json::Json::value_t::object: {
            json::Value val(json::Value::Type::OBJECT);
            for (auto it = src.begin(); it != src.end(); ++it) {
                val[it.key()] = to_json_cxx(it.value());
            }
            return val;
        }
        case json::Json::value_t::array: {
            json::Value val(json::Value::Type::ARRAY);
            for (auto elem : src) {
                val.push_back(to_json_cxx(elem));
            }
            return val;
        }
        case json::Json::value_t::string: {
            std::string value = src;
            return json::Value(value);
        }
        case json::Json::value_t::number_float: {
            double value = src;
            return json::Value(value);
        }
        case json::Json::value_t::number_integer: {
            std::int64_t value = src;
            return json::Value(value);
        }
        case json::Json::value_t::number_unsigned: {
            std::uint64_t value = src;
            return json::Value(value);
        }
        case json::Json::value_t::boolean:{
            bool value = src;
            return json::Value(value);
        }
        case json::Json::value_t::discarded:
        default:
            assert(generic::FAIL("Unhandled json value type"));
            return json::Value();
    }
}
