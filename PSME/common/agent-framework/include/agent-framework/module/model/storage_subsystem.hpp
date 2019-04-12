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
 * @file storage_subsystem.hpp
 * @brief Storage subsystem
 * */

#pragma once



#include "agent-framework/module/model/resource.hpp"



namespace agent_framework {
namespace model {

/*! Storage Subsystem */
class StorageSubsystem : public Resource {
public:
    explicit StorageSubsystem(const std::string& parent_uuid = {},
                              enums::Component parent_type = enums::Component::System);


    ~StorageSubsystem();


    StorageSubsystem(const StorageSubsystem&) = default;


    StorageSubsystem& operator=(const StorageSubsystem&) = default;


    StorageSubsystem(StorageSubsystem&&) = default;


    StorageSubsystem& operator=(StorageSubsystem&&) = default;


    /*!
     * @brief construct an object of class StorageSubsystem from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed StorageSubsystem object
     */
    static StorageSubsystem from_json(const json::Json& json);


    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;


    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return StorageSubsystem::collection_name;
    }


    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return StorageSubsystem::component;
    }


private:

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}

