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
 * @file common_components.hpp
 * @brief CommonComponents Interface
 * */

#pragma once



#include "agent-framework/generic/singleton.hpp"

#include "agent-framework/module/managers/generic_manager.hpp"
#include "agent-framework/module/managers/many_to_many_manager.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"

#include "agent-framework/module/model/manager.hpp"
#include "agent-framework/module/model/chassis.hpp"
#include "agent-framework/module/model/drive.hpp"
#include "agent-framework/module/model/system.hpp"
#include "agent-framework/module/model/storage_subsystem.hpp"
#include "agent-framework/module/model/task.hpp"



namespace agent_framework {
namespace module {

/*!
 * Class for managing common components
 * */
class CommonComponents final : public agent_framework::generic::Singleton<CommonComponents> {
public:
    using ModuleManager = GenericManager<model::Manager>;
    using ChassisManager = GenericManager<model::Chassis>;
    using DriveManager = GenericManager<model::Drive>;
    using SystemManager = GenericManager<model::System>;
    using StorageSubsystemManager = GenericManager<model::StorageSubsystem>;
    using TaskManager = GenericManager<model::Task>;
    using StorageDrivesManager = managers::ManyToManyManager;


    virtual ~CommonComponents();


    /*!
     * Get module manager
     *
     * @return Reference to module manager
     * */
    ModuleManager& get_module_manager() {
        return m_module_manager;
    }


    /*!
     * Get chassis manager
     *
     * @return Reference to chassis manager
     * */
    ChassisManager& get_chassis_manager() {
        return m_chassis_manager;
    }


    /*!
     * Get drive manager
     *
     * @return Reference to drive manager
     * */
    DriveManager& get_drive_manager() {
        return m_drive_manager;
    }


    /*!
     * Get task manager
     *
     * @return Reference to task manager
     * */
    TaskManager& get_task_manager() {
        return m_task_manager;
    }


    /*!
     * @brief Get Storage Subsystem manager
     * @return Reference to a storage Subsystem manager instance
     * */
    StorageSubsystemManager& get_storage_subsystem_manager() {
        return m_storage_subsystem_manager;
    }

    /*!
     * @brief Get system manager
     * @return Instance of system manager
     * */
    SystemManager& get_system_manager() {
        return m_system_manager;
    }

    /*!
     * @brief Get manager for StorageSubsystem - Drive mappings
     * @return manager for StorageSubsystem - Drive mappings
     * */
    StorageDrivesManager& get_storage_subsystem_drives_manager() {
        return m_storage_subsystem_drives_manager;
    }

private:
    ModuleManager m_module_manager{};
    ChassisManager m_chassis_manager{};
    DriveManager m_drive_manager{};
    TaskManager m_task_manager{};
    SystemManager m_system_manager{};
    StorageSubsystemManager m_storage_subsystem_manager{};
    StorageDrivesManager m_storage_subsystem_drives_manager{};
};

}
}
