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
 * @brief Collection specific data used in commands response
 * */

#pragma once
#include "agent-framework/module/enum/common.hpp"
#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace attribute {

/*! @brief Class representing elements of collections arrays */
class Collection {
public:
    explicit Collection();

    Collection(const std::string& name,
               const enums::CollectionType& type,
               const std::string& slot_mask):
               m_name{name}, m_type{type}, m_slot_mask{slot_mask}
               {}

    Collection(const enums::CollectionName& name,
               const enums::CollectionType& type,
               const std::string& slot_mask):
               m_name{name.to_string()}, m_type{type}, m_slot_mask{slot_mask}
               {}

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
    void set_name(const std::string name) {
        m_name = name;
    }

    /*!
     * @brief Get collections array entry name
     * @return Entry name
     */
    std::string get_name() const {
        return m_name;
    }

    /*!
     * @brief Set collections array entry type
     * @param[in] type Entry type
     */
    void set_type(const enums::CollectionType type) {
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
     * @brief Set collections array entry slot mask
     * @param[in] slot_mask Entry slot mask
     */
    void set_slot_mask(const std::string& slot_mask) {
        m_slot_mask = slot_mask;
    }

    /*!
     * @brief Get collections array entry slot mask
     * @return Entry slot mask
     */
    const std::string& get_slot_mask() const {
        return m_slot_mask;
    }

    /*!
     * @brief Make Json from collections array entry
     * @return Json object
     */
    Json::Value to_json() const;

    /*!
     * @brief make collections array entry from Json
     * @param[in] json the Json input
     * @return new collection entry
     */
    static Collection from_json(const Json::Value& json);

private:

    std::string m_name{};
    enums::CollectionType m_type{enums::CollectionType::None};
    std::string m_slot_mask{};

};

}
}
}
