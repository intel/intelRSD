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
 *
 * @file processor.cpp
 *
 * @brief ipmi-way of discovering processor properties
 * */

#include "discovery/common.hpp"
#include "discovery/basic/processor.hpp"
#include "ipmi/management_controller.hpp"
#include "ipmi/command/sdv/get_processor_info.hpp"
#include "agent-framework/module/model/processor.hpp"
#include "agent-framework/module/compute_components.hpp"

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace ipmi::command;
using COMPLETION_CODE = ipmi::Response::COMPLETION_CODE;

namespace {

// As there are three ways of discovering processor properties (basic, smbios and deep discovery) and two of them (smbios
// and deep) return processor socket value in the form of a string "SOCKET #", it is necessary to slightly modify
// the value obtained from basic discovery
string get_socket_name(uint8_t processor_index) {
    std::string socket_name{"SOCKET " + std::to_string(unsigned(processor_index))};
    log_debug(GET_LOGGER("basic-discovery"), "Processor index: "
                                   << unsigned(processor_index)
                                   << " is translated to: "
                                   << socket_name);
    return socket_name;
}

}

namespace agent {
namespace compute {
namespace discovery {
namespace basic {

static constexpr const uint8_t HASSWEL_CPU_NO = 2;

std::tuple<bool, agent_framework::model::enums::Health> discover_processors(ipmi::ManagementController& mc, System& system) {
    log_debug(GET_LOGGER("basic-discovery"), "Send GetProcessorInfo.");
    sdv::request::GetProcessorInfo request{};
    sdv::response::GetProcessorInfo response{};
    enums::Health composite_health = enums::Health::OK;

    for (uint8_t cpu_index = 0; cpu_index < HASSWEL_CPU_NO; cpu_index++) {
        log_debug(GET_LOGGER("basic-discovery"), "Sending GetProcessorInfo. CPU: " << std::uint32_t(cpu_index));
        request.set_id(cpu_index);

        try {
            mc.send(request, response);
        }
        catch (const std::runtime_error& e) {
            composite_health = enums::Health::Warning;
            log_error(GET_LOGGER("basic-discovery"), "Cannot send command GetProcessorInfo: "
                                           << e.what()
                                           << " for System: "
                                           << system.get_uuid());
            continue;
        }

        const auto cc = response.get_completion_code();

        if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
            composite_health = enums::Health::Warning;
            log_bad_completion_code(system.get_uuid(), cc, "GetProcessorInfo");
            continue;
        }

        attribute::Status status{};
        Processor processor{system.get_uuid()};
        processor.set_socket(get_socket_name(cpu_index));
        log_debug(GET_LOGGER("basic-discovery"), "Discovering Processor (" << processor.get_socket() << ").");


        if (!response.is_present()) {
            log_warning(GET_LOGGER("basic-discovery"), "Processor (" << processor.get_socket() << ") is not present.");
            status.set_state(enums::State::Absent);
            status.set_health({}); // Set health to null
        }
        else {
            processor.set_max_speed_mhz(response.get_cpu_frequency());
            processor.set_processor_type(enums::ProcessorType::CPU);
            processor.set_model_name(response.get_cpu_type_name());
            status.set_state(enums::State::Enabled);
        }

        processor.set_status(status);
        ComputeComponents::get_instance()->get_processor_manager().add_entry(std::move(processor));
        log_debug(GET_LOGGER("basic-discovery"), "GetProcessorInfo successful.");
    }

    log_info(GET_LOGGER("basic-discovery"), "Processors Discovery was successful.");
    return std::make_tuple(enums::Health::OK == composite_health ? true : false, composite_health);
}


}
}
}
}
