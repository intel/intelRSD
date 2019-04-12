/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file acl.hpp
 * @brief ACL model interface
 * */
#pragma once



#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/network.hpp"
#include "agent-framework/module/enum/common.hpp"



namespace agent_framework {
namespace model {

/*! ACL */
class Acl : public Resource {
public:
    explicit Acl(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::Switch);


    ~Acl();


    Acl(const Acl&) = default;


    Acl& operator=(const Acl&) = default;


    Acl(Acl&&) = default;


    Acl& operator=(Acl&&) = default;


    /*!
     * @brief construct an object of class ACL from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed ACL object
     * */
    static Acl from_json(const json::Json& json);


    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     * */
    json::Json to_json() const;


    /*!
     * @brief Get collection name
     *
     * @return collection name
     * */
    static enums::CollectionName get_collection_name() {
        return Acl::collection_name;
    }


    /*!
     * @brief Get component name
     *
     * @return component name
     * */
    static enums::Component get_component() {
        return Acl::component;
    }

private:
    static const enums::CollectionName collection_name;
    static const enums::Component component;
};

} // namespace model
} // namespace agent_framework

