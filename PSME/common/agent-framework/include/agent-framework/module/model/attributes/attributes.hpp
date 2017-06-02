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
 *
 * @file attributes.hpp
 * @brief Attributes class
 * */

#pragma once
#include <json/json.h>
#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! @brief Component attributes */
class Attributes {
private:
    Json::Value m_value{};

public:

    using Members = std::vector<std::string>;
    using Size = Json::ArrayIndex;

    explicit Attributes();
    virtual ~Attributes();

    /*! Enable copy */
    Attributes(const Attributes&) = default;
    Attributes& operator=(const Attributes&) = default;
    Attributes(Attributes&&) = default;
    Attributes& operator=(Attributes&&) = default;

    /*!
     * @brief Set attributes value
     * @param[in] name Attribute name
     * @param[in] value Attributes as JSON object
     * */
    void set_value(const std::string& name, const Json::Value& value) {
        m_value[name] = value;
    }

    /*!
     * @brief Get attributes value
     * @param[in] name Attribute name
     * @return Attributes as JSON object
     * */
    const Json::Value& get_value(const std::string& name) const {
        return m_value[name];
    }

    /*!
     * @brief Get names of attributes
     * @return Collection of names
     * */
    const Members get_names() const {
        return m_value.getMemberNames();
    }

    /*!
     * @brief Get attributes size
     * @return Number of fields in JSON attribute object
     * */
    Size size() const {
        return m_value.size();
    }

    /*!
     * @brief Check if JSON object is empty
     * @return true if object has no fields, false otherwise
     * */
    bool empty() const {
        return m_value.empty();
    }

    /*!
     * @brief transform the object to JSon
     * @return the object serialized to Json::Value
     */
    const Json::Value& to_json() const {
        return m_value;
    }

    /*!
     * @brief construct an object of class Attributes from JSON
     * @param json the Json::Value deserialized to object
     * @return the newly constructed Attributes object
     */
    static Attributes from_json(const Json::Value& json) {
        Attributes attributes{};
        attributes.m_value = json;
        return attributes;
    }
};

}
}
}

