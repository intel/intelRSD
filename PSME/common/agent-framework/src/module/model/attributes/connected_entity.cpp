/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 */

#include "agent-framework/module/model/attributes/connected_entity.hpp"
#include "agent-framework/module/constants/common.hpp"



using namespace agent_framework::model::attribute;
using namespace agent_framework::model;


ConnectedEntity::~ConnectedEntity() {}


json::Json ConnectedEntity::to_json() const {
    json::Json result = json::Json();
    result[literals::ConnectedEntity::ROLE] = get_entity_role();
    result[literals::ConnectedEntity::IDENTIFIERS] = get_identifiers().to_json();
    result[literals::ConnectedEntity::ENTITY] = get_entity();
    result[literals::ConnectedEntity::LUN] = get_lun();
    return result;
}


ConnectedEntity ConnectedEntity::from_json(const json::Json& json) {
    attribute::ConnectedEntity ce{};
    ce.set_entity_role(json[literals::ConnectedEntity::ROLE]);
    ce.set_identifiers(Identifiers::from_json(json[literals::ConnectedEntity::IDENTIFIERS]));
    ce.set_entity(json[literals::ConnectedEntity::ENTITY]);
    ce.set_lun(json.value(literals::ConnectedEntity::LUN, json::Json{}));
    return ce;
}
