/*!
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
bool JsonConverter<json::Json>::is_null(const json::Json& json) {
    return json.is_null();
}

/* Template specialization for construction from json::Json object */
template<>
template<>
OptionalField<double> JsonConverter<json::Json>::from_json<double>(const json::Json& json) {
    return (is_null(json) ? OptionalField<double>() : OptionalField<double>(json.get<double>()) );
}

template<>
template<>
OptionalField<std::string> JsonConverter<json::Json>::from_json<std::string>(const json::Json& json) {
    return (is_null(json) ? OptionalField<std::string>() : OptionalField<std::string>(json.get<std::string>()) );
}

template<>
template<>
OptionalField<int> JsonConverter<json::Json>::from_json<int>(const json::Json& json) {
    return (is_null(json) ? OptionalField<int>() : OptionalField<int>(json.get<int>()) );
}

template<>
template<>
OptionalField<unsigned int> JsonConverter<json::Json>::from_json<unsigned int>(const json::Json& json) {
    return (is_null(json) ? OptionalField<unsigned int>() : OptionalField<unsigned int>(json.get<unsigned int>()) );
}

template<>
template<>
OptionalField<int64_t> JsonConverter<json::Json>::from_json<int64_t>(const json::Json& json) {
    return (is_null(json) ? OptionalField<int64_t>() : OptionalField<int64_t>(json.get<std::int64_t>()) );
}

template<>
template<>
OptionalField<uint64_t> JsonConverter<json::Json>::from_json<uint64_t>(const json::Json& json) {
    return (is_null(json) ? OptionalField<uint64_t>() : OptionalField<uint64_t>(json.get<uint64_t>()) );
}

template<>
template<>
OptionalField<bool> JsonConverter<json::Json>::from_json<bool>(const json::Json& json) {
    return (is_null(json) ? OptionalField<bool>() : OptionalField<bool>(json.get<bool>()) );
}

template<>
template<>
OptionalField<const char*> JsonConverter<json::Json>::from_json<const char*>(const json::Json& json) {
    return (is_null(json) ? OptionalField<const char*>() : OptionalField<const char*>(json.get_ref<const std::string&>().c_str()));
}

/* Template specializations to convert json to string */
template<>
std::string JsonConverter<json::Json>::json_to_string(const json::Json& json) {
    return json;
}


}
}
}
