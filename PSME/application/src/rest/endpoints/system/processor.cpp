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

#include "psme/rest/endpoints/system/processor.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/processor.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/responses/common.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

namespace {

json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Processor.Processor";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Processor.v1_3_0.Processor";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Processor";
    r[Common::DESCRIPTION] = "Processor Description";
    r[Processor::SOCKET] = json::Json::value_t::null;
    r[Common::MODEL] = json::Json::value_t::null;
    r[Common::MANUFACTURER] = json::Json::value_t::null;
    r[Processor::PROCESSOR_TYPE] = json::Json::value_t::null;
    r[Processor::PROCESSOR_ARCHITECTURE] = json::Json::value_t::null;
    r[Processor::INSTRUCTION_SET] = json::Json::value_t::null;
    r[Processor::PROCESSOR_ID][Common::VENDOR_ID] = json::Json::value_t::null;
    r[Processor::PROCESSOR_ID][Processor::IDENTIFICATION_REGISTERS] = json::Json::value_t::null;
    r[Processor::PROCESSOR_ID][Processor::EFFECTIVE_FAMILY] = json::Json::value_t::null;
    r[Processor::PROCESSOR_ID][Processor::EFFECTIVE_MODEL] = json::Json::value_t::null;
    r[Processor::PROCESSOR_ID][Processor::STEP] = json::Json::value_t::null;
    r[Processor::PROCESSOR_ID][Processor::MICROCODE_INFO] = json::Json::value_t::null;
    r[Processor::MAX_SPEED] = json::Json::value_t::null;
    r[Processor::TOTAL_CORES] = json::Json::value_t::null;
    r[Processor::TOTAL_THREADS] = json::Json::value_t::null;
    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;

    json::Json rackscale = json::Json();
    rackscale[Common::ODATA_TYPE] = "#Intel.Oem.Processor";
    rackscale[Processor::BRAND] = json::Json::value_t::null;
    rackscale[Processor::CAPABILITIES] = json::Json::value_t::array;
    rackscale[Processor::EXTENDED_IDENTIFICATION_REGISTERS] = json::Json::value_t::null;
    rackscale[Processor::INTEGRATED_MEMORY] = json::Json::value_t::array;
    rackscale[Processor::THERMAL_DESIGN_POWER_WATT] = json::Json::value_t::null;
    rackscale[Processor::FPGA] = json::Json::value_t::null;
    rackscale[Common::METRICS] = json::Json::value_t::null;
    r[Common::OEM][Common::RACKSCALE] = std::move(rackscale);

    r[Common::LINKS][Common::OEM][Common::RACKSCALE][constants::Processor::ENDPOINTS] = json::Json::value_t::array;

    return r;
}


void fill_fpga_oem(const agent_framework::model::Processor& processor, json::Json& json) {
    json::Json fpga = json::Json();
    fpga[constants::Fpga::FPGA_TYPE] = processor.get_fpga().get_type();

    if (processor.get_fpga().get_model().has_value()) {
        fpga[constants::Fpga::MODEL] = processor.get_fpga().get_model();
    }
    fpga[constants::Fpga::FW_ID] = processor.get_fpga().get_fw_id();
    fpga[constants::Fpga::FW_MANUFACTURER] = processor.get_fpga().get_fw_manufacturer();
    fpga[constants::Fpga::FW_VERSION] = processor.get_fpga().get_fw_version();
    if (processor.get_fpga().get_host_interface().has_value()) {
        fpga[constants::Fpga::HOST_INTERFACE] = processor.get_fpga().get_host_interface();
    }
    fpga[constants::Fpga::EXTERNAL_INTERFACES] = processor.get_fpga().get_external_interfaces().to_json();
    fpga[constants::Fpga::SIDEBAND_INTERFACE] = processor.get_fpga().get_sideband_interface();
    fpga[constants::Fpga::PCIE_VIRTUAL_FUNCTIONS] = processor.get_fpga().get_pcie_virtual_functions();
    fpga[constants::Fpga::PROGRAMMABLE_FROM_HOST] = processor.get_fpga().get_programmable_from_host();
    fpga[constants::Fpga::RECONFIGURATION_SLOTS_DETAILS] = processor.get_fpga().get_reconfiguration_slots_details().to_json();
    fpga[constants::Fpga::ERASED] = processor.get_fpga().get_erased();
    json[Common::OEM][Common::RACKSCALE][constants::Processor::FPGA] = std::move(fpga);

    for (const auto& memory : processor.get_integrated_memory()) {
        json::Json p = json::Json();

        p[constants::Processor::MEMORY_TYPE] = memory.get_type();
        p[constants::Processor::MEMORY_CAPACITY_MB] = memory.get_capacity_mb();
        p[constants::Processor::SPEED_MHZ] = memory.get_speed_mhz();
        json[Common::OEM][Common::RACKSCALE][constants::Processor::INTEGRATED_MEMORY].push_back(std::move(p));
    }

    json[Common::OEM][Common::RACKSCALE][Processor::THERMAL_DESIGN_POWER_WATT] = processor.get_tdp_watt();
}


void
fill_fpga_actions(const server::Request& req, json::Json& json) {
    json::Json actions = json::Json();
    actions[Common::OEM][constants::Fpga::HASH_SECURE_ERASE][Common::TARGET] = endpoint::PathBuilder(req).append(
        Common::ACTIONS).append(Common::OEM).append(constants::Fpga::SECURE_ERASE).build();
    json[Common::ACTIONS] = std::move(actions);
}


