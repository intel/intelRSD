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
 * @file identifier.hpp
 * @brief identifier attribute
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

/*! Identifier attribute */
class Identifier {
public:
    /*! Default constructor */
    explicit Identifier();


    /*! Enable copy & assignment */
    Identifier(const Identifier&) = default;
    Identifier(Identifier&&) = default;
    Identifier& operator=(const Identifier&) = default;
    Identifier& operator=(Identifier&&) = default;


    /*!
     * @brief Set durable_name
     * @param durable_name durable_name
     */
    void set_durable_name(const std::string& durable_name) {
        m_durable_name = durable_name;
    }


    /*!
     * @brief Get durable_name
     * @return durable_name
     */
    const std::string& get_durable_name() const {
        return m_durable_name;
    }


    /*!
     * @brief Set durable name format
     * @param durable_name_format the format of the durable name
     */
    void set_durable_name_format(const enums::IdentifierType& durable_name_format) {
        m_durable_name_format = durable_name_format;
    }


    /*!
     * @brief Get durable name format
     * @return the format of the durable name
     */
    const enums::IdentifierType& get_durable_name_format() const {
        return m_durable_name_format;
    }


    /*!
     * @brief Convert Identifier to JSON object
     *
     * @return JSON representation of Identifier object
     * */
    Json::Value to_json() const;


    /*!
     * @brief construct an object of class Identifier from JSON
     *
     * @param json the Json::Value deserialized to object
     *
     * @return the newly constructed Identifier object
     */
    static Identifier from_json(const Json::Value& json);


    /*! Default destructor */
    ~Identifier();


private:
    std::string m_durable_name{};
    enums::IdentifierType m_durable_name_format{enums::IdentifierType::UUID};
};

}
}
}
