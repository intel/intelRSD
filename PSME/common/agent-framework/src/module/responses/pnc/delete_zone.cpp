/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @file responses/pnc/delete_zone.cpp
 * @brief DeleteZone response
 * */

#include "agent-framework/module/responses/pnc/delete_zone.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "agent-framework/module/constants/common.hpp"

#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::responses;
using namespace agent_framework::model::literals;

DeleteZone::DeleteZone(const attribute::Oem& oem):
    m_oem{oem} {}


json::Json DeleteZone::to_json() const {
    json::Json value;
    value[Zone::OEM] = m_oem.to_json();
    return value;
}

DeleteZone DeleteZone::from_json(const json::Json& json) {
    return DeleteZone{attribute::Oem::from_json(json[Zone::OEM])};
}
