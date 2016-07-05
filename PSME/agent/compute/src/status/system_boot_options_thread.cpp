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
 * @section System Boot Options Thread
*/

#include "agent-framework/eventing/event_data.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"
#include "agent-framework/module-ref/enum/compute.hpp"
#include "ipmi/management_controller.hpp"
#include "status/system_boot_options_thread.hpp"
#include "discovery/components/utils.hpp"
#include "ipmi/manager/ipmitool/management_controller.hpp"
#include "ipmi/command/generic/get_system_boot_options.hpp"

using namespace ipmi;
using namespace ipmi::manager;
using namespace ipmi::command::generic;
using namespace agent::compute::discovery;
using namespace agent::compute::status;
using namespace agent_framework::model;
using namespace agent_framework::eventing;

using ComputeComponents = agent_framework::module::ComputeManager;
/*! Default system boot options update interval */
constexpr const SystemBootOptionsThread::Seconds
SystemBootOptionsThread::DEFAULT_BOOT_OPTIONS_UPDATE_INTERVAL;

namespace {

    using SystemReference = agent_framework::generic::ObjReference<System, std::recursive_mutex>;

    enums::BootOverrideTarget get_boot_override_target(
            response::GetSystemBootOptions::BootOverrideTarget target) {
        switch (target) {
            case response::GetSystemBootOptions::BootOverrideTarget::Hdd:
                return enums::BootOverrideTarget::Hdd;
            case response::GetSystemBootOptions::BootOverrideTarget::Pxe:
                return enums::BootOverrideTarget::Pxe;
            case response::GetSystemBootOptions::BootOverrideTarget::None:
            case response::GetSystemBootOptions::BootOverrideTarget::Other:
            default:
                return enums::BootOverrideTarget::None;
        }
    }

    enums::BootOverride get_boot_override(
            response::GetSystemBootOptions::BootOverride override) {
        switch (override) {
            case response::GetSystemBootOptions::BootOverride::Once:
                return enums::BootOverride::Once;
            case response::GetSystemBootOptions::BootOverride::Continuous:
                return enums::BootOverride::Continuous;
            case response::GetSystemBootOptions::BootOverride::Disabled:
            default:
                return enums::BootOverride::Disabled;
        }
    }

    SystemReference get_system_reference(const string& system_uuid) {
        try {
            auto system = ComputeComponents::get_instance()->
                get_system_manager().get_entry_reference(system_uuid);
            return system;
        }
        catch (::agent_framework::exceptions::InvalidUuid) {
            log_error(GET_LOGGER("agent"), "System " << system_uuid <<
                    " not found while updating boot options");
            throw;
        }
    }

    System get_system(const string& system_uuid) {
        try {
            return ComputeComponents::get_instance()->
                get_system_manager().get_entry(system_uuid);
        }
        catch (::agent_framework::exceptions::InvalidUuid) {
            log_error(GET_LOGGER("agent"), "System " << system_uuid <<
                    " not found while updating boot options");
            throw;
        }
    }

    void set_connection_data(ipmitool::ManagementController& mc, const Manager& manager) {
        attribute::ConnectionData connection_data = manager.get_connection_data();
        mc.set_ip(connection_data.get_ip_address());
        mc.set_port(connection_data.get_port());
        mc.set_username(connection_data.get_username());
        mc.set_password(connection_data.get_password());
    }

    void update_boot_options(agent_framework::model::enums::BootOverride& override,
                             agent_framework::model::enums::BootOverrideTarget& target,
                             const Manager& manager) {
        // get connection data from manager
        ipmitool::ManagementController mc{};
        set_connection_data(mc, manager);

        // execute IPMI command
        request::GetSystemBootOptions request{};
        response::GetSystemBootOptions response{};
        try {
            mc.send(request, response);
        }
        catch (const std::exception& e) {
            log_warning(GET_LOGGER("agent"),
                "Unable to send execute GetSystemBootOptions IPMI Command: " << e.what());
            throw;
        }

        // update values
        override = get_boot_override(response.get_boot_override());
        target = get_boot_override_target(response.get_boot_override_target());
    }


    void update_system(const std::string& system_uuid, const Manager& manager) {

        auto system = get_system(system_uuid);

        agent_framework::model::enums::BootOverride boot_override =
            system.get_boot_override();
        agent_framework::model::enums::BootOverrideTarget boot_override_target =
            system.get_boot_override_target();

        auto last_boot_override = boot_override;
        auto last_boot_target = boot_override_target;

        update_boot_options(boot_override, boot_override_target, manager);

        if (last_boot_override != boot_override) {
            log_debug(GET_LOGGER("agent"), "Boot Override for system " <<
                system_uuid << " changed from " << last_boot_override.to_string() <<
                " to " << boot_override.to_string());
        }

        if (last_boot_target != boot_override_target) {
            log_debug(GET_LOGGER("agent"), "Boot Override Target for system " <<
                system_uuid << " changed from " << last_boot_target.to_string() <<
                " to " << boot_override_target.to_string());
        }

        {
            auto system_ref = get_system_reference(system_uuid);
            system_ref->set_boot_override(boot_override);
            system_ref->set_boot_override_target(boot_override_target);
        }
    }

}

void SystemBootOptionsThread::start() {
    if (!m_running) {
        m_running = true;
        m_thread = std::thread(&SystemBootOptionsThread::m_task, this);
    }
}

void SystemBootOptionsThread::stop() {
    if (m_running) {
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
}

void SystemBootOptionsThread::m_task() {

    log_debug(GET_LOGGER("status"), "System boot options thread started.");

    while (m_running) {
        auto manager_uuids = ComputeComponents::get_instance()->
                                 get_module_manager().get_keys();
        for (const auto& manager_uuid : manager_uuids) {

            auto manager = ComputeComponents::get_instance()->
                               get_module_manager().get_entry(manager_uuid);
            // ignore nonpresent managers
            if (!manager.get_presence()) {
                continue;
            }

            auto system_uuids = ComputeComponents::get_instance()->
                                    get_system_manager().get_keys(manager_uuid);

            for (const auto& system_uuid : system_uuids) {
                try {
                    update_system(system_uuid, manager);
                }
                catch (const std::exception) {
                    log_warning(GET_LOGGER("status"), "Unable to get boot options for system: "
                            << system_uuid);
                }
            }
        }
        std::this_thread::sleep_for(m_interval);
    }

    log_debug(GET_LOGGER("status"), "System boot options thread stopped.");
}

SystemBootOptionsThread::~SystemBootOptionsThread() {
    stop();
}
