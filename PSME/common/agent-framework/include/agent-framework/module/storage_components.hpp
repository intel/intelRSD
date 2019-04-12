/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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

#include "agent-framework/module/model/model_storage.hpp"

/*! Psme namespace */
namespace agent_framework {
namespace module {

/*!
 * @brief Class for managing storage components and subcomponents
 * */
class StorageComponents final : public agent_framework::generic::Singleton<StorageComponents> {
public:
    using StoragePoolManager = GenericManager<model::StoragePool>;
    using VolumeManager = GenericManager<model::Volume>;
    using StorageServiceManager = GenericManager<model::StorageService>;
    using SystemStorageServicesManager = managers::ManyToManyManager;


    /*!
     * @brief Destructor
     * */
    virtual ~StorageComponents();


    /*!
     * @brief Get storage service manager
     * @return Reference to a storage service manager instance
     * */
    StorageServiceManager& get_storage_service_manager() {
        return m_storage_service_manager;
    }


    /*!
     * @brief Get storage pool manager
     * @return Reference to a storage pool manager instance
     * */
    StoragePoolManager& get_storage_pool_manager() {
        return m_storage_pool_manager;
    }


    /*!
     * @brief Get volume manager
     * @return Reference to a volume manager instance
     * */
    VolumeManager& get_volume_manager() {
        return m_volume_manager;
    }


    /*!
     * @brief Get ManyToMany manager of System and StorageServices
     * @return Reference to System-StorageServices manager instance
     * */
    SystemStorageServicesManager& get_system_storage_services_manager() {
        return m_system_storage_services_manager;
    }


private:
    StorageServiceManager m_storage_service_manager{};
    StoragePoolManager m_storage_pool_manager{};
    VolumeManager m_volume_manager{};
    SystemStorageServicesManager m_system_storage_services_manager{};
};

}
}
