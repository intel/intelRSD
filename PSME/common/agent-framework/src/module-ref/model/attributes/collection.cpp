/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 *
 * @brief Collection Entry specific data
 * */

#include "agent-framework/module-ref/model/attributes/collection.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include <json/json.h>

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

Collection::Collection() { }

Collection::~Collection() { }

Json::Value Collection::to_json() const {
    Json::Value entry;
    entry[literals::Collections::NAME] = get_name();
    entry[literals::Collections::TYPE] = get_type().to_string();
    entry[literals::Collections::SLOT_MASK] = get_slot_mask();
    return entry;
}

Collection Collection::from_json(const Json::Value& json)
{
    Collection entry;
    entry.set_name(
        json[literals::Collections::NAME].asString());
    entry.set_type(enums::CollectionType::from_string(
        json[literals::Collections::TYPE].asString()));
    entry.set_slot_mask(
        json[literals::Collections::SLOT_MASK].asString());
    return entry;
}
