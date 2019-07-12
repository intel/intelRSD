/*!
 * @brief Processor builder class implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file processor_builder.cpp
 */

#include "discovery/builders/processor_builder.hpp"



using namespace agent::pnc::discovery::builders;
using namespace agent_framework::model::enums;
using namespace agent_framework::model;


ProcessorBuilder::~ProcessorBuilder() {}


void ProcessorBuilder::build_default() {
    attribute::Fpga fpga{};
    fpga.set_type(FpgaType::Discrete);
    fpga.set_erased(true);

    attribute::Status status{};
    status.set_state(enums::State::Enabled);
    status.set_health(enums::Health::OK);

    m_obj.set_processor_type(ProcessorType::FPGA);
    m_obj.set_processor_architecture(ProcessorArchitecture::OEM);
    m_obj.set_instruction_set(ProcessorInstructionSet::OEM);
    m_obj.set_fpga(fpga);
    m_obj.set_total_cores(1);
    m_obj.set_status(status);

    m_obj.add_collection(attribute::Collection(
        enums::CollectionName::Processors,
        enums::CollectionType::Processors
    ));

    m_obj.add_collection(attribute::Collection(
        enums::CollectionName::PcieFunctions,
        enums::CollectionType::PCIeFunctions
    ));
}


ProcessorBuilder::ReturnType ProcessorBuilder::add_dsp_port_uuid(const Uuid& dsp_port_uuid) {
    m_obj.add_dsp_port_uuid(dsp_port_uuid);
    return *this;
}


ProcessorBuilder::ReturnType ProcessorBuilder::update_critical_state() {
    attribute::Status status{};
    status.set_health(enums::Health::Critical);
    status.set_state(enums::State::Disabled);
    m_obj.set_status(std::move(status));
    return *this;
}


ProcessorBuilder::ReturnType ProcessorBuilder::update_fru_info(const attribute::FruInfo& fru_info) {
    m_obj.set_fru_info(fru_info);
    return *this;
}
