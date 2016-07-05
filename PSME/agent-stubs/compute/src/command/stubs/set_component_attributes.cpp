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
 * @file command/stubs/set_component_attributes.cpp
 * @brief Implementation of SetComponentAttributes command
 * */

#include "agent-framework/command/compute/set_component_attributes.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"
#include "agent-framework/module-ref/enum/common.hpp"
#include "agent-framework/module-ref/enum/compute.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/logger_ext.hpp"

using namespace agent_framework::command;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent_framework::exceptions;

/*! Compute stub SetComponentAttributes implementation */
class SetComponentAttributes : public compute::SetComponentAttributes {
public:
    SetComponentAttributes() {}

    using compute::SetComponentAttributes::execute;
    using ComputeComponents = agent_framework::module::ComputeManager;

    void execute(const Request& request, Response&) {

        const auto& uuid = request.get_component();
        const auto& attributes = request.get_attributes();
        std::string message{};

        if(ComputeComponents::get_instance()->
            get_system_manager().entry_exists(uuid)) {
            message = "For system with uuid: " + uuid + " ";
            process_system(uuid, attributes, message);
        }
        else if(ComputeComponents::get_instance()->
            get_network_interface_manager().entry_exists(uuid)) {
            message = "For network interface with uuid: " + uuid + " ";
            process_interface(uuid, attributes, message);
        }
        else if(ComputeComponents::get_instance()->
            get_port_vlan_manager().entry_exists(uuid)) {
            message = "For port vlan with uuid: " + uuid + " ";
            process_port_vlan(uuid, attributes, message);
        }
        else if(ComputeComponents::get_instance()->
                get_module_manager().entry_exists(uuid)
            || ComputeComponents::get_instance()->
                get_processor_manager().entry_exists(uuid)
            || ComputeComponents::get_instance()->
                get_dimm_manager().entry_exists(uuid)
            || ComputeComponents::get_instance()->
                get_drive_manager().entry_exists(uuid)
            || ComputeComponents::get_instance()->
                get_storage_controller_manager().entry_exists(uuid)) {

            THROW(InvalidParameters, "network-agent", "setComponentAttributes not available for this component.");
        }
        else {
            THROW(InvalidUuid, "network-agent", "No component with uuid: " + uuid);
        }

        log_info(GET_LOGGER("network-agent"), message);
        log_info(GET_LOGGER("network-agent"),
            "setComponentAttributes finished succesfully.");
    }

    ~SetComponentAttributes();

private:
    /* message_short is being appended to message only if the attribute value
    is printable */
    void process_system(const std::string& uuid, const Attributes& attributes,
        std::string& message) {
        auto& attribute_names = attributes.get_names();
        auto system = ComputeComponents::get_instance()->
                get_system_manager().get_entry_reference(uuid);

        for(const auto& name : attribute_names) {
            const auto& value = attributes.get_value(name);
            std::string message_short;
            message_short = "Changed " + name + " attribute value to: ";
            try {
                if(literals::System::BOOT_OVERRIDE == name) {
                    system->set_boot_override(enums::BootOverride::
                        from_string(value.asString()));
                    message_short += value.asString();
                }
                else if(literals::System::BOOT_OVERRIDE_TARGET == name) {
                    system->set_boot_override_target(
                        enums::BootOverrideTarget::
                        from_string(value.asString()));
                    message_short += value.asString();
                }
                else if(literals::System::POWER_STATE == name) {
                    const auto& reset_type =
                            enums::ResetType::from_string(value.asString());
                    switch (reset_type) {
                        case enums::ResetType::None:
                        default:
                            break;
                        case enums::ResetType::On:
                        case enums::ResetType::GracefulRestart:
                        case enums::ResetType::ForceRestart:
                        case enums::ResetType::PushPowerButton:
                        case enums::ResetType::ForceOn:
                            system->set_power_state(enums::PowerState::On);
                            break;
                        case enums::ResetType::ForceOff:
                        case enums::ResetType::GracefulShutdown:
                        case enums::ResetType::Nmi:
                            system->set_power_state(enums::PowerState::Off);
                            break;
                    }
                    message_short += value.asString();
                }
                else if(literals::System::OEM == name) {
                    system->set_oem(Oem::from_json(value));
                    message += "Changed " + name + " attribute\n";
                    continue;
                }
                else {
                    message += "Unknown or unsettable attribute name: " + name + "\n";
                    continue;
                }
                message += message_short + "\n";
            }
            catch(::agent_framework::exceptions::InvalidParameters e) {
                THROW(InvalidParameters, "network-agent", "Did not change  "
                    + name + " attribute to: " + value.asString() + " - invalid value.");
            }
        }
    }

    void process_interface(const std::string& uuid,
        const Attributes& attributes, std::string& message) {
        auto& attribute_names = attributes.get_names();
        auto iface = ComputeComponents::get_instance()->
                get_network_interface_manager().get_entry_reference(uuid);

        for(const auto& name : attribute_names) {
            const auto& value = attributes.get_value(name);
            std::string message_short;
            message_short = "Changed " + name + " attribute value to: ";
            if(literals::NetworkInterface::FRAME_SIZE == name) {
                iface->set_frame_size(value.asUInt());
                message_short += std::to_string(value.asUInt());
            }
            if(literals::NetworkInterface::SPEED == name) {
                iface->set_speed_mbps(value.asUInt());
                message_short += std::to_string(value.asUInt());
            }
            if(literals::NetworkInterface::AUTO_SENSE == name) {
                iface->set_autosense(value.asUInt());
                message_short += std::to_string(value.asUInt());
            }
            else if(literals::NetworkInterface::OEM == name) {
                iface->set_oem(Oem::from_json(value));
                message += "Changed " + name + " attribute\n";
                continue;
            }
            else {
                message += "Unknown or unsettable attribute name: " + name + "\n";
                continue;
            }
            message += message_short + "\n";
        }
    }

    void process_port_vlan(const std::string& uuid,
        const Attributes& attributes, std::string& message) {
        auto& attribute_names = attributes.get_names();
        auto portvlan = ComputeComponents::get_instance()->
                get_port_vlan_manager().get_entry_reference(uuid);

        for(const auto& name : attribute_names) {
            const auto& value = attributes.get_value(name);
            std::string message_short;
            message_short = "Changed " + name + " attribute value to: ";
            if(literals::PortVlan::VLAN_NAME == name) {
                portvlan->set_vlan_name(value.asString());
                message_short += std::to_string(value.asUInt());
            }
            if(literals::PortVlan::VLAN_ENABLE == name) {
                portvlan->set_vlan_enable(value.asBool());
                message_short += std::to_string(value.asUInt());
            }
            else if(literals::PortVlan::OEM == name) {
                portvlan->set_oem(Oem::from_json(value));
                message += "Changed " + name + " attribute\n";
                continue;
            }
            else {
                message += "Unknown or unsettable attribute name: " + name + "\n";
                continue;
            }
            message += message_short + "\n";
        }
    }
};

SetComponentAttributes::~SetComponentAttributes() {}

static Command::Register<SetComponentAttributes> g("Stubs");