void fill_cpu_id(const agent_framework::model::Processor& processor, json::Json& json) {
    const auto& cpu_id = processor.get_cpu_id();
    json[constants::Processor::PROCESSOR_ID][Common::VENDOR_ID] = cpu_id.get_vendor_id();
    json[constants::Processor::PROCESSOR_ID][constants::Processor::IDENTIFICATION_REGISTERS] = cpu_id.get_numeric_id();
    json[constants::Processor::PROCESSOR_ID][constants::Processor::EFFECTIVE_FAMILY] = cpu_id.get_family();
    json[constants::Processor::PROCESSOR_ID][constants::Processor::EFFECTIVE_MODEL] = cpu_id.get_model();
    json[constants::Processor::PROCESSOR_ID][constants::Processor::STEP] = cpu_id.get_step();
    json[constants::Processor::PROCESSOR_ID][constants::Processor::MICROCODE_INFO] = cpu_id.get_microcode_info();

    json::Json extended_cpu_id = json::Json();
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


void fill_endpoint_links(const agent_framework::model::Processor& processor, json::Json& json) {
    auto& manager = agent_framework::module::get_manager<agent_framework::model::Endpoint>();
    for (const auto& endpoint_uuid : manager.get_keys()) {
        if (agent_framework::model::Endpoint::has_entity(endpoint_uuid, processor.get_uuid())) {
            json::Json endpoint_link = json::Json();
            endpoint_link[Common::ODATA_ID] = psme::rest::endpoint::utils::get_component_url(
                agent_framework::model::enums::Component::Endpoint, endpoint_uuid);
            json[Common::LINKS][Common::OEM][Common::RACKSCALE][constants::Processor::ENDPOINTS].push_back(
                std::move(endpoint_link));
        }
    }
}


static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::Fpga::ERASED, endpoint::PathBuilder(Common::OEM)
                                                         .append(Common::RACKSCALE)
                                                         .append(constants::Processor::FPGA)
                                                         .append(constants::Fpga::ERASED)
                                                         .build()
    }
};

}


endpoint::Processor::Processor(const std::string& path) : EndpointBase(path) {}


endpoint::Processor::~Processor() {}


void endpoint::Processor::get(const server::Request& req, server::Response& res) {
    auto json = make_prototype();

    auto processor = psme::rest::model::find<agent_framework::model::System, agent_framework::model::Processor>(
        req.params).get();

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
        if (!utils::has_resource_capability(processor, Capability::COMPUTE)) {
            ::fill_fpga_actions(req, json);
        }
    }

    ::fill_cpu_id(processor, json);

    json[constants::Processor::MAX_SPEED] = processor.get_max_speed_mhz();
    json[constants::Processor::TOTAL_CORES] = processor.get_total_cores();
    json[constants::Processor::TOTAL_THREADS] = processor.get_total_threads();

    json[Common::OEM][Common::RACKSCALE][constants::Common::METRICS][Common::ODATA_ID] =
        PathBuilder(req).append(constants::PathParam::OEM_INTEL_RACKSCALE).append(constants::Common::METRICS).build();

    ::fill_endpoint_links(processor, json);

    set_response(res, json);
}


void endpoint::Processor::patch(const server::Request& req, server::Response& res) {
    static const constexpr char TRANSACTION_NAME[] = "PatchProcessor";

    auto processor = psme::rest::model::find<agent_framework::model::System, agent_framework::model::Processor>(
        req.params).get();
    agent_framework::model::attribute::Attributes attributes{};

    const auto& json = JsonValidator::validate_request_body<schema::ProcessorPatchSchema>(req);

    json::Json js_fpga = json.value(Common::OEM, json::Json::object()).value(Common::RACKSCALE,
                                                                             json::Json::object()).value(
        constants::Processor::FPGA, json::Json::object());

    if (js_fpga.value(constants::Fpga::ERASED, json::Json()).is_boolean()) {
        bool fpga_erased = json[Common::OEM][Common::RACKSCALE][constants::Processor::FPGA][constants::Fpga::ERASED];
        attributes.set_value(agent_framework::model::literals::Fpga::ERASED, fpga_erased);
    }

    if (!attributes.empty()) {
        agent_framework::model::requests::SetComponentAttributes set_component_attributes_request{processor.get_uuid(),
                                                                                                  attributes};

        const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(processor.get_agent_id());

        auto set_processor_attributes = [&, gami_agent] {
            const auto& set_component_attributes_response = gami_agent->
                execute<agent_framework::model::responses::SetComponentAttributes>(set_component_attributes_request);

            const auto& result_statuses = set_component_attributes_response.get_statuses();
            if (!result_statuses.empty()) {
                const auto& error = error::ErrorFactory::create_error_from_set_component_attributes_results(
                    result_statuses, gami_to_rest_attributes);
                throw error::ServerException(error);
            }

            psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                agent_framework::model::enums::Component::Processor)->load(gami_agent, processor.get_parent_uuid(),
                                                                           agent_framework::model::enums::Component::System,
                                                                           processor.get_uuid(), false);
        };

        gami_agent->execute_in_transaction(TRANSACTION_NAME, set_processor_attributes);
    }
    get(req, res);
}