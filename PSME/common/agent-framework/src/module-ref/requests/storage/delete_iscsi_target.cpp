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
 * */

#include "agent-framework/module-ref/requests/storage/delete_iscsi_target.hpp"
#include "agent-framework/module-ref/constants/storage.hpp"
#include <json/json.h>

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

DeleteIscsiTarget::DeleteIscsiTarget(const std::string& target): m_target{target} {}

Json::Value DeleteIscsiTarget::to_json() const {
    Json::Value value;
    value[IscsiTarget::TARGET] = m_target;
    return value;
}

DeleteIscsiTarget DeleteIscsiTarget::from_json(const Json::Value& json) {
    return DeleteIscsiTarget{
        json[IscsiTarget::TARGET].asString()};
}
