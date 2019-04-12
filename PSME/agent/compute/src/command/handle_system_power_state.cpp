/*!
 * @brief Implementation of PowerState related commands
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
 * @file handle_system_power_state.cpp
 */

#include "command/handle_system_power_state.hpp"

#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/action/task_creator.hpp"

#include "ipmi/manager/ipmitool/management_controller.hpp"
#include "ipmi/command/generic/get_chassis_status.hpp"

#include "generic/assertions.hpp"

using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::module;
using namespace agent_framework::exceptions;
using namespace ipmi::manager::ipmitool;
using namespace ipmi::command::generic;
using namespace ::generic;


namespace {

using PowerState = ipmi::command::generic::request::ChassisControlCommand::PowerState;

System get_system(const Uuid& system_uuid) {
    auto& sm = get_manager<System>();
    return sm.get_entry(system_uuid);
}


System::Reference get_system_reference(const Uuid& system_uuid) {
    auto& sm = get_manager<System>();
    return sm.get_entry_reference(system_uuid);
}


Manager find_parent_manager(const Uuid& system_uuid) {
    auto system = get_system(system_uuid);

    auto& mm = get_manager<Manager>();
    auto managers = mm.get_keys("", [&system](const Manager& manager) {
        return manager.get_uuid() == system.get_parent_uuid();
    });

    if (managers.empty()) {
        THROW(InvalidUuid, "compute-agent",
            "Manger for System: '" + system_uuid + "' not found!");
    }

    return mm.get_entry(managers.front());
}


void set_connection_data(ManagementController& mc, const Uuid& system_uuid) {
    auto manager = find_parent_manager(system_uuid);
    auto connection_data = manager.get_connection_data();

    mc.set_ip(connection_data.get_ip_address());
    mc.set_port(connection_data.get_port());
    mc.set_username(connection_data.get_username());
    mc.set_password(connection_data.get_password());
}


void send_desired_power_state(ManagementController& mc, PowerState power_state) {
    request::ChassisControlCommand request{};
    request.set_power_state(power_state);
    response::ChassisControlCommand response{};
    mc.send(request, response);
}


bool check_current_power_state_is_on(ManagementController& mc) {
    request::GetChassisStatus request{};
    response::GetChassisStatus response{};
    mc.send(request, response);
    return response.is_power_on();
}

}

