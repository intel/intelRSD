/*!
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
 *
 * @file responses/storage/delete_iscsi_target.cpp
 * @brief DeleteLogicalDrive response
 * */

#include "agent-framework/module-ref/responses/storage/delete_iscsi_target.hpp"
#include "agent-framework/module-ref/constants/storage.hpp"
#include <json/json.h>

using namespace agent_framework::model::responses;
using namespace agent_framework::model::literals;

DeleteIscsiTarget::DeleteIscsiTarget(Oem oem):
    m_oem{oem} {}


Json::Value DeleteIscsiTarget::to_json() const {
    Json::Value value;
    value[IscsiTarget::OEM] = m_oem.to_json();
    return value;
}

DeleteIscsiTarget DeleteIscsiTarget::from_json
    (const Json::Value& json) {
    return DeleteIscsiTarget{Oem::from_json(
        json[IscsiTarget::OEM])};
}
