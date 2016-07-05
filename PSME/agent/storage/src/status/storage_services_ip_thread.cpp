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
 * @section Storage Services Ip Update Thread
*/

#include "status/storage_services_ip_thread.hpp"
#include "agent-framework/module-ref/storage_manager.hpp"
#include "agent-framework/module/module_manager.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "utils/net_utils.hpp"

using namespace agent_framework::generic;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::module::managers;
using namespace agent::storage::status;
using namespace agent::storage::utils;

/*! Default Storage Service Ip update interval */
constexpr const StorageServicesIpUpdateThread::Seconds
StorageServicesIpUpdateThread::DEFAULT_STORAGE_SERVICES_IP_UPDATE_INTERVAL;

namespace {
    const std::string get_interface_name() {
        auto& iscsi_manager = get_manager<IscsiTarget, IscsiTargetManager>();
        auto& iscsi_data = iscsi_manager.get_iscsi_data();
        return iscsi_data.get_interface_name();
    }

    const std::string get_ip_address() {
        const auto& interface_name = get_interface_name();
        log_debug(GET_LOGGER("storage"), "interface_name: " << interface_name);
        const auto& ip_address = NetUtils::get_ip_address(interface_name);
        log_debug(GET_LOGGER("storage"), "ip_address: " << ip_address);
        return ip_address;
    }

    void update_storage_servives_ip() {
        const std::string portal_ip = get_ip_address();
        for (const auto& module : ModuleManager::get_modules()) {
            for(auto& submodule : module->get_submodules()) {
                auto& target_manager = submodule->get_target_manager();
                for(const auto& target : target_manager.get_targets()) {
                    target->set_target_address(portal_ip);
                    log_debug(GET_LOGGER("storage"), "iSCSI Target updated: " << target->get_uuid());
                }
            }
        }
    }
}

void StorageServicesIpUpdateThread::start() {
    if (!m_running) {
        m_running = true;
        m_thread = std::thread(&StorageServicesIpUpdateThread::m_task, this);
    }
}

void StorageServicesIpUpdateThread::stop() {
    if (m_running) {
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
}

void StorageServicesIpUpdateThread::m_task() {
    log_debug(GET_LOGGER("status"), "Storage Services Ip Update thread started.");

    while (m_running) {
        update_storage_servives_ip();
        std::this_thread::sleep_for(m_interval);
    }
    log_debug(GET_LOGGER("status"), "Storage Services Ip Update thread stopped.");
}

StorageServicesIpUpdateThread::~StorageServicesIpUpdateThread() {
    stop();
}
