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
 * @file identifier.hpp
 * @brief identifier attribute
 */

#pragma once



#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>



namespace agent_framework {
namespace model {
namespace attribute {

/*! Identifier attribute */
class Identifier {
public:
    /*! Default constructor */
    explicit Identifier() = default;


    /*!
     * @brief Constructor
     * @param[in] durable_name Value of the identifier
     * @param[in] durable_format Format of the identifier
     */
    Identifier(const OptionalField<std::string>& durable_name,
               const OptionalField<enums::IdentifierType>& durable_format);


    /*! Enable copy & assignment */
    Identifier(const Identifier&) = default;


    Identifier(Identifier&&) = default;


    Identifier& operator=(const Identifier&) = default;


    Identifier& operator=(Identifier&&) = default;


    /*! Default destructor */
    virtual ~Identifier();


    /*!
     * @brief Set durable_name
     * @param durable_name durable_name
     */
    void set_durable_name(const OptionalField<std::string>& durable_name) {
        m_durable_name = durable_name;
    }


    /*!
     * @brief Get durable_name
     * @return durable_name
     */
    const OptionalField<std::string>& get_durable_name() const {
        return m_durable_name;
    }


    /*!
     * @brief Set durable name format
     * @param durable_name_format the format of the durable name
     */
    void set_durable_name_format(const OptionalField<enums::IdentifierType>& durable_name_format) {
        m_durable_name_format = durable_name_format;
    }


    /*!
     * @brief Get durable name format
     * @return the format of the durable name
     */
    const OptionalField<enums::IdentifierType>& get_durable_name_format() const {
        return m_durable_name_format;
    }


    /*!
     * @brief Convert Identifier to JSON object
     *
     * @return JSON representation of Identifier object
     */
    json::Json to_json() const;


    /*!
     * @brief construct an object of class Identifier from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed Identifier object
     */
    static Identifier from_json(const json::Json& json);


    /*!
     * @brief Helper method for easy access to NQN identifier.
     *
     * @tparam T Resource type.
     * @param[in] model Model object with identifiers array property.
     *
     * @return NQN of the provided model object.
     * @throw Throws std::logic_error when object has no NQN.
     */
    template<typename T>
    static const std::string& get_nqn(const T& model) {
        return get_identifier_by_type(model, enums::IdentifierType::NQN);
    }


    /*!
     * @brief Helper method for easy access to iQN identifier.
     *
     * @tparam T Resource type.
     * @param[in] model Model object with identifiers array property.
     *
     * @return The iQN of the provided model object.
     * @throw Throws std::logic_error when object has no iQN.
     */
    template<typename T>
    static const std::string& get_iqn(const T& model) {
        return get_identifier_by_type(model, enums::IdentifierType::iQN);
    }


    /*!
     * @brief Helper method for easy access to UUID identifier.
     *
     * @tparam T Resource type.
     * @param[in] model Model object with identifiers array property.
     *
     * @return UUID of the provided model object.
     * @throw Throws std::logic_error when object has no UUID identifier.
     */
    template<typename T>
    static const std::string& get_uuid(const T& model) {
        return get_identifier_by_type(model, enums::IdentifierType::UUID);
    }


protected:

    /*!
     * @brief Helper method for easy access to identifier by type.
     *
     * @tparam T Resource type.
     * @param model Model object with identifiers array property.
     * @param type Identifier type to find in identifiers array.
     *
     * @return Durable name of requested identifier type
     * @throw Throws std::logic_error when object has no identifier.
     */
    template<typename T>
    static const std::string& get_identifier_by_type(const T& model, const enums::IdentifierType& type) {
        for (const auto& identifier : model.get_identifiers()) {
            if (identifier.get_durable_name_format() == type) {
                if (!identifier.get_durable_name().has_value()) {
                    break;
                }
                return identifier.get_durable_name().value();
            }
        }
        throw std::logic_error("Object has no " + std::string(type.to_string()) + " identifier.");
    }


private:
    OptionalField<std::string> m_durable_name{};
    OptionalField<enums::IdentifierType> m_durable_name_format{};
};

}
}
}
