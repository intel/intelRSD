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
 *
 * @brief System Power State Thread
 * */

#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/enum/compute.hpp"
#include "agent-framework/module/model/attributes/event_data.hpp"
#include "ipmi/command/generic/get_chassis_status.hpp"
#include "status/system_power_state_thread.hpp"
#include "status/bmc.hpp"

using namespace ipmi;
using namespace ipmi::manager;
using namespace agent::compute::status;
using namespace agent_framework::model;
using namespace agent_framework::eventing;

using agent_framework::module::CommonComponents;
using agent::compute::Bmc;

namespace {

void notify(const std::string& parent_uuid, const std::string& system_uuid) {
    attribute::EventData event_data{};
    event_data.set_parent(parent_uuid);
    event_data.set_component(system_uuid);
    event_data.set_type(agent_framework::model::enums::Component::System);
    event_data.set_notification(enums::Notification::Update);
    EventsQueue::get_instance()->push_back(event_data);
}

enums::PowerState read_power_status(ipmi::IpmiController& ipmi) {
    ipmi::command::generic::request::GetChassisStatus request;
    ipmi::command::generic::response::GetChassisStatus response;
    ipmi.send(request, response);
    return (response.is_power_on() ? enums::PowerState::On : enums::PowerState::Off);
}

}

void agent::compute::status::update_system_power_state(Bmc& bmc) {
    log_debug("bmc", bmc.get_id() << " reading system power status");
    if (Bmc::State::ONLINE == bmc.get_state()) {
        try {
            auto power_status = read_power_status(bmc.ipmi());
            const auto system_uuids = CommonComponents::get_instance()->get_system_manager().get_keys(bmc.get_manager_uuid());
            log_debug("bmc", bmc.get_id() << " power status: " << power_status.to_string()
                    << " systems present: " << (system_uuids.empty() ? "no" : "yes"));
            for (const auto& system_uuid: system_uuids) {
                auto system_ref = CommonComponents::get_instance()->get_system_manager().get_entry_reference(system_uuid);
                const auto& last_power_status = system_ref->get_power_state();
                if (last_power_status != power_status) {
                    system_ref->set_power_state(power_status);
                    log_info("status", bmc.get_id() << " powered " << power_status.to_string()
                            << " system " << system_uuid);
                    notify(bmc.get_manager_uuid(), system_uuid);
                }
            }
        }
        catch (const ipmi::ResponseError& e) {
            log_warning("agent", "power status read failed: " << e.what());
        }
    }
}
