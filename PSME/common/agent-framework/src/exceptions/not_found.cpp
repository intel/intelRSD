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
 * @file not_found.cpp
 *
 * @brief Component not found exception implementation
 * */

#include "agent-framework/exceptions/not_found.hpp"



using namespace agent_framework::exceptions;

const constexpr char NotFound::URI[];

NotFound::NotFound(const std::string& msg) : GamiException(ErrorCode::NOT_FOUND, msg) {}


NotFound::NotFound(const std::string& msg, const std::string& uri) :
    GamiException(ErrorCode::NOT_FOUND, msg, create_json_data_from_uri(uri)) {}


NotFound::~NotFound() {}


std::string NotFound::get_uri_from_json_data(const json::Json& data, bool should_return_empty) {
    const auto& uri = get_string_from_data(data, NotFound::URI);
    if(uri.empty() && !should_return_empty) {
        return NOT_SPECIFIED;
    }
    return uri;
}

json::Json NotFound::create_json_data_from_uri(const std::string& uri) {
    json::Json json_data = json::Json();
    json_data[NotFound::URI] = uri;
    return json_data;
}
