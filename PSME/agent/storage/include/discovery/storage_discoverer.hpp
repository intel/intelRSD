/*!
 * @brief Storage discoverer interface.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file storage_discoverer.hpp
 */

#pragma once



#include "agent-framework/module/model/model_storage.hpp"
#include "agent-framework/module/storage_components.hpp"



namespace agent {
namespace storage {
namespace discovery {

class StorageDiscoverer {
public:
    using SPtr = std::shared_ptr<StorageDiscoverer>;


    /*! @brief Default constructor. */
    StorageDiscoverer() = default;


    /*! @brief Destructor. */
    virtual ~StorageDiscoverer();


    /*!
     * @brief Perform discovery of a manager object.
     * @param manager Manager object to be filled with discovered data.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool discover_manager(agent_framework::model::Manager& manager);


    /*!
     * @brief Perform discovery of a storage service object.
     * @param storage_service Storage service object to be filled with discovered data.
     * @return True if discovery was successful, false otherwise.
     * */
    virtual bool discover_storage_service(agent_framework::model::StorageService& storage_service);


    /*!
     * @brief Perform discovery of a chassis object.
     * @param chassis Chassis object to be filled with discovered data.
     * @return True if discovery was successful, false otherwise.
     * */
    virtual bool discover_chassis(agent_framework::model::Chassis& chassis);


    /*!
     * @brief Perform discovery of a system object.
     * @param system System object to be filled with discovered data.
     * @return True if discovery was successful, false otherwise.
     * */
    virtual bool discover_system(agent_framework::model::System& system);


    /*!
     * @brief Perform discovery of ethernet interface objects.
     * @param interfaces Empty vector of ethernet interface objects, filled by the method with discovered data.
     * @param parent_uuid Parent UUID.
     * @return True if discovery was successful, false otherwise.
     * */
    virtual bool discover_ethernet_interfaces(std::vector<agent_framework::model::NetworkInterface>& interfaces,
                                              const Uuid& parent_uuid);


    /*!
     * @brief Perform discovery of drive objects.
     * @param drives Empty vector of drive objects, filled by the method with discovered data.
     * @param parent_uuid Parent UUID.
     * @return True if discovery was successful, false otherwise.
     * */
    virtual bool discover_drives(std::vector<agent_framework::model::Drive>& drives, const Uuid& parent_uuid);


    /*!
     * @brief Perform discovery of Fabric object.
     * @param fabric Fabric object to be filled by the method with discovered data.
     * @return True if discovery was successful, false otherwise.
     * */
    virtual bool discover_fabric(agent_framework::model::Fabric& fabric);


    /*!
     * @brief Perform discovery of zone objects.
     * @param zones Empty vector of zone objects, filled by the method with discovered data.
     * @param parent_uuid Parent UUID.
     * @return True if discovery was successful, false otherwise.
     * */
    virtual bool discover_zones(std::vector<agent_framework::model::Zone>& zones, const Uuid& parent_uuid);


    /*!
     * @brief Perform discovery of endpoint objects.
     * @param endpoints Empty vector of endpoint objects, filled by the method with discovered data.
     * @param parent_uuid Parent UUID.
     * @return True if discovery was successful, false otherwise.
     * */
    virtual bool discover_endpoints(std::vector<agent_framework::model::Endpoint>& endpoints, const Uuid& parent_uuid);


    /*!
     * @brief Perform discovery of storage pool objects.
     *
     * @param storage_pools Empty vector of storage pool objects, filled by the method with discovered data.
     * @param parent_uuid Parent UUID.
     *
     * @return True if discovery was successful, false otherwise.
     * @warning Method needs drives collection discovered in model managers.
     * */
    virtual bool discover_storage_pools(std::vector<agent_framework::model::StoragePool>& storage_pools,
                                        const Uuid& parent_uuid) = 0;

    /*!
     * @brief Perform discovery of storage pool's capacity sources.
     *
     * @param storage_pool Storage pool object to be filled by the method with discovered data.
     *
     * @return True if discovery was successful, false otherwise.
     * @warning Method needs drives collection discovered in model managers.
     */
    virtual bool discover_capacity_sources(agent_framework::model::StoragePool& storage_pool) = 0;

    /*!
     * @brief Perform discovery of volume objects.
     * @param volumes Empty vector of volume objects, filled by the method with discovered data.
     * @param parent_uuid Parent UUID.
     * @return True if discovery was successful, false otherwise.
     * */
    virtual bool discover_volumes(std::vector<agent_framework::model::Volume>& volumes, const Uuid& parent_uuid) = 0;
};

}
}
}
