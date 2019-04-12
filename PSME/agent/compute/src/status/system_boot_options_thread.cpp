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
 * @brief System Boot Options Thread
 * @file system_boot_options_thread.cpp
 * */

#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/model/attributes/event_data.hpp"
#include "ipmi/command/generic/get_system_boot_options.hpp"
#include "status/system_boot_options_thread.hpp"
#include "command/set_network_device_function_attributes.hpp"
#include "status/bmc.hpp"



using namespace ipmi;
using namespace ipmi::manager;
using namespace ipmi::command::generic;
using namespace agent::compute::status;
using namespace agent_framework::model;
using namespace agent_framework::eventing;

using agent_framework::module::ComputeComponents;
using agent_framework::module::CommonComponents;
using agent::compute::Bmc;

namespace {

struct BootOptions {
    enums::BootOverride BootOverride{enums::BootOverride::Disabled};
    enums::BootOverrideTarget BootOverrideTarget{enums::BootOverrideTarget::None};
    enums::BootOverrideMode BootMode{enums::BootOverrideMode::Legacy};
};


enums::BootOverrideTarget get_boot_override_target(BootOverrideTarget target) {
    switch (target) {
        case BootOverrideTarget::Hdd:
            return enums::BootOverrideTarget::Hdd;
        case BootOverrideTarget::Pxe:
            return enums::BootOverrideTarget::Pxe;
        case BootOverrideTarget::RemoteDrive:
            return enums::BootOverrideTarget::RemoteDrive;
        case BootOverrideTarget::None:
        case BootOverrideTarget::Other:
        default:
            return enums::BootOverrideTarget::None;
    }
}


enums::BootOverride get_boot_override(BootOverride _override) {
    switch (_override) {
        case BootOverride::Once:
            return enums::BootOverride::Once;
        case BootOverride::Continuous:
            return enums::BootOverride::Continuous;
        case BootOverride::Disabled:
        default:
            return enums::BootOverride::Disabled;
    }
}


enums::BootOverrideMode get_boot_mode(BootMode mode) {
    switch (mode) {
        case BootMode::UEFI:
            return enums::BootOverrideMode::UEFI;
        case BootMode::Legacy:
        default:
            return enums::BootOverrideMode::Legacy;
    }
}


BootOptions read_boot_options(ipmi::IpmiController& ipmi) {
    request::GetSystemBootOptions request{};
    response::GetSystemBootOptions response{};
    ipmi.send(request, response);
    BootOptions bootOptions{};
    bootOptions.BootOverride = get_boot_override(response.get_boot_override());
    bootOptions.BootOverrideTarget = get_boot_override_target(response.get_boot_override_target());
    bootOptions.BootMode = get_boot_mode(response.get_boot_mode());
    return bootOptions;
}


void update_system(BootOptions& boot_options, System& system) {

    auto last_boot_override = system.get_boot_override();
    auto last_boot_target = system.get_boot_override_target();
    auto last_boot_mode = system.get_boot_override_mode();

    bool iscsi_enabled = agent::compute::get_iscsi_enabled(system);

    if (boot_options.BootOverrideTarget == enums::BootOverrideTarget::Hdd && iscsi_enabled) {
        boot_options.BootOverrideTarget = enums::BootOverrideTarget::RemoteDrive;
    }

    if (last_boot_override != boot_options.BootOverride) {
        log_info("agent", "Boot Override for system " <<
                                                      system.get_uuid() << " changed from "
                                                      << last_boot_override <<
                                                      " to " << boot_options.BootOverride.to_string());
    }

    if (last_boot_target != boot_options.BootOverrideTarget) {
        log_info("agent", "Boot Override Target for system " <<
                                                             system.get_uuid() << " changed from "
                                                             << last_boot_target <<
                                                             " to "
                                                             << boot_options.BootOverrideTarget.to_string());
    }

    if (last_boot_mode != boot_options.BootMode) {
        log_info("agent", "Boot Mode for system " <<
                                                  system.get_uuid() << " changed from "
                                                  << last_boot_mode <<
                                                  " to " << boot_options.BootMode.to_string());
    }

    system.set_boot_override(boot_options.BootOverride);
    system.set_boot_override_target(boot_options.BootOverrideTarget);
    system.set_boot_override_mode(boot_options.BootMode);

    // todo: ??
    //notify(b.get_manager_uuid(), system_uuid);
}

}


void agent::compute::status::update_system_boot_options(Bmc& bmc) {
    using agent_framework::module::get_manager;
    log_debug("bmc", bmc.get_id() << " reading system boot options");
    if (Bmc::State::ONLINE == bmc.get_state()) {
        try {
            auto boot_options = read_boot_options(bmc.ipmi());
            const auto system_uuids = get_manager<System>().get_keys(bmc.get_manager_uuid());
            for (const auto& system_uuid: system_uuids) {
                auto system_ref = get_manager<System>().get_entry_reference(system_uuid);
                update_system(boot_options, system_ref.get_raw_ref());
            }
        }
        catch (const ipmi::ResponseError& e) {
            log_warning("bmc", bmc.get_id() << "boot options read failed: " << e.what());
        }
    }
}
