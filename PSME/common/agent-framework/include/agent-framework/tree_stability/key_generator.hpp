/*!
 * @brief Provides class for one resource tree stabilization based on key generator.
 *
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file key_generator.hpp
 * */

#include "agent-framework/module/model/model_common.hpp"
#include "agent-framework/module/model/model_storage.hpp"
#include "agent-framework/tree_stability/key_value_missing_error.hpp"



namespace agent_framework {
/*!
 * @brief Key generating class for tree stabilization
 */
class KeyGenerator {
public:
    using KeyType = std::string;
    using KeysBaseMap = std::map<agent_framework::model::enums::Component, KeyType>;


    /*!
     * @brief Generate base key for specified Component Type.
     * @param component Type of component.
     * @return Base key.
     * */
    virtual KeyType generate_key_base(const agent_framework::model::enums::Component& component) = 0;


    /*!
     * @brief Generate unique key for Manager.
     * @param[in] manager Manager resource.
     * @return Manager unique key.
     * */
    virtual KeyType generate_key(const agent_framework::model::Manager& manager);


    /*!
     * @brief Generate unique key for Chassis.
     * @param[in] chassis Chassis resource.
     * @return Chassis unique key.
     * */
    virtual KeyType generate_key(const agent_framework::model::Chassis& chassis);


    /*!
      * @brief Generate unique key for Fabric.
      * @param[in] fabric Fabric resource.
      * @return Fabric unique key.
      * */
    virtual KeyType generate_key(const agent_framework::model::Fabric& fabric);


    /*!
     * @brief Generate unique key for StorageService.
     * @param[in] storage_service Storage service resource.
     * @return StorageService unique key.
     * */
    virtual KeyType generate_key(const agent_framework::model::StorageService& storage_service);


    /*!
     * @brief Generate unique key for System.
     * @param[in] system System resource.
     * @return System unique key.
     * */
    virtual KeyType generate_key(const agent_framework::model::System& system);


    /*!
     * @brief Generate unique key for Volume.
     * @param[in] volume Volume resource.
     * @return Volume unique key.
     * */
    virtual KeyType generate_key(const agent_framework::model::Volume& volume);


    /*!
     * @brief Generate unique key for StoragePool.
     * @param[in] pool Storage Pool resource.
     * @return StoragePool unique key.
     * */
    virtual KeyType generate_key(const agent_framework::model::StoragePool& pool);


    /*!
     * @brief Generate unique key for Drive.
     * @param[in] drive Drive resource.
     * @return Drive unique key.
     * */
    virtual KeyType generate_key(const agent_framework::model::Drive& drive);

    /*!
     * @brief Generate unique key for NetworkInterface.
     * @param[in] interface Network interface resource.
     * @return Network interface unique key.
     * */
    virtual KeyType generate_key(const agent_framework::model::NetworkInterface& interface);

    /*!
     * @brief Generate unique key for Endpoint.
     * @param[in] endpoint Endpoint resource.
     * @return Endpoint unique key.
     * */
    virtual KeyType generate_key(const agent_framework::model::Endpoint& endpoint);

};
}