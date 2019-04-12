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
 * @brief Collection specific data used in commands response
 * */

#pragma once



#include "agent-framework/module/enum/common.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>



namespace agent_framework {
namespace model {
namespace attribute {

/*! @brief Class representing elements of collections arrays */
class Collection {
public:
    explicit Collection();

    Collection(const std::string& name,
               const enums::CollectionType& type) :
        m_name{name}, m_type{type} {}


    Collection(const enums::CollectionName& name,
               const enums::CollectionType& type) :
        m_name{name.to_string()}, m_type{type} {}


    ~Collection();

    /*! Enable copy */
    Collection(const Collection&) = default;


    Collection& operator=(const Collection&) = default;


    Collection(Collection&&) = default;


    Collection& operator=(Collection&&) = default;


    /*!
     * @brief Set collections array entry name
     * @param[in] name Entry name
     */
    void set_name(const std::string& name) {
        m_name = name;
    }


    /*!
     * @brief Get collections array entry name
     * @return Entry name
     */
    const std::string& get_name() const {
        return m_name;
    }


    /*!
     * @brief Set collections array entry type
     * @param[in] type Entry type
     */
    void set_type(enums::CollectionType type) {
        m_type = type;
    }


    /*!
     * @brief Get collections array entry type
     * @return Entry type
     */
    enums::CollectionType get_type() const {
        return m_type;
    }

    /*!
     * @brief Make Json from collections array entry
     * @return Json object
     */
    json::Json to_json() const;


    /*!
     * @brief make collections array entry from Json
     * @param[in] json the Json input
     * @return new collection entry
     */
    static Collection from_json(const json::Json& json);


private:

    std::string m_name{};
    enums::CollectionType m_type{enums::CollectionType::None};
};

}
}
}