/*!
 * @brief Grantley processor builder class implementation.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file grantley_processor_builder.hpp
 */


#include "discovery/builders/platform_specific/grantley/grantley_processor_builder.hpp"

#include <bitset>



using namespace agent::compute::discovery;
using namespace smbios::parser;
using namespace agent_framework::model;


void GrantleyProcessorBuilder::update_processor_info(agent_framework::model::Processor& processor,
                                                     const ipmi::command::sdv::response::GetProcessorInfo& get_processor_info_response) {
    auto status = processor.get_status();

    if (!get_processor_info_response.is_present()) {
        status.set_state(enums::State::Absent);
        status.set_health({});
    }
    else {
        processor.set_max_speed_mhz(get_processor_info_response.get_cpu_frequency());
        processor.set_processor_type(enums::ProcessorType::CPU);
        processor.set_model_name(get_processor_info_response.get_cpu_type_name());
        status.set_state(enums::State::Enabled);
    }

    processor.set_status(status);
}


void GrantleyProcessorBuilder::update_socket_id(agent_framework::model::Processor& processor, std::uint8_t socket_id) {
    processor.set_socket("SOCKET " + std::to_string(static_cast<unsigned>(socket_id)));
}
