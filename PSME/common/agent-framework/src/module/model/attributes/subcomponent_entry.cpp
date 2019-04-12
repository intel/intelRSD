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
 * @brief Subcomponent Entry specific data
 * @file subcomponent_entry.cpp
 * */

#include "agent-framework/module/model/attributes/subcomponent_entry.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

SubcomponentEntry::SubcomponentEntry() { }

SubcomponentEntry::~SubcomponentEntry() { }

json::Json SubcomponentEntry::to_json() const {
    json::Json entry = json::Json();
    entry[literals::SubcomponentEntry::SUBCOMPONENT] = get_subcomponent();
    return entry;
}

SubcomponentEntry SubcomponentEntry::from_json(const json::Json& json) {
    SubcomponentEntry entry;
    entry.set_subcomponent(json[literals::SubcomponentEntry::SUBCOMPONENT]);
    return entry;
}
