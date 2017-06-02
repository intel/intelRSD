/*!
 * @brief Provides class for stabilizing storage agent resource tree
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
 * @file storage_tree_stabilizer.hpp
 * */

#pragma once



#include "agent-framework/tree_stability/tree_stabilizer.hpp"

#include <string>



namespace agent {
namespace storage {

/*!
 * @brief Tree stability managing class for storage agent.
 * */
class StorageTreeStabilizer : public agent_framework::TreeStabilizer {
public:
    /*! Destructor */
    virtual ~StorageTreeStabilizer();


    /*!
     * Stabilizes storage agent resource tree starting from the module.
     *
     * @param[in] module_uuid Temporary UUID of module to stabilize
     *
     * @return Persistent module UUID
     * */
    virtual const std::string stabilize(const std::string& module_uuid) override;


    /*!
     * Stabilize storage service and all resources below.
     *
     * @param[in] service_uuid UUID of service to stabilize
     *
     * @return Service persistent UUID
     * */
    const std::string stabilize_storage_service(const std::string& service_uuid);


    /*!
     * Stabilize physical drive and all resources below.
     *
     * @param[in] drive_uuid UUID of physical drive to stabilize
     *
     * @return Physical drive persistent UUID
     * */
    const std::string stabilize_physical_drive(const std::string& drive_uuid);


    /*!
     * Stabilize logical drive and all resources below.
     *
     * @param[in] drive_uuid UUID of service to stabilize
     *
     * @return Logical drive persistent UUID
     * */
    const std::string stabilize_logical_drive(const std::string& drive_uuid);


    /*!
     * Stabilize iSCSI target and all resources below.
     *
     * @param[in] target_uuid UUID of service to stabilize
     *
     * @return iSCSI target persistent UUID
     * */
    const std::string stabilize_target(const std::string& target_uuid);

};

}
}
