/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * @file lvm_clone_task.cpp
 * @brief LVM clone task implementation
 * */

#include "lvm/lvm_clone_task.hpp"

#include "agent-framework/exceptions/exception.hpp"

#include <fstream>

using namespace agent::storage::lvm;

LvmCloneTask::LvmCloneTask(const model::CreationData& creation_data) :
    m_creation_data{creation_data} { }

void LvmCloneTask::operator()() {
    log_info("lvm", "Clone task started [Src: " << get_source() << ", Dest: " << get_destination() << "]");

    try {
        std::ifstream source(get_source(), std::ios::binary);
        std::ofstream destination(get_destination(), std::ios::binary);
        static constexpr int buffer_size = 1024*1024;
        char buffer[buffer_size];

        do {
            source.read(buffer, buffer_size);
            destination.write(buffer, source.gcount());
        }
        while (!source.eof());

        source.close();
        destination.close();
    }
    catch (const std::exception& error) {
        THROW(agent_framework::exceptions::LvmError, "lvm", std::string{"Could not copy data to clone: "} + error.what());
    }

    log_info("lvm", "Clone task finished.");
}

std::string LvmCloneTask::get_source() const {
    return "/dev/" + m_creation_data.get_volume_group() + "/" + m_creation_data.get_logical_volume();
}

std::string LvmCloneTask::get_destination() const {
    return "/dev/" + m_creation_data.get_volume_group() + "/" + m_creation_data.get_create_name();
}

