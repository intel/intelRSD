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

#include "agent-framework/module/compute_components.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/system/processor.hpp"

#include <json/json.hpp>
#include <regex>

using namespace psme::rest;
using namespace psme::rest::constants;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Systems/Members/__SYSTEM_ID__/Processors/Members/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Processor.v1_0_0.Processor";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Processor";
    r[Common::DESCRIPTION] = "Processor Description";
    r[Processor::SOCKET] = json::Value::Type::NIL;
    r[Common::MODEL] = json::Value::Type::NIL;
    r[Common::MANUFACTURER] = json::Value::Type::NIL;
    r[Processor::PROC_TYPE] = json::Value::Type::NIL;
    r[Processor::PROC_ARCHITECTURE] = json::Value::Type::NIL;
    r[Processor::INSTRUCTION_SET] = json::Value::Type::NIL;
    r[Processor::PROC_ID][Common::VENDOR_ID] = json::Value::Type::NIL;
    r[Processor::PROC_ID][Processor::IDENTIFICATION_REGISTERS] = json::Value::Type::NIL;
    r[Processor::PROC_ID][Processor::EFFECTIVE_FAMILY] = json::Value::Type::NIL;
    r[Processor::PROC_ID][Processor::EFFECTIVE_MODEL] = json::Value::Type::NIL;
    r[Processor::PROC_ID][Processor::STEP] = json::Value::Type::NIL;
    r[Processor::PROC_ID][Processor::MICROCODE_INFO] = json::Value::Type::NIL;
    r[Processor::MAX_SPEED] = json::Value::Type::NIL;
    r[Processor::TOTAL_CORES] = json::Value::Type::NIL;
    r[Processor::TOTAL_THREADS] = json::Value::Type::NIL;
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    json::Value rs;
    rs[Common::ODATA_TYPE] = "#Intel.Oem.Processor";
    rs[Processor::BRAND] = json::Value::Type::NIL;
    rs[Processor::CAPABILITIES] = json::Value::Type::ARRAY;

    r[Common::OEM][Common::RACKSCALE] = std::move(rs);

    return r;
}

}

endpoint::Processor::Processor(const std::string& path) : EndpointBase(path) {}
endpoint::Processor::~Processor() {}

void endpoint::Processor::get(const server::Request& req, server::Response& res) {
    auto json = make_prototype();

    auto system_id = psme::rest::model::Find<agent_framework::model::System>(req.params[PathParam::SYSTEM_ID])
        .get_one()->get_id();
    auto processor = psme::rest::model::Find<agent_framework::model::Processor>(req.params[PathParam::PROCESSOR_ID]).
        via<agent_framework::model::System>(req.params[PathParam::SYSTEM_ID]).get();

    json[Common::ODATA_ID] = PathBuilder(req).build();
    json[Common::ODATA_CONTEXT] = std::regex_replace(json[Common::ODATA_CONTEXT].as_string(),
                                                     std::regex("__SYSTEM_ID__"), std::to_string(system_id));
    json[constants::Common::ID] = req.params[PathParam::PROCESSOR_ID];
    json[constants::Processor::SOCKET] = processor.get_socket();
    json[constants::Common::MODEL] = processor.get_model_name();
    json[constants::Common::MANUFACTURER] = processor.get_manufacturer();
    json[constants::Processor::PROC_TYPE] = processor.get_processor_type();
    json[constants::Processor::PROC_ARCHITECTURE] = processor.get_processor_architecture();
    json[constants::Processor::INSTRUCTION_SET] = processor.get_instruction_set();

    endpoint::status_to_json(processor,json);
    json[Common::STATUS][Common::HEALTH_ROLLUP] = processor.get_status().get_health();

    json[Common::OEM][Common::RACKSCALE][constants::Processor::BRAND] = processor.get_model();
    auto& capabilities = processor.get_capabilities();
    for (auto& capability : capabilities) {
        json[Common::OEM][Common::RACKSCALE][constants::Processor::CAPABILITIES].push_back(json::Value(capability));
    }

    const auto& cpuid = processor.get_cpu_id();
    json[constants::Processor::PROC_ID][Common::VENDOR_ID] = cpuid.get_vendor_id();
    json[constants::Processor::PROC_ID][constants::Processor::IDENTIFICATION_REGISTERS] = cpuid.get_numeric_id();
    json[constants::Processor::PROC_ID][constants::Processor::EFFECTIVE_FAMILY] = cpuid.get_family();
    json[constants::Processor::PROC_ID][constants::Processor::EFFECTIVE_MODEL] = cpuid.get_model();
    json[constants::Processor::PROC_ID][constants::Processor::STEP] = cpuid.get_step();
    json[constants::Processor::PROC_ID][constants::Processor::MICROCODE_INFO] = cpuid.get_microcode_info();

    json[constants::Processor::MAX_SPEED] = processor.get_max_speed_mhz();
    json[constants::Processor::TOTAL_CORES] = processor.get_total_cores();
    json[constants::Processor::TOTAL_THREADS] = processor.get_total_threads();

    set_response(res, json);
}
