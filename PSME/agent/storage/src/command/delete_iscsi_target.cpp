/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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


#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/storage_commands.hpp"
#include "agent-framework/eventing/event_data.hpp"

#include "iscsi/manager.hpp"
#include "iscsi/response.hpp"
#include "iscsi/tgt/config/tgt_config.hpp"
#include "iscsi/utils.hpp"
#include "event/storage_event.hpp"

using namespace agent_framework;
using namespace agent_framework::command_ref;
using namespace agent::storage::iscsi::tgt::config;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;
using namespace agent_framework::module;
using namespace agent_framework::module::managers;

using Errors = agent::storage::iscsi::tgt::Errors;

namespace {

    static const constexpr char EMPTY_VALUE[] = "";

    void remove_tgt_config_file(const string& target_iqn) {
        const auto& iscsi_data =
            get_manager<IscsiTarget, IscsiTargetManager>().get_iscsi_data();
        TgtConfig tgtConfig(iscsi_data.get_configuration_path());
        try {
            tgtConfig.remove_target(target_iqn);
        } catch (const std::exception& ex) {
            log_warning(GET_LOGGER("storage-agent"),
                    "Unable to remove TGT target config file: " << ex.what());
        }
    }

    void delete_iscsi_target(const DeleteIscsiTarget::Request& request,
                             DeleteIscsiTarget::Response&) {
        const auto target_uuid = request.get_uuid();
        const auto target = get_manager<IscsiTarget>().get_entry(target_uuid);

        auto prev_chap_username = target.get_chap_username();
        auto prev_mutual_chap_username = target.get_mutual_chap_username();

        const auto target_id = get_manager<IscsiTarget, IscsiTargetManager>().
            get_target_id(target.get_uuid());
        ::agent::storage::iscsi::tgt::Manager manager{};

        // Delete CHAP account if exist
        if (prev_chap_username.has_value() && prev_chap_username.value() != EMPTY_VALUE) {
            try {
                delete_chap_account(manager, prev_chap_username.value());
            } catch (const std::exception &ex) {
                log_warning(GET_LOGGER("storage-agent"),
                            "CHAP account delete operation failed: " << ex.what());
            }
        }

        // Delete Mutual CHAP account if exist
        if (prev_mutual_chap_username.has_value() && prev_mutual_chap_username.value() != EMPTY_VALUE) {
            try {
                delete_chap_account(manager, prev_mutual_chap_username.value());
            } catch (const std::exception &ex) {
                log_warning(GET_LOGGER("storage-agent"),
                            "Mutual CHAP account delete operation failed: " << ex.what());
            }
        }

        auto tgt_response = manager.destroy_target(int(target_id));

        if (!tgt_response.is_valid()) {
            Errors::throw_exception(tgt_response.get_error(),
                                    "Cannot destroy target: ");
        }
        remove_tgt_config_file(target.get_target_iqn());
        get_manager<IscsiTarget, IscsiTargetManager>().
            remove_target_id(target_id);

        log_info(GET_LOGGER("storage-agent"), "iSCSI target removed: " << target_uuid);
        get_manager<IscsiTarget>().remove_entry(target_uuid);
    }

}

REGISTER_COMMAND(DeleteIscsiTarget, ::delete_iscsi_target);
