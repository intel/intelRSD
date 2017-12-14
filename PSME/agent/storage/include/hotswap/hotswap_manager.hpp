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
 * @file hotswap_manager.hpp
 *
 * @brief Initial hotswap implementation.
 * */

#include "sysfs/sysfs_api.hpp"

/* forward declaration */
namespace agent_framework {
    namespace model {
        class PhysicalDrive;
        class LogicalDrive;
    }
}

namespace agent {
namespace storage {
namespace hotswap_discovery {

/*!
 * @brief HotSwap manager implementation
 */
class HotswapManager {
public:
    using PhysicalDrive = agent_framework::model::PhysicalDrive;
    using LogicalDrive = agent_framework::model::LogicalDrive;
    using HardDrive = agent::storage::SysfsAPI::HardDrive;

    /*!
     * @brief Update storage physical drive information
     */
    void hotswap_discover_hard_drives();

private:
    void resolve_dependencies(const PhysicalDrive&);
    void check_volume_groups_state(const std::string&);
    void check_logical_volumes_state(const std::string&);
    bool check_physical_volumes_state(const PhysicalDrive&, const std::string&);
    void check_targets_state(const LogicalDrive&, const std::string&);
    bool compare_disks(const HardDrive&, const PhysicalDrive&);
    void add_disk(const HardDrive&);
    void remove_disk(const PhysicalDrive&);
};

}
}
}

