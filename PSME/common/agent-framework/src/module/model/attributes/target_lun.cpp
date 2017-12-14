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
 * */

#include "agent-framework/module/model/attributes/target_lun.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

TargetLun::TargetLun() { }

TargetLun::~TargetLun() { }

json::Json TargetLun::to_json() const {
    json::Json json;
    json[literals::TargetLun::LUN] = get_lun();
    json[literals::TargetLun::LOGICAL_DRIVE] = get_logical_drive();
    return json;
}

TargetLun TargetLun::from_json(const json::Json& json) {
    TargetLun target_lun;
    target_lun.set_lun(json[literals::TargetLun::LUN]);
    target_lun.set_logical_drive(json[literals::TargetLun::LOGICAL_DRIVE]);
    return target_lun;
}
