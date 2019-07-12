/*!
 * @brief Implementation of FPGA secure erase task.
 *
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
 * @file fpga_secure_erase_task.cpp
 */



#include "fpga/fpga_secure_erase_task.hpp"
#include "fpga/fpga_secure_erase.hpp"
#include "utils.hpp"

#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/module/model/attributes/event_data.hpp"



using namespace agent_framework::model;
using namespace agent_framework::eventing;
using namespace agent_framework::module;
using namespace agent::fpgaof;
using namespace agent::fpgaof::fpga;


FpgaSecureEraseTask::FpgaSecureEraseTask(const std::shared_ptr<AgentContext> agent_context, const Uuid& processor_uuid)
    : m_agent_context{agent_context}, m_processor_uuid{processor_uuid} {}


void FpgaSecureEraseTask::operator()() {
    log_info("agent", "Erasing FPGA processor securely started.");

    log_debug("agent", "Processor to be erased: " + m_processor_uuid);
    try {
        secure_erase(m_agent_context, m_processor_uuid);
        utils::update_processor_erased(m_processor_uuid, true);
        update_reconfigured_slot_details(m_agent_context, m_processor_uuid);
    }
    catch (const std::exception& e) {
        log_error("agent", "An error occurred while erasing processor: " << e.what());
        // throw task exception, processor status will by updated by the callback
        THROW(agent_framework::exceptions::FpgaofError, "agent", "Secure erase processor failed");
    }
    utils::set_processor_is_being_erased(m_processor_uuid, false);
    log_info("agent", "Erasing processor securely finished.");
}
