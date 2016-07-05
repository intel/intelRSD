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
 *
 * @file hotswap_manager.hpp
 *
 * @brief Initial hotswap implementation.
 * */


#include "agent-framework/module/module_manager.hpp"
#include "agent-framework/logger_ext.hpp"
#include "agent-framework/module/hard_drive.hpp"
#include "agent-framework/eventing/event_data.hpp"
#include "sysfs/sysfs_api.hpp"

using agent_framework::generic::Module;
using agent_framework::generic::Submodule;
using agent_framework::generic::ModuleManager;
using HardDriveSharedPtr = agent_framework::generic::HardDrive::HardDriveSharedPtr;
using SysfsAPI = agent::storage::sysfs::SysfsAPI;
using SubmoduleUniquePtr = Submodule::SubmoduleUniquePtr;

namespace agent {
namespace storage {
namespace hotswap_discovery {

/*!
 * @brief Implementation of initial hotswap_discovery.
 */
class HotswapManager {
public:
    void hotswap_discover_hard_drives();
    bool compare_disks(const SysfsAPI::HardDrive&, HardDriveSharedPtr);
    void add_disk(agent_framework::generic::StorageController*, SysfsAPI::HardDrive&, const SubmoduleUniquePtr&);
    void remove_disk(agent_framework::generic::StorageController*, HardDriveSharedPtr&, const SubmoduleUniquePtr&);
    void resolve_dependencies(HardDriveSharedPtr&);
    void fill_disk_parameters(HardDriveSharedPtr&, SysfsAPI::HardDrive&);
    bool check_physical_volumes_state(const SubmoduleUniquePtr&, HardDriveSharedPtr&);
    void check_volume_groups_state(const SubmoduleUniquePtr&);
    void check_logical_volumes_state(const SubmoduleUniquePtr&);
    void check_targets_state(const SubmoduleUniquePtr&, const agent_framework::generic::LogicalDriveSharedPtr&);

};

}
}
}

