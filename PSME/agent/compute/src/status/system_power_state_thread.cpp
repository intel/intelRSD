/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @section System Power State Thread
*/

#include "agent-framework/eventing/event_data.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"
#include "agent-framework/module-ref/enum/compute.hpp"
#include "ipmi/command/generic/get_chassis_status.hpp"
#include "ipmi/management_controller.hpp"
#include "ipmi/manager/ipmitool/management_controller.hpp"
#include "status/system_power_state_thread.hpp"
#include "discovery/components/utils.hpp"

using namespace ipmi;
using namespace ipmi::manager;
using namespace agent::compute::discovery;
using namespace agent::compute::status;
using namespace agent_framework::model;
using namespace agent_framework::eventing;

using ComputeComponents = agent_framework::module::ComputeManager;
/*! Default system power state update interval */
constexpr const SystemPowerStateThread::Seconds
SystemPowerStateThread::DEFAULT_POWER_STATE_UPDATE_INTERVAL;

namespace {
Manager find_parent_manager(const string& system_uuid) {
    auto& sm = ComputeComponents::get_instance()->get_system_manager();
    auto system = sm.get_entry(system_uuid);

    auto& mm = ComputeComponents::get_instance()->get_module_manager();
    auto managers = mm.get_keys("", [&system](const Manager& manager) {
        return manager.get_uuid() == system.get_parent_uuid();
    });

    if (managers.empty()) {
        THROW(agent_framework::exceptions::InvalidUuid, "compute-agent",
              "Manger for System: '" + system_uuid + "' not found!");
    }

    return mm.get_entry(managers.front());
}

void set_connection_data(ManagementController& mc, const string& system_uuid) {
    auto manager = find_parent_manager(system_uuid);
    auto connection_data = manager.get_connection_data();
    mc.set_ip(connection_data.get_ip_address());
    mc.set_port(connection_data.get_port());
    mc.set_username(connection_data.get_username());
    mc.set_password(connection_data.get_password());
}

enums::PowerState get_ipmi_system_power_state(const std::string& system_uuid) {
    using namespace ipmi::command::generic;
    ipmi::manager::ipmitool::ManagementController mc{};
    auto& system_manager = ComputeComponents::get_instance()->get_system_manager();
    auto system = system_manager.get_entry(system_uuid);

    set_connection_data(mc, system_uuid);

    request::GetChassisStatus request;
    response::GetChassisStatus response;

    try {
        mc.send(request, response);
    }
    catch (const std::exception& e) {
        log_warning(GET_LOGGER("agent"), "Unable to send GetChassisStatus: "
                                                            << e.what());
        throw;
    }

    if (response.get_completion_code()) {
        log_error(GET_LOGGER("agent"), "Bad GetChassisStatus response: "
                  << to_string(unsigned(response.get_completion_code())));
    }

    return response.is_power_on() ? enums::PowerState::On : enums::PowerState::Off;
}

void notify(const std::string& system_uuid) {
    EventData event_data{};
    event_data.set_parent(ComputeComponents::get_instance()->
        get_system_manager().get_entry_reference(system_uuid)->get_parent_uuid());
    event_data.set_component(system_uuid);
    event_data.set_type(agent_framework::model::enums::Component::System);
    event_data.set_notification(Notification::Update);
    EventsQueue::get_instance()->push_back(event_data);
    log_info(GET_LOGGER("status"), "Adding system power state notification for "
            << "system uuid: " << system_uuid);
}

bool system_model_update(const std::string& system_uuid) {
    auto& system_manager = ComputeComponents::get_instance()->get_system_manager();
    if (!system_manager.entry_exists(system_uuid)){
        log_warning(GET_LOGGER("status"), "System: " << system_uuid
            << " is not present any more. ");
        return false;
    }
    auto system = system_manager.get_entry(system_uuid);
    const auto& last_power_state = system.get_power_state();
    try {
        enums::PowerState power_state = get_ipmi_system_power_state(system_uuid);
        if (last_power_state != power_state) {
            auto system_ref = system_manager.get_entry_reference(system_uuid);
            system_ref->set_power_state(power_state);
            log_info(GET_LOGGER("status"), "Power state for"
                << " system_uuid: " << system_uuid
                << " has been changed to: " << power_state);
            return true;
        }
    }
    catch (const std::exception& ex) {
        log_warning(GET_LOGGER("status"), "Unable to get system power state, system uuid: "
            << system_uuid <<". "<<ex.what());
    }
    return false;
}

void update_and_notify(const std::string& system_uuid) {
    bool is_model_updated{false};
    try {
        is_model_updated = system_model_update(system_uuid);
    }
    catch (const std::exception) {
            log_warning(GET_LOGGER("status"), "Unable to update system: "
                << system_uuid);
    }
    if (is_model_updated) {
        notify(system_uuid);
    }
}

}

void SystemPowerStateThread::start() {
    if (!m_running) {
        m_running = true;
        m_thread = std::thread(&SystemPowerStateThread::m_task, this);
    }
}

void SystemPowerStateThread::stop() {
    if (m_running) {
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
}

void SystemPowerStateThread::m_task() {
    log_debug(GET_LOGGER("status"), "System power state thread started.");

    while (m_running) {
        auto& system_manager = ComputeComponents::get_instance()->get_system_manager();
        for (const auto& system_uuid : system_manager.get_keys()) {
            update_and_notify(system_uuid);
        }
        std::this_thread::sleep_for(m_interval);
    }
    log_debug(GET_LOGGER("status"), "System power state thread stopped.");
}

SystemPowerStateThread::~SystemPowerStateThread() {
    stop();
}
