/*!
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file set_processor_attributes.cpp
 * @brief Implementation of SetProcessorAttributes command
 * */


#include "command/set_processor_attributes.hpp"
#include "agent-framework/module/requests/validation/common.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/action/task_creator.hpp"
#include "fpga/fpga_secure_erase_task.hpp"
#include "utils.hpp"



using namespace agent_framework::model::requests::validation;
using namespace agent_framework::model::attribute;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::exceptions;
using namespace agent_framework;
using namespace agent::fpgaof::utils;
using namespace agent::fpgaof;

namespace {

void throw_if_processor_not_ready_for_secure_erase(const std::string& uuid) {
    auto processor = get_manager<Processor>().get_entry(uuid);

    if (processor.get_is_being_discovered()) {
        THROW(exceptions::FpgaofError, "fpgaof-gami", "processor is still being discovered");
    }
    if (processor.get_is_being_erased()) {
        THROW(exceptions::FpgaofError, "fpgaof-gami", "processor is still being erased");
    }
}


void securely_erase(AgentContext::SPtr ctx, const std::string& processor_uuid,
                    agent::fpgaof::SetComponentAttributes::Response& response) {
    log_info("fpgaof-gami", "Executing erase processor securely command.");

    throw_if_processor_not_ready_for_secure_erase(processor_uuid);

    auto response_builder = []() {
        agent::fpgaof::SetComponentAttributes::Response res{};
        return res.to_json();
    };

    action::TaskCreator task_creator{};
    task_creator.prepare_task();

    task_creator.add_prerun_action(std::bind(set_processor_is_being_erased, processor_uuid, true));

    task_creator.add_subtask(fpga::FpgaSecureEraseTask{ctx, processor_uuid});

    task_creator.add_exception_callback(std::bind(set_processor_status, processor_uuid,
                                                  attribute::Status{enums::State::StandbyOffline,
                                                                    enums::Health::Warning}));
    task_creator.add_exception_callback(std::bind(set_processor_is_in_warning_state, processor_uuid, true));
    task_creator.add_exception_callback(std::bind(set_processor_is_being_erased, processor_uuid, false));

    task_creator.set_promised_response(response_builder);

    task_creator.register_task();
    auto erase_processor_securely_task = task_creator.get_task();
    action::TaskRunner::get_instance().run(erase_processor_securely_task);

    response.set_task(task_creator.get_task_resource().get_uuid());
}

}


void agent::fpgaof::command::set_processor_attributes(AgentContext::SPtr ctx,
                                                      const Uuid& uuid,
                                                      const Attributes& attributes,
                                                      SetComponentAttributes::Response& response) {

    CommonValidator::validate_set_processor_attributes(attributes);
    const auto& attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("fpgaof-gami", "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);

        try {
            if (literals::Fpga::ERASED == name) {
                update_processor_erased(uuid, value.get<bool>());
                log_debug("fpgaof-gami", "Set " + name + " attribute to " << std::boolalpha << value.get<bool>());
            }
            else if (literals::Fpga::SECURELY_ERASE == name) {
                securely_erase(ctx, uuid, response);
            }
            else {
                THROW(UnsupportedField, "fpgaof-gami", "Setting attribute is not supported.", name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }
}
