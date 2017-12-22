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
 *
 * @file discovery_manager.hpp
 *
 * @brief Initial discovery implementation.
 * */

#pragma once
#include "agent-framework/discovery/discovery.hpp"
#include "agent-framework/module/model/logical_drive.hpp"

#include <mutex>
#include <condition_variable>

namespace agent {
namespace storage {
namespace discovery {

/*!
 * @brief Implementation of initial discovery
 */
class DiscoveryManager final : public agent_framework::discovery::Discovery {
public:
    using LogicalDrive = agent_framework::model::LogicalDrive;

    /*! @brief Default constructor */
    DiscoveryManager() = default;

    /*!
     * @brief Gather information about storage and its submodules.
     * @param module component uuid.
     */
    void discovery(const std::string& module) override;

    /*! @brief Wait for discovery complete */
    void wait_for_complete();

    /*! @brief Default destructor */
     ~DiscoveryManager();

private:
    void discovery_physical_drives(const std::string& uuid) const;
    void discovery_logical_drives(const std::string& uuid) const;
    void discovery_iscsi_targets(const std::string& uuid) const;

    void init_logical_volume(LogicalDrive& logical_volume) const;
    std::string get_logical_drive_uuid(const std::string& device_path) const;
    std::string get_physical_drive_uuid(const std::string& device_path) const;

    std::mutex m_mutex{};
    std::condition_variable m_cv{};
};

}
}
}

