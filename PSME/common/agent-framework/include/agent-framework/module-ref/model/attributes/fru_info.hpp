/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file fru_info.hpp
 * @brief FRU info
 * */
#ifndef AGENT_FRAMEWORK_MODULE_MODEL_ATTRIBUTES_FRU_INFO_HPP
#define AGENT_FRAMEWORK_MODULE_MODEL_ATTRIBUTES_FRU_INFO_HPP

#include <string>
#include <json/json.h>
#include "agent-framework/module-ref/utils/utils.hpp"

namespace agent_framework {
namespace model {
namespace attribute {

/*! FRU Info */
class FruInfo {
public:


    explicit FruInfo();

    FruInfo(const std::string& serial_number,
            const std::string& manufacturer,
            const std::string& model_number,
            const std::string& part_number):
            m_serial_number{serial_number},
            m_manufacturer{manufacturer},
            m_model_number{model_number},
            m_part_number{part_number}
            {}

    ~FruInfo();

    /*! Enable copy */
    FruInfo(const FruInfo&) = default;
    FruInfo& operator=(const FruInfo&) = default;
    FruInfo(FruInfo&&) = default;
    FruInfo& operator=(FruInfo&&) = default;


    /*!
     * Sets serial number.
     *
     * @param[in] serial_number serial number.
     * */
    void set_serial_number(const OptionalField<std::string>& serial_number) {
        m_serial_number = serial_number;
    }

    /*!
     * Gets serial number.
     * */
    const OptionalField<std::string>& get_serial_number() const {
        return m_serial_number;
    }

    /*!
     * Sets manufacturer name
     *
     * @param[in] manufacturer Manufacturer name
     * */
    void set_manufacturer(const OptionalField<std::string>& manufacturer) {
        m_manufacturer = manufacturer;
    }

    /*!
     * Sets manufacturer name
     * */
    const OptionalField<std::string>& get_manufacturer() const {
        return m_manufacturer;
    }

    /*!
     * Sets model number
     *
     * @param[in] model_number model number
     * */
    void set_model_number(const OptionalField<std::string>& model_number) {
        m_model_number = model_number;
    }

    /*!
     * Gets model number
     * */
    const OptionalField<std::string>& get_model_number() const {
        return m_model_number;
    }

    /*!
     * Sets part number
     *
     * @param[in] part_number part number
     * */
    void set_part_number(const OptionalField<std::string>& part_number) {
        m_part_number = part_number;
    }

    /*!
     * Gets part number
     * */
    const OptionalField<std::string>& get_part_number() const {
        return m_part_number;
    }

    /*!
     * Converts this to json representation.
     *
     * //return json representation of this class.
     * */
    Json::Value to_json() const;

    /*!
     * @brief construct an object of class FruInfo from JSON
     *
     * @param json the Json::Value deserialized to object
     *
     * @return the newly constructed FruInfo object
     */
    static FruInfo from_json(const Json::Value& json);

private:
    OptionalField<std::string> m_serial_number{};
    OptionalField<std::string> m_manufacturer{};
    OptionalField<std::string> m_model_number{};
    OptionalField<std::string> m_part_number{};
};

}
}
}

#endif /* AGENT_FRAMEWORK_MODULE_MODEL_ATTRIBUTES_FRU_INFO_HPP */
