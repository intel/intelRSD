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
 * @file storage_services.hpp
 *
 *
 * @file storage_services.hpp
 * @brief Declaration of StorageServices class
 * */

#pragma once
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! StorageServices */
class StorageServices : public Resource {
public:
    explicit StorageServices(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::None);
    ~StorageServices();

    StorageServices(const StorageServices&) = default;
    StorageServices& operator=(const StorageServices&) = default;
    StorageServices(StorageServices&&) = default;
    StorageServices& operator=(StorageServices&&) = default;

    /*!
     * @brief construct an object of class StorageServices from JSON
     *
     * @param json the Json::Value deserialized to object
     *
     * @return the newly constructed StorageServices object
     */
    static StorageServices from_json(const Json::Value& json);

    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to Json::Value
     */
    Json::Value to_json() const;

    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return StorageServices::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return StorageServices::component;
    }

private:

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}

