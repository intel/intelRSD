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
*/

#include "agent-framework/module-ref/model/attributes/target_lun.hpp"
#include "agent-framework/module-ref/constants/storage.hpp"
#include <json/json.h>

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

TargetLun::TargetLun() { }

TargetLun::~TargetLun() { }

Json::Value TargetLun::to_json() const {
    Json::Value json;
    json[literals::TargetLun::LUN] = get_lun();
    json[literals::TargetLun::LOGICAL_DRIVE] = get_logical_drive();
    return json;
}

TargetLun TargetLun::from_json(const Json::Value& json) {
    TargetLun target_lun;
    target_lun.set_lun(json[literals::TargetLun::LUN].asUInt());
    target_lun.set_logical_drive(json[literals::TargetLun::LOGICAL_DRIVE].asString());
    return target_lun;
}
