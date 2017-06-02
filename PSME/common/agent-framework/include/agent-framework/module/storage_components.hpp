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
 * @file storage_components.hpp
 * @brief Storage Components interface
 * */

#pragma once
#include "agent-framework/generic/singleton.hpp"

#include "agent-framework/module/managers/generic_manager.hpp"
#include "agent-framework/module/managers/many_to_many_manager.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/module/managers/iscsi_target_manager.hpp"

#include "agent-framework/module/model/model_storage.hpp"

/*! Psme namespace */
namespace agent_framework {
namespace module {

/*!
 * @brief Class for managing storage components and subcomponents
 * */
class StorageComponents final :
                    public agent_framework::generic::Singleton<StorageComponents> {
public:
    using StorageServicesManager = GenericManager<model::StorageServices>;
    using PhysicalDriveManager = GenericManager<model::PhysicalDrive>;
    using LogicalDriveManager = GenericManager<model::LogicalDrive>;
    using IscsiTargetManager = managers::IscsiTargetManager;
    using LogicalDriveMembersManager = managers::ManyToManyManager;
    using PhysicalDriveMembersManager = managers::ManyToManyManager;

    /*!
     * @brief Destructor
     * */
    virtual ~StorageComponents();


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
    StorageServicesManager m_storage_services_manager{};
    LogicalDriveManager m_logical_drive_manager{};
    PhysicalDriveManager m_physical_drive_manager{};
    IscsiTargetManager m_iscsi_target_manager{};
    LogicalDriveMembersManager m_logical_drive_members_manager{};
    PhysicalDriveMembersManager m_physical_drive_members_manager{};
};

}
}
