/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * */

#include "agent-framework/module/model/attributes/fru_info.hpp"
#include "agent-framework/module/constants/common.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

FruInfo::FruInfo() { }

FruInfo::~FruInfo() { }

json::Json FruInfo::to_json() const {
    json::Json json = json::Json();
    json[literals::FruInfo::MANUFACTURER] = get_manufacturer();
    json[literals::FruInfo::PART] = get_part_number();
    json[literals::FruInfo::SERIAL] = get_serial_number();
    json[literals::FruInfo::MODEL] = get_model_number();
    return json;
}

FruInfo FruInfo::from_json(const json::Json& json) {
    attribute::FruInfo fru_info;
    fru_info.set_serial_number(json[literals::FruInfo::SERIAL]);
    fru_info.set_manufacturer(json[literals::FruInfo::MANUFACTURER]);
    fru_info.set_model_number(json[literals::FruInfo::MODEL]);
    fru_info.set_part_number(json[literals::FruInfo::PART]);
    return fru_info;
}
