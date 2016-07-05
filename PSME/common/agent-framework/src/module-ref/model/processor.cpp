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
 * */

#include "agent-framework/module-ref/model/processor.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"
#include "agent-framework/module-ref/enum/common.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Processor::component = enums::Component::Processor;
const enums::CollectionName Processor::collection_name =
    enums::CollectionName::Processors;

Processor::Processor(const std::string& parent_uuid) : Resource{parent_uuid,
    enums::Component::System} {}

Processor::~Processor() {}

Json::Value Processor::to_json() const {
    Json::Value result;
    result[literals::Processor::STATUS] = get_status().to_json();
    result[literals::Processor::SOCKET] = get_socket();
    result[literals::Processor::TYPE] = get_processor_type();
    result[literals::Processor::ARCHITECTURE] = get_processor_architecture();
    result[literals::Processor::INSTRUCTION_SET] = get_instruction_set();
    result[literals::Processor::CAPABILITIES] = get_capabilities().to_json();
    result[literals::Processor::MANUFACTURER] = get_manufacturer();
    result[literals::Processor::MODEL] = get_model();
    result[literals::Processor::MODEL_NAME] = get_model_name();
    result[literals::Processor::CPU_ID] = get_cpu_id().to_json();
    result[literals::Processor::MAX_SPEED] = get_max_speed_mhz();
    result[literals::Processor::TOTAL_CORES] = get_total_cores();
    result[literals::Processor::ENABLED_CORES] = get_enabled_cores();
    result[literals::Processor::TOTAL_THREADS] = get_total_threads();
    result[literals::Processor::ENABLED_THREADS] = get_enabled_threads();
    result[literals::Processor::OEM] = get_oem().to_json();
    return result;
}

Processor Processor::from_json(const Json::Value& json) {
    Processor processor;

    processor.set_status(attribute::Status::from_json(
        json[literals::Processor::STATUS]));
    processor.set_socket(json[literals::Processor::SOCKET]);
    processor.set_processor_type(
        json[literals::Processor::TYPE]);
    processor.set_processor_architecture(
        json[literals::Processor::ARCHITECTURE]);
    processor.set_instruction_set(
        json[literals::Processor::INSTRUCTION_SET]);
    processor.set_capabilities(
        Capabilities::from_json(
        json[literals::Processor::CAPABILITIES]));
    processor.set_manufacturer(
        json[literals::Processor::MANUFACTURER]);
    processor.set_model(
        json[literals::Processor::MODEL]);
    processor.set_model_name(
        json[literals::Processor::MODEL_NAME]);
    processor.set_cpu_id(attribute::CpuId::from_json(
        json[literals::Processor::CPU_ID]));
    processor.set_max_speed_mhz(
        json[literals::Processor::MAX_SPEED]);
    processor.set_total_cores(
        json[literals::Processor::TOTAL_CORES]);
    processor.set_enabled_cores(
        json[literals::Processor::ENABLED_CORES]);
    processor.set_total_threads(
        json[literals::Processor::TOTAL_THREADS]);
    processor.set_enabled_threads(
        json[literals::Processor::ENABLED_THREADS]);
    processor.set_oem(attribute::Oem::from_json(json[literals::Processor::OEM]));
    processor.set_resource_hash(compute_hash(json));

    return processor;
}
