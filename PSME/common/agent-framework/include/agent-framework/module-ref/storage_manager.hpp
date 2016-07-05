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
 * @file storage_manager.hpp
 * @brief Storage manager interface
 * */

#ifndef AGENT_FRAMEWORK_MODULE_STORAGE_MANAGER_HPP
#define AGENT_FRAMEWORK_MODULE_STORAGE_MANAGER_HPP

#include "agent-framework/generic/singleton.hpp"

#include "agent-framework/module-ref/model/storage_controller.hpp"
#include "agent-framework/module-ref/managers/generic_manager.hpp"
#include "agent-framework/module-ref/managers/many_to_many_manager.hpp"
#include "agent-framework/module-ref/managers/utils/manager_utils.hpp"
#include "agent-framework/module-ref/model/manager.hpp"
#include "agent-framework/module-ref/model/storage_services.hpp"
#include "agent-framework/module-ref/model/physical_drive.hpp"
#include "agent-framework/module-ref/model/logical_drive.hpp"
#include "agent-framework/module-ref/model/attributes/iscsi_data.hpp"
#include "agent-framework/module-ref/managers/iscsi_target_manager.hpp"
#include "agent-framework/module-ref/managers/utils/manager_utils.hpp"

/*! Psme namespace */
namespace agent_framework {
namespace module {

/*!
 * @brief Class for managing storage components and subcomponents
 * */
class StorageManager final :
                    public agent_framework::generic::Singleton<StorageManager> {
public:
    using ModuleManager = GenericManager<model::Manager>;
    using StorageServicesManager = GenericManager<model::StorageServices>;
    using PhysicalDriveManager = GenericManager<model::PhysicalDrive>;
    using LogicalDriveManager = GenericManager<model::LogicalDrive>;
    using IscsiTargetManager = managers::IscsiTargetManager;
    using LogicalDriveMembersManager = managers::ManyToManyManager;
    using PhysicalDriveMembersManager = managers::ManyToManyManager;

    /*!
     * @brief Destructor
     * */
    virtual ~StorageManager();

    /*!
     * @brief Get module manager
     * @return Reference to a module manager instance
     * */
    ModuleManager& get_module_manager() {
        return m_module_manager;
    }

    /*!
     * @brief Get storage service manager
     * @return Reference to a storage service manager instance
     * */
    StorageServicesManager& get_storage_services_manager() {
        return m_storage_services_manager;
    }

    /*!
     * @brief Get logical drive manager
     * @return Reference to a logical drive manager instance
     * */
    LogicalDriveManager& get_logical_drive_manager() {
        return m_logical_drive_manager;
    }

    /*!
     * @brief Get physical drive manager
     * @return Reference to a physical drive manager instance
     * */
    PhysicalDriveManager& get_physical_drive_manager() {
        return m_physical_drive_manager;
    }

    /*!
     * @brief Get Iscsi target manager
     * @return Reference to an Iscsi target manager instance
     * */
    IscsiTargetManager& get_iscsi_target_manager() {
        return m_iscsi_target_manager;
    }

    /*!
     * @brief Get logical drive members (children of logical drives) manager
     * @return Reference to the manager
     * */
    LogicalDriveMembersManager& get_logical_drive_members_manager() {
        return m_logical_drive_members_manager;
    }

    /*!
     * @brief Get physical drive members (children of logical drives) manager
     * @return Reference to the manager
     * */
    PhysicalDriveMembersManager& get_physical_drive_members_manager() {
        return m_physical_drive_members_manager;
    }

private:
    ModuleManager m_module_manager{};
    StorageServicesManager m_storage_services_manager{};
    LogicalDriveManager m_logical_drive_manager{};
    PhysicalDriveManager m_physical_drive_manager{};
    IscsiTargetManager m_iscsi_target_manager{};
    LogicalDriveMembersManager m_logical_drive_members_manager{};
    PhysicalDriveMembersManager m_physical_drive_members_manager{};
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_STORAGE_MANAGER_HPP */
