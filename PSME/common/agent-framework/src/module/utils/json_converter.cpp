/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 *
 * @brief System model interface
 * */

#include "agent-framework/module/utils/json_converter.hpp"
#include "agent-framework/module/utils/optional_field.hpp"

namespace agent_framework {
namespace module {
namespace utils {

/* Template specializations to check if a json field is null */
template<>
bool JsonConverter<Json::Value>::is_null(const Json::Value& json) {
    return (Json::ValueType::nullValue == json.type() ? true : false );
}

template<>
bool JsonConverter<json::Value>::is_null(const json::Value& json) {
    return (json::Value::Type::NIL == json.get_type() ? true : false );
}

/* Template specialization for construction from Json::Value object */
template<>
template<>
OptionalField<double> JsonConverter<Json::Value>::from_json<double>(const Json::Value& json) {
    return (is_null(json) ? OptionalField<double>() : OptionalField<double>(json.asDouble()) );
}

template<>
template<>
OptionalField<std::string> JsonConverter<Json::Value>::from_json<std::string>(const Json::Value& json) {
    return (is_null(json) ? OptionalField<std::string>() : OptionalField<std::string>(json.asString()) );
}

template<>
template<>
OptionalField<int> JsonConverter<Json::Value>::from_json<int>(const Json::Value& json) {
    return (is_null(json) ? OptionalField<int>() : OptionalField<int>(json.asInt()) );
}

template<>
template<>
OptionalField<unsigned int> JsonConverter<Json::Value>::from_json<unsigned int>(const Json::Value& json) {
    return (is_null(json) ? OptionalField<unsigned int>() : OptionalField<unsigned int>(json.asUInt()) );
}

template<>
template<>
OptionalField<int64_t> JsonConverter<Json::Value>::from_json<int64_t>(const Json::Value& json) {
    return (is_null(json) ? OptionalField<int64_t>() : OptionalField<int64_t>(json.asInt64()) );
}

template<>
template<>
OptionalField<uint64_t> JsonConverter<Json::Value>::from_json<uint64_t>(const Json::Value& json) {
    return (is_null(json) ? OptionalField<uint64_t>() : OptionalField<uint64_t>(json.asUInt64()) );
}

template<>
template<>
OptionalField<bool> JsonConverter<Json::Value>::from_json<bool>(const Json::Value& json) {
    return (is_null(json) ? OptionalField<bool>() : OptionalField<bool>(json.asBool()) );
}

template<>
template<>
OptionalField<const char*> JsonConverter<Json::Value>::from_json<const char*>(const Json::Value& json) {
    return (is_null(json) ? OptionalField<const char*>() : OptionalField<const char*>(json.asCString()) );
}

/* Template specialization for construction from json::Value object */
template<>
template<>
OptionalField<int> JsonConverter<json::Value>::from_json<int>(const json::Value& json) {
    return (is_null(json) ? OptionalField<int>() : OptionalField<int>(json.as_int()) );
}

template<>
template<>
OptionalField<unsigned int> JsonConverter<json::Value>::from_json<unsigned int>(const json::Value& json) {
    return (is_null(json) ? OptionalField<unsigned int>() : OptionalField<unsigned int>(json.as_uint()) );
}

template<>
template<>
OptionalField<double> JsonConverter<json::Value>::from_json<double>(const json::Value& json) {
    return (is_null(json) ? OptionalField<double>() : OptionalField<double>(json.as_double()) );
}

template<>
template<>
OptionalField<bool> JsonConverter<json::Value>::from_json<bool>(const json::Value& json) {
    return (is_null(json) ? OptionalField<bool>() : OptionalField<bool>(json.as_bool()) );
}

template<>
template<>
OptionalField<const char*> JsonConverter<json::Value>::from_json<const char*>(const json::Value& json) {
    return (is_null(json) ? OptionalField<const char*>() : OptionalField<const char*>(json.as_char()) );;
}

template<>
template<>
OptionalField<std::string> JsonConverter<json::Value>::from_json<std::string>(const json::Value& json) {
    return (is_null(json) ? OptionalField<std::string>() : OptionalField<std::string>(json.as_string()) );
}

/* Template specializations to convert json to string */
template<>
std::string JsonConverter<Json::Value>::json_to_string(const Json::Value& json) {
    return json.asString();
}

template<>
std::string JsonConverter<json::Value>::json_to_string(const json::Value& json) {
    return json.as_string();
}



}
}
}
