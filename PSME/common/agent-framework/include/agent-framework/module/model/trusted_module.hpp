/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file trusted_module.hpp
 * @brief Definition of Trusted Module class
 * */

#pragma once



#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/compute.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

class TrustedModule : public Resource {
public:

    explicit TrustedModule(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::None);


    ~TrustedModule();


    /*! Enable copy */
    TrustedModule(const TrustedModule&) = default;


    TrustedModule& operator=(const TrustedModule&) = default;


    TrustedModule(TrustedModule&&) = default;


    TrustedModule& operator=(TrustedModule&&) = default;


    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return TrustedModule::collection_name;
    }


    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return TrustedModule::component;
    }


    /*!
     * @brief Sets firmware version
     * @param[in] firmware_version The firmware version of TPM.
     * */
    void set_firmware_version(const OptionalField<std::string>& firmware_version) {
        m_firmware_version = firmware_version;
    }


    /*!
     * @brief Gets firmware version.
     * @return The firmware version of TPM.
     * */
    const OptionalField<std::string>& get_firmware_version() const {
        return m_firmware_version;
    }


    /*!
     * @brief Sets interface type.
     * @param[in] interface_type TPM interface type.
     * */
    void set_interface_type(const OptionalField<enums::InterfaceType>& interface_type) {
        m_interface_type = interface_type;
    }


    /*!
     * @brief Gets interface type.
     * @return A interface type of TMP.
     * */
    const OptionalField<enums::InterfaceType>& get_interface_type() const {
        return m_interface_type;
    }


    /*!
     * @brief Sets configuration index.
     * @param[in] configuration_index Configuration index used by IPMI.
     * */
    void set_configuration_index(std::uint8_t configuration_index) {
        m_configuration_index = configuration_index;
    }


    /*!
     * @brief Gets configuration index.
     * @return Configuration index used by IPMI.
     * */
    std::uint8_t get_configuration_index() const {
        return m_configuration_index;
    }


    /*!
     * @brief Get TPM task
     * @return TPM task
     * */
    const OptionalField<std::string>& get_task() const {
        return m_task;
    }


    /*!
     * @brief Set TPM task
     * @param[in] task the TPM task
     * */
    void set_task(const OptionalField<std::string>& task) {
        m_task = task;
    }


    /*!
     * @brief Converts this to json representation.
     * @return json representation of this class.
     * */
    json::Json to_json() const;


    /*!
     * @brief construct an object of class FruInfo from JSON
     *
     * @param[in] json the json::Json deserialized to object
     *
     * @return the newly constructed FruInfo object
     */
    static TrustedModule from_json(const json::Json& json);


private:
    OptionalField<std::string> m_firmware_version{};
    OptionalField<enums::InterfaceType> m_interface_type{};

    /*! IPMI specific data */
    std::uint8_t m_configuration_index{};

    /*!
     * @brief m_task - internal flag. If it's not empty, it points
     * to a running task whose purpose is to set the TPM's state.
     * Used to prevent duplicate tasks for the same TPM.
     */
    OptionalField<Uuid> m_task{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;
};

}
}
