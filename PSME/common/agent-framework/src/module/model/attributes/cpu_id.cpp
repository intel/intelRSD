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
 * */

#include "agent-framework/module/model/attributes/cpu_id.hpp"
#include "agent-framework/module/constants/compute.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

CpuId::CpuId() { }

CpuId::~CpuId() { }

json::Json CpuId::to_json() const {
    json::Json result = json::Json();
    result[literals::CpuId::VENDOR_ID] = get_vendor_id();
    result[literals::CpuId::NUMERIC_ID] = get_numeric_id();
    result[literals::CpuId::FAMILY] = get_family();
    result[literals::CpuId::MODEL] = get_model();
    result[literals::CpuId::STEP] = get_step();
    result[literals::CpuId::MICROCODE_INFO] = get_microcode_info();
    return result;
}

CpuId CpuId::from_json(const json::Json& json) {
    CpuId cpu_id;
    cpu_id.set_vendor_id(json[literals::CpuId::VENDOR_ID]);
    cpu_id.set_numeric_id(json[literals::CpuId::NUMERIC_ID]);
    cpu_id.set_family(json[literals::CpuId::FAMILY]);
    cpu_id.set_model(json[literals::CpuId::MODEL]);
    cpu_id.set_step(json[literals::CpuId::STEP]);
    cpu_id.set_microcode_info(json[literals::CpuId::MICROCODE_INFO]);
    return cpu_id;
}
