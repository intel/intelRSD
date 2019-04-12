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
 * @file storage_service.hpp
 * @brief Declaration of StorageService class
 * */

#pragma once



#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/common.hpp"



namespace agent_framework {
namespace model {

/*! StorageServices */
class StorageService : public Resource {
public:
    explicit StorageService(const std::string& parent_uuid = {},
                            enums::Component parent_type = enums::Component::None);


    ~StorageService();


    StorageService(const StorageService&) = default;


    StorageService& operator=(const StorageService&) = default;


    StorageService(StorageService&&) = default;


    StorageService& operator=(StorageService&&) = default;


    /*!
     * @brief Construct an object of class StorageService from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed StorageService object
     */
    static StorageService from_json(const json::Json& json);


    /*!
     * @brief transform the object to JSON
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;


    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return StorageService::collection_name;
    }


    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return StorageService::component;
    }


private:

    static const enums::CollectionName collection_name;
    static const enums::Component component;
};

}
}

