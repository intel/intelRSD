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
 * @file fpgaof_processor_builder.cpp
 */

#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "discovery/builders/fpgaof_processor_builder.hpp"
#include "agent-framework/module/model/system.hpp"
#include "utils.hpp"



using namespace agent::fpgaof::discovery::builders;
using namespace agent_framework::model::enums;
using namespace agent_framework::model;


Processor FpgaofProcessorBuilder::build_default(const Uuid& system_uuid) {
    attribute::Fpga fpga{};
    fpga.set_type(FpgaType::Discrete);
    fpga.set_erased(true);

    attribute::Status status{};
    status.set_state(enums::State::Enabled);
    status.set_health(enums::Health::OK);

    Processor processor{system_uuid};

    processor.set_processor_type(ProcessorType::FPGA);
    processor.set_processor_architecture(ProcessorArchitecture::OEM);
    processor.set_instruction_set(ProcessorInstructionSet::OEM);
    processor.set_fpga(fpga);
    processor.set_total_cores(1);
    processor.set_status(status);

    processor.add_collection(attribute::Collection(
        enums::CollectionName::Processors,
        enums::CollectionType::Processors
    ));

    processor.add_collection(attribute::Collection(
        enums::CollectionName::PcieFunctions,
        enums::CollectionType::PCIeFunctions
    ));

    return processor;
}
