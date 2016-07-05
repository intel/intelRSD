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
 *
 * @file lvm_clone_task.cpp
 *
 * @brief Lvm clone task implementation
 * */

#include "lvm/lvm_clone_task.hpp"
#include "agent-framework/action/task_status_manager.hpp"
#include "logger/logger_factory.hpp"
#include "hotswap/hotswap_event.hpp"
#include "agent-framework/module/module_manager.hpp"

#include <fstream>

using namespace agent::storage::lvm;

LvmCloneTask::LvmCloneTask(const LvmCreateData& create_data) :
m_create_data {
    create_data
}
{
}

void LvmCloneTask::operator()() {
    using namespace agent_framework::action;
    log_info(GET_LOGGER("lvm"), "Clone start \n"
            << " src: " << get_source() << " \n "
            << " dest: " << get_dest());

    bool status{true};
    try {
        std::ifstream source(get_source(), std::ios::binary);
        std::ofstream dest(get_dest(), std::ios::binary);
        // TODO make bufsize configurable, maybe add to REST/GAMI API
        static constexpr int bufsize = 1024*1024;
        char buffer[bufsize];

        do {
            source.read(buffer, bufsize);
            dest.write(buffer, source.gcount());
        } while (!source.eof());

        source.close();
        dest.close();
    } catch (std::exception const& err) {
        log_error(GET_LOGGER("lvm"), "Could not copy data to clone: "
                << err.what());
        status = false;
    }
    log_info(GET_LOGGER("lvm"), "Clone finished");
    TaskStatusManager::get_instance().add_status(m_create_data.get_uuid(),
            status);
    agent::storage::hotswap_event::send_event(m_create_data.get_uuid(),
            ::agent_framework::model::enums::Component::LogicalDrive,
            ::agent_framework::eventing::Notification::Update,
            agent_framework::generic::ModuleManager::get_modules().front()->get_submodules().front()->get_name());
    log_debug(GET_LOGGER("storage-agent"), "Logical volume cloning finished: " << m_create_data.get_uuid());
}

std::string LvmCloneTask::get_source() const {
    return "/dev/" +
    m_create_data.get_volume_group() +
            "/" +
            m_create_data.get_logical_volume();
}

std::string LvmCloneTask::get_dest() const {
    return "/dev/" +
    m_create_data.get_volume_group() +
            "/" +
            m_create_data.get_create_name();
}

