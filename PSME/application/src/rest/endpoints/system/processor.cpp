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
#include "psme/rest/endpoints/system/processor.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;

namespace {

json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Processor.Processor";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Processor.v1_0_0.Processor";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Processor";
    r[Common::DESCRIPTION] = "Processor Description";
    r[Processor::SOCKET] = json::Value::Type::NIL;
    r[Common::MODEL] = json::Value::Type::NIL;
    r[Common::MANUFACTURER] = json::Value::Type::NIL;
    r[Processor::PROCESSOR_TYPE] = json::Value::Type::NIL;
    r[Processor::PROCESSOR_ARCHITECTURE] = json::Value::Type::NIL;
    r[Processor::INSTRUCTION_SET] = json::Value::Type::NIL;
    r[Processor::PROCESSOR_ID][Common::VENDOR_ID] = json::Value::Type::NIL;
    r[Processor::PROCESSOR_ID][Processor::IDENTIFICATION_REGISTERS] = json::Value::Type::NIL;
    r[Processor::PROCESSOR_ID][Processor::EFFECTIVE_FAMILY] = json::Value::Type::NIL;
    r[Processor::PROCESSOR_ID][Processor::EFFECTIVE_MODEL] = json::Value::Type::NIL;
    r[Processor::PROCESSOR_ID][Processor::STEP] = json::Value::Type::NIL;
    r[Processor::PROCESSOR_ID][Processor::MICROCODE_INFO] = json::Value::Type::NIL;
    r[Processor::MAX_SPEED] = json::Value::Type::NIL;
    r[Processor::TOTAL_CORES] = json::Value::Type::NIL;
    r[Processor::TOTAL_THREADS] = json::Value::Type::NIL;
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    json::Value rackscale{};
    rackscale[Common::ODATA_TYPE] = "#Intel.Oem.Processor";
    rackscale[Processor::BRAND] = json::Value::Type::NIL;
    rackscale[Processor::CAPABILITIES] = json::Value::Type::ARRAY;
    rackscale[Processor::EXTENDED_IDENTIFICATION_REGISTERS] = json::Value::Type::NIL;
    rackscale[Processor::ON_PACKAGE_MEMORY] = json::Value::Type::ARRAY;
    rackscale[Processor::THERMAL_DESIGN_POWER_WATT] = json::Value::Type::NIL;
    rackscale[Processor::FPGA] = json::Value::Type::NIL;
    rackscale[Common::METRICS] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE] = std::move(rackscale);

    return r;
}


void fill_fpga_oem(const agent_framework::model::Processor& processor, json::Value& json) {
    json::Value fpga{};
    fpga[constants::Processor::FPGA_TYPE] = processor.get_fpga().get_type();
    fpga[constants::Processor::BIT_STREAM_VERSION] = processor.get_fpga().get_bit_stream_version();
    fpga[constants::Processor::HSSI_CONFIGURATION] = processor.get_fpga().get_hssi_configuration();
    fpga[constants::Processor::HSSI_SIDEBAND] = processor.get_fpga().get_hssi_sideband();
    fpga[constants::Processor::RECONFIGURATION_SLOTS] = processor.get_fpga().get_reconfiguration_slots();
    json[Common::OEM][Common::RACKSCALE][constants::Processor::FPGA] = std::move(fpga);

    for (const auto& memory : processor.get_on_package_memory()) {
        json::Value p{};

        p[constants::Processor::MEMORY_TYPE] = memory.get_type();
        p[constants::Processor::MEMORY_CAPACITY_MB] = memory.get_capacity_mb();
        p[constants::Processor::SPEED_MHZ] = memory.get_speed_mhz();

        json[Common::OEM][Common::RACKSCALE][constants::Processor::ON_PACKAGE_MEMORY].push_back(std::move(p));
    }

    json[Common::OEM][Common::RACKSCALE][Processor::THERMAL_DESIGN_POWER_WATT] = processor.get_tdp_watt();
}

