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
 *
 * @file lvm_clone_task.cpp
 *
 * @brief Lvm clone task implementation
 * */

#include "event/storage_event.hpp"
#include "lvm/lvm_clone_task.hpp"

#include "agent-framework/module/storage_components.hpp"

#include <fstream>



using namespace agent::storage::event;
using namespace agent::storage::lvm;
using namespace agent_framework::model;
using namespace agent_framework::eventing;
using namespace agent_framework::module;

LvmCloneTask::LvmCloneTask(const LvmCreateData& create_data) :
    m_create_data{create_data} { }

void LvmCloneTask::operator()() {
    log_info(GET_LOGGER("lvm"), "Clone start \n"
            << " src: " << get_source() << " \n "
            << " dest: " << get_dest());

    attribute::Status status{enums::State::Enabled, enums::Health::OK};
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
        status = {enums::State::Absent, enums::Health::OK};
    }
    log_info(GET_LOGGER("lvm"), "Clone finished");
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