namespace agent {
namespace compute {

using PowerState = ipmi::command::generic::request::ChassisControlCommand::PowerState;

void handle_system_power_state(PowerState power_state,
                               PowerStateCycle cycle, const std::string& action, const Uuid& system_uuid,
                               std::function<void()> pre_action, std::function<void()> cycle_on_action) {

    static constexpr auto LOCK_CHECKING_PERIOD = std::chrono::milliseconds(100);
    /* Check if command successful only for TRIES * PERIOD */
    static constexpr auto POWER_STATE_CHECKING_PERIOD = std::chrono::milliseconds(1000);
    static constexpr unsigned POWER_STATE_CHECKING_TRIES = 120;

    /* check if system is "locked" by another power_control command.
     * Mutex cannot be used in purpose of locking: whole manager should be
     * locked then, so "busy waiting" pattern must be used.
     */
    while (true) {
        {
            /* critical section on system */
            auto system = get_system_reference(system_uuid);
            if (!system->is_power_state_being_changed()) {
                system->set_power_state_being_changed(true);
                break;
            }
        }
        std::this_thread::sleep_for(LOCK_CHECKING_PERIOD);
    }

    ManagementController mc{};
    set_connection_data(mc, system_uuid);

    std::string message{};
    bool ipmi_fail{false};
    bool is_power_on{};
    bool set_state{true};

    try {
        is_power_on = check_current_power_state_is_on(mc);

        /* check if system is in correct state. The job cannot be done
         * when task is being started: another task might be running and
         * power state might be been changed.
         */
        switch (power_state) {
            case PowerState::ACPI_SOFT_SHUTDOWN:
            case PowerState::POWER_DOWN:
                if (!is_power_on) {
                    set_state = false;
                }
                break;

            case PowerState::POWER_UP:
                if (is_power_on) {
                    set_state = false;
                }
                break;

            case PowerState::POWER_CYCLE:
            case PowerState::HARD_RESET:
                if (!is_power_on) {
                    message = "System is OFF.";
                }
                break;

            /* special case for TPM */
            case PowerState::LAST:
                if (cycle == PowerStateCycle::CYCLE) {
                    if (is_power_on) {
                        power_state = PowerState::POWER_CYCLE;
                    }
                    else {
                        power_state = PowerState::POWER_UP;
                    }
                    break;
                }
                /* continue in DIAGNOSTIC/UNKNOWN */

            case PowerState::DIAGNOSTIC_INTERRUPT:
            case PowerState::UNKNOWN:
            default:
                assert(FAIL("Unreachable code"));
                message = "Not handled";
                set_state = false;
                break;
        }

        if (!set_state) {
            log_info("compute-agent", "State " << action << " [" << static_cast<unsigned>(power_state)
                                   << "] already set for " << mc.get_info());
        }
        else if (message.empty()) {
            pre_action();

            log_info("compute-agent", "Setting state " << action << " [" << static_cast<unsigned>(power_state)
                                   << "] for " << mc.get_info());
            send_desired_power_state(mc, power_state);

            /* check if state is being changed properly */
            unsigned tries;
            for (tries = 0; tries < POWER_STATE_CHECKING_TRIES; tries++) {
                /* wait for potential state change.. */
                std::this_thread::sleep_for(POWER_STATE_CHECKING_PERIOD);

                bool previous_state = is_power_on;
                try {
                    is_power_on = check_current_power_state_is_on(mc);
                }
                catch (const std::runtime_error& e) {
                    log_error("compute-agent", "An error occured while checking current power state: " << e.what());
                    continue;
                }

                /* cycle_on action is done only if got into ON state */
                if (is_power_on && (previous_state != is_power_on)) {
                    /* there's no time checking for this function! */
                    cycle_on_action();
                }

                /* check "stable" target state */
                if (((cycle == PowerStateCycle::ON) && is_power_on) ||
                    ((cycle == PowerStateCycle::OFF) && (!is_power_on))) {
                    log_info("compute-agent", "Target state for " << action << " is set after " << tries
                                                                  <<  " for " << mc.get_info());
                    break;
                }

                /* If power state should cycle, wait for "next" target state.. */
                if ((cycle == PowerStateCycle::CYCLE) && (is_power_on != previous_state)) {
                    if (power_state != PowerState::POWER_CYCLE) {
                        power_state = (is_power_on) ? PowerState::POWER_DOWN : PowerState::POWER_UP;

                        log_info("compute-agent", "Restoring state " << action
                                               << " [" << static_cast<unsigned>(power_state) << "] for " << mc.get_info());
                        send_desired_power_state(mc, power_state);
                    }
                    /* wait for new state */
                    cycle = (is_power_on) ? PowerStateCycle::OFF : PowerStateCycle::ON;
                    log_info("compute-agent", "Wait for system is " << (is_power_on ? "OFF" : "ON") << " for " << mc.get_info());
                }
            }
            if (tries >= POWER_STATE_CHECKING_TRIES) {
                message = "Power state setting timeout";
            }
        }
    }
    catch (ipmi::ResponseError& error) {
        message = std::string(error.what()) + " [CC=" + std::to_string(unsigned(error.get_completion_code())) + "]";
        ipmi_fail = true;
    }
    catch (const std::runtime_error& e) {
        message = std::string(e.what());
        ipmi_fail = true;
    }

    /* done, set state in the resource, regardless it has succeeded! */
    {
        /* critical section on system */
        auto system = get_system_reference(system_uuid);
        system->set_power_state_being_changed(false);
        /* set last read power_state in the model */
        system->set_power_state(is_power_on ? enums::PowerState::On : enums::PowerState::Off);
    }

    if (!message.empty()) {
        if (ipmi_fail) {
            THROW(IpmiError, "compute-agent", "Error sending " + action + " for " + mc.get_info() + ": " + message);
        }
        else {
            THROW(InvalidValue, "compute-agent", "Error sending " + action + " for " + mc.get_info() + ": " + message);
        }
    }
}


Uuid process_system_power_state(const Uuid& system_uuid, enums::ResetType power_state) {
    log_info("compute-agent", "Set Power State Options for System: " << system_uuid);
    log_info("compute-agent", "ResetType: " << power_state);

    action::Task::SubtaskType power_state_task;
    switch (power_state) {
        case enums::ResetType::On:
            log_info("compute-agent", "Sending Power State On.");
            power_state_task = [system_uuid]() {
                handle_system_power_state(PowerState::POWER_UP, PowerStateCycle::ON, "Power On", system_uuid);
            };
            break;

        case enums::ResetType::ForceOff:
            log_info("compute-agent", "Sending Power State Off.");
            power_state_task = [system_uuid]() {
                handle_system_power_state(PowerState::POWER_DOWN, PowerStateCycle::OFF, "Power Off", system_uuid);
            };
            break;

        case enums::ResetType::GracefulRestart:
            log_info("compute-agent", "Sending GracefulRestart.");
            /* Power cycle causes system OFF (where is for 1s), afterfward system is brought ON.
             * Value OFF is not set in the model for that moment either.
             */
            power_state_task = [system_uuid]() {
                handle_system_power_state(PowerState::POWER_CYCLE, PowerStateCycle::CYCLE, "Power Cycle",
                    system_uuid);
            };
            break;

        case enums::ResetType::ForceRestart:
            log_info("compute-agent", "Sending ForceRestart.");
            /* Reset doesn't put system into OFF state at all.. It is always ON!
             * Thus task finishes immediatelly after sending the request (if was in ON state).
             */
            power_state_task = [system_uuid]() {
                handle_system_power_state(PowerState::HARD_RESET, PowerStateCycle::ON, "Reset", system_uuid);
            };
            break;

        case enums::ResetType::GracefulShutdown:
            log_info("compute-agent", "Sending SoftShutdown.");
            power_state_task = [system_uuid]() {
                handle_system_power_state(PowerState::ACPI_SOFT_SHUTDOWN, PowerStateCycle::OFF, "Soft Power Off",
                    system_uuid);
            };
            break;

        case enums::ResetType::ForceOn:
        case enums::ResetType::PushPowerButton:
        case enums::ResetType::Nmi:
            THROW(UnsupportedValue, "compute-agent",
                std::string(power_state.to_string()) + " is not supported.");

        case enums::ResetType::None:
        default:
            assert(FAIL("Unreachable code"));
            return {};
    }

    agent_framework::action::TaskCreator task_creator{};
    task_creator.prepare_task();
    task_creator.add_subtask(power_state_task);

    task_creator.set_promised_response([]() {
        SetComponentAttributes::Response res{};
        return res.to_json();
    });

    task_creator.register_task();
    auto task = task_creator.get_task();
    agent_framework::action::TaskRunner::get_instance().run(task);
    return task_creator.get_task_resource().get_uuid();
}

}
}
