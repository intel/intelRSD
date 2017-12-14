/*!
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
 * @file ip_manager.cpp
 *
 * @brief Implementation of Ip manager
 * */

#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "ip/ip_manager.hpp"
#include "event/storage_event.hpp"
#include "iscsi/manager.hpp"
#include "iscsi/response.hpp"
#include "iscsi/target_parser.hpp"
#include "lvm/lvm_api.hpp"
#include "lvm/lvm_attribute.hpp"
#include "tree_stability/storage_tree_stabilizer.hpp"


using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::module::managers;
using namespace agent::storage::ip_discovery;
using namespace agent::storage::event;


void IpManager::update_portal_ip(const std::string& portal_ip) {
    auto& target_manager = get_manager<IscsiTarget, IscsiTargetManager>();
    auto iscsi_data = target_manager.get_iscsi_data();
    iscsi_data.set_portal_ip(portal_ip);
    target_manager.set_iscsi_data(iscsi_data);
    log_debug(GET_LOGGER("storage-agent"), "Portal IP was changed to: " << portal_ip);

    auto& module = CommonComponents().get_instance()->get_module_manager();
    for (const auto& manager_uuid : module.get_keys()) {
        for (const auto& storage_uuid :
            get_manager<StorageService>().get_keys(manager_uuid)) {
            for (const auto& uuid : get_manager<IscsiTarget>().get_keys()) {
                auto target = get_manager<IscsiTarget>().get_entry_reference(uuid);
                log_debug(GET_LOGGER("storage-agent"), "Updated iSCSI Target Portal IP: "
                    << target->get_target_iqn());
                target->set_target_address(portal_ip);
                /* send event about iSCSI target change */
                send_event(target->get_uuid(), Component::IscsiTarget,
                           agent_framework::eventing::Notification::Update, storage_uuid);
            }
        }
    }
}

