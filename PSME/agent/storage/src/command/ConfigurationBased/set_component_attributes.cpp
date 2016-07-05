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
 * @section DESCRIPTION
 * */

#include "agent-framework/command/storage/set_component_attributes.hpp"
#include "agent-framework/module-ref/constants/storage.hpp"
#include "agent-framework/module/module_manager.hpp"

#include "iscsi/manager.hpp"
#include "iscsi/response.hpp"

#include <tuple>

using namespace agent_framework::command;
using namespace agent_framework::generic;
using agent_framework::model::literals::IscsiTarget;

/*! SetComponentAttributes implementation */
class SetComponentAttributes : public storage::SetComponentAttributes {
public:
    SetComponentAttributes() {}

    using storage::SetComponentAttributes::execute;

    void execute(const Request& request, Response&) {
        log_debug(GET_LOGGER("storage-agent"), "Trying to execute SetComponentAttributes method.");
        const auto& component = request.get_component();

        auto logical_drive = ModuleManager::find_logical_drive(component).lock();
        if (logical_drive) {
            // @TODO: implement this functionality
            return;
        }

        auto target = ModuleManager::find_target(component).lock();
        if (target) {
            update_target(target, request);
            return;
        }

        THROW(agent_framework::exceptions::InvalidUuid, "storage-agent",
            "Component uuid = '" + component + "' not found.");
    }

    ~SetComponentAttributes();

private:
    std::tuple<OptionalField<std::string>, OptionalField<std::string>> get_command_attributes(const Request& request) {
        OptionalField<std::string> initiator_iqn{};
        OptionalField<std::string> target_iqn{};

        const auto& attributes = request.get_attributes();
        const auto& attributes_names = attributes.get_names();

        for(const auto& attribute_name : attributes_names) {
            const auto& value = attributes.get_value(attribute_name).asString();

            log_debug(GET_LOGGER("agent"), "Attribute name:" << attribute_name);
            log_debug(GET_LOGGER("agent"), "Attribute value:" << value);

            if(IscsiTarget::INITIATOR_IQN == attribute_name) {
                initiator_iqn = value;
            } else if (IscsiTarget::TARGET_IQN == attribute_name) {
                target_iqn = value;
            } else if(IscsiTarget::OEM == attribute_name) {
                log_warning(GET_LOGGER("rpc"), "Unsupported attribute: oem.");
            } else {
                log_error(GET_LOGGER("rpc"), "Unrecognized attribute: "
                          << attribute_name);
            }
        }
        return std::make_tuple(initiator_iqn, target_iqn);
    }

    void update_target(const std::shared_ptr<Target>& target, const Request& request) {
        OptionalField<std::string> initiator_iqn{};
        OptionalField<std::string> target_iqn{};

        // get target attributes from the request
        std::tie(initiator_iqn, target_iqn) = get_command_attributes(request);

        // if the requested is to change the target IQN, an error is
        // thrown, as target name is permanent (see RFC3720)
        if(target_iqn.has_value()) {
            THROW(agent_framework::exceptions::UnsupportedParameter, "storage-agent",
                "RFC3720 does not allow changing iSCSI target IQN.");
        }

        // if inititator IQN was provided, bind it to the target
        if(initiator_iqn.has_value()) {
            agent::storage::iscsi::tgt::Manager manager;
            const auto target_id = target->get_target_id();
            set_initiator_name(manager, target_id, initiator_iqn.value());
            // if  set_initiator_name  didn't throw, update model:
            target->set_initiator_iqn(initiator_iqn);
        }
    }

    void set_initiator_name(agent::storage::iscsi::tgt::Manager& manager,
                                const std::int32_t target_id,
                                const std::string& initiator_name) {
        if (initiator_name.empty()) {
            log_debug(GET_LOGGER("storage-agent"), "No target initiator-name set");
            return;
        }
        agent::storage::iscsi::tgt::Manager::OptionMapper options;
        options.emplace(std::make_pair("initiator-name", initiator_name));
        auto response = manager.bind_target(target_id, options);
        if (!response.is_valid()) {
            agent::storage::iscsi::tgt::Errors::throw_exception(
                    response.get_error(),
                    "Cannot bind initiator-name to target: ");
        }
    }

};

SetComponentAttributes::~SetComponentAttributes() {}

static Command::Register<SetComponentAttributes> g("ConfigurationBased");
