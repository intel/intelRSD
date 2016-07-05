/*!
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
 * @brief Set Component Attributes JSONRPC command implementation.
 * */

#include "agent-framework/command/compute/set_component_attributes.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"
#include "ipmi/manager/ipmitool/management_controller.hpp"
#include "ipmi/command/generic/chassis_control_command.hpp"
#include "ipmi/command/generic/force_pxe_boot_once.hpp"
#include "ipmi/command/generic/force_pxe_boot_continuous.hpp"
#include "ipmi/command/generic/force_hdd_boot_once.hpp"
#include "ipmi/command/generic/force_hdd_boot_continuous.hpp"
#include "ipmi/command/generic/force_boot_disabled.hpp"

using namespace agent_framework::command;
using namespace agent_framework::model;
using namespace ipmi::manager::ipmitool;
using namespace ipmi::command::generic;

namespace agent {
/*! SetComponentAttributes implementation */
class SetComponentAttributes : public compute::SetComponentAttributes {
public:
    SetComponentAttributes() {}

    using ComputeComponents = agent_framework::module::ComputeManager;
    using compute::SetComponentAttributes::execute;

    void execute(const Request& request, Response&) {
        log_debug(GET_LOGGER("agent"), "Executing SetComponentAttributes.");

        const auto& system_uuid =  request.get_component();

        System system = get_system(system_uuid);

        // get current values
        enums::BootOverride prev_boot_override = system.get_boot_override();
        enums::BootOverrideTarget prev_boot_target = system.get_boot_override_target();

        // set defaults
        m_boot_override = prev_boot_override;
        m_boot_target = prev_boot_target;
        m_power_state = enums::ResetType::None;

        // read parameters from command
        get_command_attributes(request);

        // if one is disabled/none, set the second one
        if (enums::BootOverride::Disabled == m_boot_override ||
            enums::BootOverrideTarget::None == m_boot_target) {
            m_boot_target = enums::BootOverrideTarget::None;
            m_boot_override = enums::BootOverride::Disabled;
        }

        // if any of the boot options has changed, update boot options
        if (prev_boot_override != m_boot_override ||
            prev_boot_target != m_boot_target) {
            set_boot_options(system_uuid, m_boot_override, m_boot_target);
        }

        // if reset was sent, update power status
        if (enums::ResetType::None != m_power_state) {
            set_power_state(system_uuid, m_power_state);
        }
    }
    ~SetComponentAttributes();

private:
    using SystemReference = agent_framework::generic::ObjReference
                            <System, std::recursive_mutex>;

    enums::BootOverride m_boot_override = enums::BootOverride::Once;
    enums::BootOverrideTarget m_boot_target = enums::BootOverrideTarget::None;
    enums::ResetType m_power_state = enums::ResetType::None;

    void get_command_attributes(const Request& request) {
        const auto& attributes = request.get_attributes();
        const auto& attributes_names = attributes.get_names();

        for (const auto& attribute_name : attributes_names) {
            const auto& value = attributes.get_value(attribute_name).asString();

            log_debug(GET_LOGGER("agent"), "Attribute name:" << attribute_name);
            log_debug(GET_LOGGER("agent"), "Attribute value:" << value);

            if (literals::System::BOOT_OVERRIDE == attribute_name) {
                m_boot_override = enums::BootOverride::from_string(value);
            }
            else if (literals::System::BOOT_OVERRIDE_TARGET == attribute_name) {
                m_boot_target = enums::BootOverrideTarget::from_string(value);
            }
            else if (literals::System::POWER_STATE == attribute_name) {
                m_power_state = enums::ResetType::from_string(value);
            }
            else if (literals::System::OEM == attribute_name) {
                log_warning(GET_LOGGER("rpc"), "Unsupported attribute: oem.");
            }
            else {
                log_error(GET_LOGGER("rpc"), "Unrecognized attribute: "
                          << attribute_name);
            }
        }
    }

