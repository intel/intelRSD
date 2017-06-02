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
 * @brief Identifier attribute
 * @file identifier.cpp
 * */

#include "agent-framework/module/model/attributes/identifier.hpp"
#include "agent-framework/module/constants/common.hpp"
#include <json/json.h>



using namespace agent_framework::model::attribute;


Identifier::Identifier() { }


Identifier::~Identifier() { }


Json::Value Identifier::to_json() const {
    Json::Value json;
    json[literals::Identifier::DURABLE_NAME] = get_durable_name();
    json[literals::Identifier::DURABLE_NAME_FORMAT] = get_durable_name_format().to_string();
    return json;
}


Identifier Identifier::from_json(const Json::Value& json) {
    attribute::Identifier location;
    location.set_durable_name(json[literals::Identifier::DURABLE_NAME].asString());
    location.set_durable_name_format(
        enums::IdentifierType::from_string(json[literals::Identifier::DURABLE_NAME_FORMAT].asString()));
    return location;
}
