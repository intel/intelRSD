/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file wildcard.hpp
 */
#pragma once


#include "json-wrapper/json-wrapper.hpp"
#include "array.hpp"

namespace agent_framework {
namespace model {
namespace attribute {

/*! Wildcard */
class Wildcard {
public:
    using Values = Array<std::string>;


    explicit Wildcard();


    Wildcard(const std::string& name,
             const Values& values) :
        m_name{name}, m_values{values} {}


    ~Wildcard();


    /*! Enable copy */
    Wildcard(const Wildcard&) = default;


    Wildcard& operator=(const Wildcard&) = default;


    Wildcard(Wildcard&&) = default;


    Wildcard& operator=(Wildcard&&) = default;


    /*!
     * @brief Set wildcard name
     * @param name String with Wildcard name
     */
    void set_name(const std::string& name) {
        m_name = name;
    }


    /*!
     * @brief Get wildcard name
     * @return wildcard name
     */
    const std::string& get_name() const {
        return m_name;
    }


    /*!
     * @brief Set wildcard values
     * @param values wildcard values
     */
    void set_values(const Values& values) {
        m_values = values;
    }


    /*!
     * @brief Add value to wildcard
     * @param value wildcard value
     */
    void add_value(const std::string& value) {
        m_values.add_entry(value);
    }


    /*!
     * @brief Get wildcard values
     * @return wildcard values
     */
    const Values& get_values() const {
        return m_values;
    }


    /*!
     * @brief Convert Wildcard to JSON object
     * @return JSON representation of Wildcard object
     */
    json::Json to_json() const;


    /*!
     * @brief construct an object of class Wildcard from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed Wildcard object
     */
    static Wildcard from_json(const json::Json& json);


private:
    std::string m_name{};
    Values m_values{};

};

}
}
}