    void set_boot_options(const string& system_uuid,
                          enums::BootOverride boot_override,
                          enums::BootOverrideTarget boot_target) {
        log_debug(GET_LOGGER("agent"), "Set Boot Options for System: "
                  << system_uuid);
        log_debug(GET_LOGGER("agent"), "BootOverride: " << boot_override);
        log_debug(GET_LOGGER("agent"), "BootOverrideTarget: " << boot_target);

        ManagementController mc{};
        set_connection_data(mc, system_uuid);


        if (enums::BootOverrideTarget::Pxe == boot_target) {
            if (enums::BootOverride::Once == boot_override) {
                log_debug(GET_LOGGER("agent"), "Sending IPMI Force Pxe Once Boot.");
                send_force_pxe_boot_once(mc);
            }
            else if (enums::BootOverride::Continuous == boot_override) {
                log_debug(GET_LOGGER("agent"), "Sending IPMI Force Pxe Continous Boot.");
                send_force_pxe_boot_continous(mc);
            }
        }
        else if (enums::BootOverrideTarget::Hdd == boot_target) {
            if (enums::BootOverride::Once == boot_override) {
                log_debug(GET_LOGGER("agent"), "Sending IPMI Force Hdd Once Boot.");
                send_force_hdd_boot_once(mc);
            }
            else if (enums::BootOverride::Continuous == boot_override) {
                log_debug(GET_LOGGER("agent"), "Sending IPMI Force Hdd Continous Boot.");
                send_force_hdd_boot_continous(mc);
            }
        }
        else if (enums::BootOverrideTarget::None == boot_target) {
            log_debug(GET_LOGGER("agent"), "Sending IPMI Boot None.");
            // TODO: implement setting None for boot options via IPMI.
        }
        else {
            log_error(GET_LOGGER("agent"), "Not supported option: "
                                           << boot_target.to_string());
        }

        auto system = get_system_reference(system_uuid);
        system->set_boot_override(boot_override);
        system->set_boot_override_target(boot_target);
    }

    void set_connection_data(ManagementController& mc,
                             const string& system_uuid) {
        auto manager = find_parent_manager(system_uuid);

        auto connection_data = manager.get_connection_data();

        mc.set_ip(connection_data.get_ip_address());
        mc.set_port(connection_data.get_port());
        mc.set_username(connection_data.get_username());
        mc.set_password(connection_data.get_password());
    }

     Manager find_parent_manager(const string& system_uuid) {
        auto& sm = ComputeComponents::get_instance()->get_system_manager();
        auto system = sm.get_entry(system_uuid);

        auto& mm = ComputeComponents::get_instance()->get_module_manager();
        auto managers = mm.get_keys("", [&system](const Manager& manager)
                                        {
                                            return manager.get_uuid()
                                                   ==
                                                   system.get_parent_uuid();
                                        });

        if (managers.empty()) {
            log_error(GET_LOGGER("agent"), "Manger for System: " << system_uuid
                      << " not found!");
            throw runtime_error("System without manager!");
        }

        return mm.get_entry(managers.front());
    }

    void send_force_pxe_boot_once(ManagementController& mc) {
        request::ForcePxeBootOnce request{};
        response::ForcePxeBootOnce response{};

        mc.send(request, response);

        if (response.get_completion_code()) {
            log_error(GET_LOGGER("agent"), "Error Sending Force Pxe Once Boot CC:"
                      << to_string(unsigned(response.get_completion_code())));
        }
    }

    void send_force_pxe_boot_continous(ManagementController& mc) {
        request::ForcePxeBootContinuous request{};
        response::ForcePxeBootContinuous response{};

        mc.send(request, response);

        if (response.get_completion_code()) {
            log_error(GET_LOGGER("agent"), "Error Sending Force Pxe Continous Boot CC:"
                      << to_string(unsigned(response.get_completion_code())));
        }
    }

    void send_force_hdd_boot_once(ManagementController& mc) {
        request::ForceHddBootOnce request{};
        response::ForceHddBootOnce response{};

        mc.send(request, response);

        if (response.get_completion_code()) {
            log_error(GET_LOGGER("agent"), "Error Sending Force Hdd Once Boot CC:"
                      << to_string(unsigned(response.get_completion_code())));
        }
    }

    void send_force_hdd_boot_continous(ManagementController& mc) {
        request::ForceHddBootContinuous request{};
        response::ForceHddBootContinuous response{};

        mc.send(request, response);

        if (response.get_completion_code()) {
            log_error(GET_LOGGER("agent"), "Error Sending Force Hdd Continous Boot CC:"
                      << to_string(unsigned(response.get_completion_code())));
        }
    }

    void send_force_boot_disabled(ManagementController& mc) {
        request::ForceBootDisabled request{};
        response::ForceBootDisabled response{};

        mc.send(request, response);

        if (response.get_completion_code()) {
            log_error(GET_LOGGER("agent"), "Error Sending Force Boot Disabled CC:"
                      << to_string(unsigned(response.get_completion_code())));
        }
    }

    SystemReference get_system_reference(const string& system_uuid) {
        auto& sm = ComputeComponents::get_instance()->get_system_manager();
        return sm.get_entry_reference(system_uuid);
    }

