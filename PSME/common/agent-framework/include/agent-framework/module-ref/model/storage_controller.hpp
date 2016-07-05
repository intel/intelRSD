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
 * @file storage_controller.hpp
 * @brief Mass storage controller
 * */

#ifndef AGENT_FRAMEWORK_MODULE_MODEL_STORAGE_CONTROLLER_HPP
#define AGENT_FRAMEWORK_MODULE_MODEL_STORAGE_CONTROLLER_HPP

#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"
#include "agent-framework/module-ref/model/resource.hpp"
#include "agent-framework/module-ref/enum/common.hpp"
#include "agent-framework/module-ref/enum/storage.hpp"

namespace agent_framework {
namespace model {

/*! Storage Controller */
class StorageController : public Resource {
public:
    explicit StorageController(const std::string& parent_uuid = {});
    ~StorageController();

    StorageController(const StorageController&) = default;
    StorageController& operator=(const StorageController&) = default;
    StorageController(StorageController&&) = default;
    StorageController& operator=(StorageController&&) = default;

    /*!
     * @brief construct an object of class StorageController from JSON
     *
     * @param json the Json::Value deserialized to object
     *
     * @return the newly constructed StorageController object
     */
    static StorageController from_json(const Json::Value& json);

    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to Json::Value
     */
    Json::Value to_json() const;

    /**
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return StorageController::collection_name;
    }

    /**
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return StorageController::component;
    }

    /*!
     * Returns Storage Controller interface
     * @return Storage Controller interface
     * */
    OptionalField<enums::DriveInterface> get_interface() const {
        return m_interface;
    }

    /*!
     * @brief Set interface
     * @param[in] interface interface
     * */
    void set_interface(const OptionalField<enums::DriveInterface> interface) {
        m_interface = interface;
    }

    /*!
     * @brief Set physical id
     *
     * @param physical_id bus type and location
     * */
    void set_physical_id(const OptionalField<std::string>& physical_id) {
        m_physical_id = physical_id;
    }

    /*!
     * @brief Get physical id
     *
     * @return String with bus type and location
     * */
    const OptionalField<std::string>& get_physical_id() const {
        return m_physical_id;
    }

    /*!
     * @brief Returns FRUInfo.
     * @return Reference to FRUInfo.
     * */
    const attribute::FruInfo& get_fru_info() const { return m_fru_info; }

    /*!
     * @brief Set fru info
     * @param[in] fru_info FruInfo
     * */
    void set_fru_info(const attribute::FruInfo& fru_info) {
        m_fru_info = fru_info;
    }

private:

    OptionalField<enums::DriveInterface> m_interface{enums::DriveInterface::PCIe};
    attribute::FruInfo m_fru_info{};
    OptionalField<std::string> m_physical_id{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_MODEL_STORAGE_CONTROLLER_HPP */
