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
 * @file cpu_id.hpp
 * @brief Basic CPU ID attribute class
 * */

#pragma once

#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"



namespace agent_framework {
namespace model {
namespace attribute {

/*!
 * @brief Class representing CPUID attribute
 * */
class CpuId {
public:

    explicit CpuId();


    CpuId(const std::string& vendor_id,
          const std::string& numeric_id,
          const std::string& family,
          const std::string& model,
          const std::string& step,
          const std::string& microcode_info) :
        m_vendor_id{vendor_id},
        m_numeric_id{numeric_id},
        m_family{family},
        m_model{model},
        m_step{step},
        m_microcode_info{microcode_info} {}


    virtual ~CpuId();


    /*! Enable copy */
    CpuId(const CpuId&) = default;


    CpuId& operator=(const CpuId&) = default;


    CpuId(CpuId&&) = default;


    CpuId& operator=(CpuId&&) = default;


    /*!
     * @brief Get vendor id
     * @return Vendor id
     * */
    const OptionalField<std::string>& get_vendor_id() const {
        return m_vendor_id;
    }


    /*!
    * @brief Set vendor id for response
    * @param[in] vendor_id  Vendor id
    * */
    void set_vendor_id(const OptionalField<std::string>& vendor_id) {
        m_vendor_id = vendor_id;
    }


    /*!
     * @brief Get numeric id
     * @return numeric id
     * */
    const OptionalField<std::string>& get_numeric_id() const {
        return m_numeric_id;
    }


    /*!
     * @brief Set numeric id for response
     * @param[in] numeric_id Numeric id
     * */
    void set_numeric_id(const OptionalField<std::string>& numeric_id) {
        m_numeric_id = numeric_id;
    }


    /*!
     * @brief Get family
     * @return family
     * */
    const OptionalField<std::string>& get_family() const {
        return m_family;
    }


    /*!
     * @brief Set processor family for response
     * @param[in] family Processor family
     * */
    void set_family(const OptionalField<std::string>& family) {
        m_family = family;
    }


    /*!
     * @brief Get model
     * @return model
     * */
    const OptionalField<std::string>& get_model() const {
        return m_model;
    }


    /*!
     * @brief Set processor model for response
     * @param[in] model Processor model
     * */
    void set_model(const OptionalField<std::string>& model) {
        m_model = model;
    }


    /*!
     * @brief Get step
     * @return step
     * */
    const OptionalField<std::string>& get_step() const {
        return m_step;
    }


    /*!
     * @brief Set processor step for response
     * @param[in] step Processor step
     * */
    void set_step(const OptionalField<std::string>& step) {
        m_step = step;
    }


    /*!
     * @brief Get microcode info
     * @return microcode info
     * */
    const OptionalField<std::string>& get_microcode_info() const {
        return m_microcode_info;
    }


    /*!
     * @brief Set microcode info for response
     * @param[in] microcode_info Microcode info
     * */
    void set_microcode_info(const OptionalField<std::string>& microcode_info) {
        m_microcode_info = microcode_info;
    }


    /*!
     * @brief construct an object of class CpuId from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed CpuId object
     */
    static CpuId from_json(const json::Json& json);


    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;


private:

    OptionalField<std::string> m_vendor_id{};
    OptionalField<std::string> m_numeric_id{};
    OptionalField<std::string> m_family{};
    OptionalField<std::string> m_model{};
    OptionalField<std::string> m_step{};
    OptionalField<std::string> m_microcode_info{};

};

}
}
}