    void set_power_state(const string& system_uuid,
                         enums::ResetType power_state) {
        auto system = get_system(system_uuid);

        ManagementController mc{};
        set_connection_data(mc, system_uuid);

        switch (power_state) {
            case enums::ResetType::On:
                log_debug(GET_LOGGER("agent"), "Sending Power State On.");
	            send_power_on(mc);
                system.set_power_state(enums::PowerState::On);
            break;
            case enums::ResetType::ForceOff:
                log_debug(GET_LOGGER("agent"), "Sending Power State Off.");
                send_power_off(mc);
                system.set_power_state(enums::PowerState::Off);
            break;

            case enums::ResetType::GracefulRestart:
                log_debug(GET_LOGGER("agent"), "Sending GracefulRestart.");
                send_reset(mc);
            break;
            case enums::ResetType::ForceRestart:
                log_debug(GET_LOGGER("agent"), "Sending ForceRestart.");
                send_power_cycle(mc);
            break;
            case enums::ResetType::Nmi:
                log_debug(GET_LOGGER("agent"), "Nmi not supported.");
            break;
            case enums::ResetType::ForceOn:
                log_debug(GET_LOGGER("agent"), "ForceOn not supported.");
            break;
            case enums::ResetType::PushPowerButton:
                log_debug(GET_LOGGER("agent"), "PushPowerButton not supported.");
            break;
            case enums::ResetType::GracefulShutdown:
                log_debug(GET_LOGGER("agent"), "Sending SoftShutdown.");
                send_soft_power_off(mc);
                system.set_power_state(enums::PowerState::Off);
            break;
            case enums::ResetType::None:
                log_debug(GET_LOGGER("agent"), "Sending Power State None.");
            break;
            default: // do nothing;
                log_error(GET_LOGGER("agent"), "Unknow Reset Type.");
            break;
        }
    }


    System get_system(const string& system_uuid) {
        auto& sm = ComputeComponents::get_instance()->get_system_manager();
        return sm.get_entry(system_uuid);
    }

    void send_power_on(ManagementController& mc) {
        request::ChassisControlCommand request{};
        response::ChassisControlCommand response{};

        request.set_power_state(request::ChassisControlCommand::
                                PowerState::POWER_UP);

        mc.send(request, response);

        if (response.get_completion_code()) {
            log_error(GET_LOGGER("agent"), "Error Sending Power On CC:"
                      << to_string(unsigned(response.get_completion_code())));
        }
    }

    void send_power_off(ManagementController& mc) {
        request::ChassisControlCommand request{};
        response::ChassisControlCommand response{};

        request.set_power_state(request::ChassisControlCommand::
                                PowerState::POWER_DOWN);

        mc.send(request, response);

        if (response.get_completion_code()) {
            log_error(GET_LOGGER("agent"), "Error Sending Power Off CC:"
                      << to_string(unsigned(response.get_completion_code())));
        }
    }

    void send_soft_power_off(ManagementController& mc) {
        request::ChassisControlCommand request{};
        response::ChassisControlCommand response{};

        request.set_power_state(request::ChassisControlCommand::
                                PowerState::ACPI_SOFT_SHUTDOWN);

        mc.send(request, response);

        if (response.get_completion_code()) {
            log_error(GET_LOGGER("agent"), "Error Sending Soft Power Off CC:"
                      << to_string(unsigned(response.get_completion_code())));
        }
    }

    void send_power_cycle(ManagementController& mc) {
        request::ChassisControlCommand request{};
        response::ChassisControlCommand response{};

        request.set_power_state(request::ChassisControlCommand::
                                PowerState::POWER_CYCLE);

        mc.send(request, response);

        if (response.get_completion_code()) {
            log_error(GET_LOGGER("agent"), "Error Sending Power Cycle CC:"
                      << to_string(unsigned(response.get_completion_code())));
        }
    }

    void send_reset(ManagementController& mc) {
        request::ChassisControlCommand request{};
        response::ChassisControlCommand response{};

        request.set_power_state(request::ChassisControlCommand::
                                PowerState::HARD_RESET);

        mc.send(request, response);

        if (response.get_completion_code()) {
            log_error(GET_LOGGER("agent"), "Error Sending Power Cycle CC:"
                      << to_string(unsigned(response.get_completion_code())));
        }
    }

};

SetComponentAttributes::~SetComponentAttributes() {}
}
static Command::Register<agent::SetComponentAttributes> g("sdv");