void fill_cpu_id(const agent_framework::model::Processor& processor, json::Value& json) {
    const auto& cpu_id = processor.get_cpu_id();
    json[constants::Processor::PROCESSOR_ID][Common::VENDOR_ID] = cpu_id.get_vendor_id();
    json[constants::Processor::PROCESSOR_ID][constants::Processor::IDENTIFICATION_REGISTERS] = cpu_id.get_numeric_id();
    json[constants::Processor::PROCESSOR_ID][constants::Processor::EFFECTIVE_FAMILY] = cpu_id.get_family();
    json[constants::Processor::PROCESSOR_ID][constants::Processor::EFFECTIVE_MODEL] = cpu_id.get_model();
    json[constants::Processor::PROCESSOR_ID][constants::Processor::STEP] = cpu_id.get_step();
    json[constants::Processor::PROCESSOR_ID][constants::Processor::MICROCODE_INFO] = cpu_id.get_microcode_info();

    json::Value extended_cpu_id{};
    extended_cpu_id[constants::ExtendedCpuId::EAX_00H] = processor.get_extended_cpu_id().get_eax_00h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_01H] = processor.get_extended_cpu_id().get_eax_01h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_02H] = processor.get_extended_cpu_id().get_eax_02h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_03H] = processor.get_extended_cpu_id().get_eax_03h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_04H] = processor.get_extended_cpu_id().get_eax_04h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_05H] = processor.get_extended_cpu_id().get_eax_05h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_06H] = processor.get_extended_cpu_id().get_eax_06h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_07H] = processor.get_extended_cpu_id().get_eax_07h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_09H] = processor.get_extended_cpu_id().get_eax_09h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_0AH] = processor.get_extended_cpu_id().get_eax_0ah();
    extended_cpu_id[constants::ExtendedCpuId::EAX_0BH] = processor.get_extended_cpu_id().get_eax_0bh();
    extended_cpu_id[constants::ExtendedCpuId::EAX_0DH] = processor.get_extended_cpu_id().get_eax_0dh();
    extended_cpu_id[constants::ExtendedCpuId::EAX_0FH] = processor.get_extended_cpu_id().get_eax_0fh();
    extended_cpu_id[constants::ExtendedCpuId::EAX_10H] = processor.get_extended_cpu_id().get_eax_10h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_14H] = processor.get_extended_cpu_id().get_eax_14h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_15H] = processor.get_extended_cpu_id().get_eax_15h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_16H] = processor.get_extended_cpu_id().get_eax_16h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_17H_ECX_00H] = processor.get_extended_cpu_id().get_eax_17h_ecx_00h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_17H_ECX_01H] = processor.get_extended_cpu_id().get_eax_17h_ecx_01h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_17H_ECX_02H] = processor.get_extended_cpu_id().get_eax_17h_ecx_02h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_17H_ECX_03H] = processor.get_extended_cpu_id().get_eax_17h_ecx_03h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_80000000H] = processor.get_extended_cpu_id().get_eax_80000000h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_80000001H] = processor.get_extended_cpu_id().get_eax_80000001h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_80000002H] = processor.get_extended_cpu_id().get_eax_80000002h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_80000003H] = processor.get_extended_cpu_id().get_eax_80000003h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_80000004H] = processor.get_extended_cpu_id().get_eax_80000004h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_80000006H] = processor.get_extended_cpu_id().get_eax_80000006h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_80000007H] = processor.get_extended_cpu_id().get_eax_80000007h();
    extended_cpu_id[constants::ExtendedCpuId::EAX_80000008H] = processor.get_extended_cpu_id().get_eax_80000008h();
    json[constants::Common::OEM][constants::Common::RACKSCALE]
        [constants::Processor::EXTENDED_IDENTIFICATION_REGISTERS] = std::move(extended_cpu_id);
}

}


endpoint::Processor::Processor(const std::string& path) : EndpointBase(path) {}


endpoint::Processor::~Processor() {}


void endpoint::Processor::get(const server::Request& req, server::Response& res) {
    auto json = make_prototype();

    auto processor = psme::rest::model::Find<agent_framework::model::Processor>(req.params[PathParam::PROCESSOR_ID])
        .via<agent_framework::model::System>(req.params[PathParam::SYSTEM_ID]).get();

    json[Common::ODATA_ID] = PathBuilder(req).build();
    json[constants::Common::ID] = req.params[PathParam::PROCESSOR_ID];
    json[constants::Processor::SOCKET] = processor.get_socket();
    json[constants::Common::MODEL] = processor.get_model_name();
    json[constants::Common::MANUFACTURER] = processor.get_manufacturer();
    json[constants::Processor::PROCESSOR_TYPE] = processor.get_processor_type();
    json[constants::Processor::PROCESSOR_ARCHITECTURE] = processor.get_processor_architecture();
    json[constants::Processor::INSTRUCTION_SET] = processor.get_instruction_set();

    endpoint::status_to_json(processor, json);
    json[Common::STATUS][Common::HEALTH_ROLLUP] = processor.get_status().get_health();

    json[Common::OEM][Common::RACKSCALE][constants::Processor::BRAND] = processor.get_model();
    endpoint::utils::string_array_to_json(
        json[Common::OEM][Common::RACKSCALE][constants::Processor::CAPABILITIES], processor.get_capabilities());

    if (agent_framework::model::enums::ProcessorType::FPGA == processor.get_processor_type()) {
        ::fill_fpga_oem(processor, json);
    }

    ::fill_cpu_id(processor, json);

    json[constants::Processor::MAX_SPEED] = processor.get_max_speed_mhz();
    json[constants::Processor::TOTAL_CORES] = processor.get_total_cores();
    json[constants::Processor::TOTAL_THREADS] = processor.get_total_threads();

    json[Common::OEM][Common::RACKSCALE][constants::Common::METRICS][Common::ODATA_ID] =
        PathBuilder(req).append(constants::Common::METRICS).build();

    set_response(res, json);
}
