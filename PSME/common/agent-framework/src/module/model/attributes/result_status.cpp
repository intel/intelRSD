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
 * */

#include "agent-framework/module/model/attributes/result_status.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::exceptions;
using namespace agent_framework::model::attribute;
using namespace agent_framework::model::literals;

ResultStatus::~ResultStatus() { }

json::Json ResultStatus::to_json() const {
    json::Json json = json::Json();
    json[Component::ATTRIBUTE] = get_attribute();
    json[Component::CODE] = static_cast<std::int32_t>(get_status_code());
    json[Component::MESSAGE] = get_message();
    return json;
}

ResultStatus ResultStatus::from_json(const json::Json& json) {
    ResultStatus result{};
    result.set_attribute(json[Component::ATTRIBUTE]);
    result.set_status_code(static_cast<ErrorCode>(json[Component::CODE].get<std::uint32_t>()));
    result.set_message(json[Component::MESSAGE]);
    return result;
}
