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
 * @file storage_controller.hpp
 * @brief Mass storage controller
 * */

#pragma once



#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/storage.hpp"



namespace agent_framework {
namespace model {

/*! Storage Controller */
class StorageController : public Resource {
public:
    using Identifiers = attribute::Array<attribute::Identifier>;
    using StorageProtocols = attribute::Array<enums::TransportProtocol>;

    explicit StorageController(const std::string& parent_uuid = {},
                               enums::Component parent_type = enums::Component::System);


    ~StorageController();


    StorageController(const StorageController&) = default;


    StorageController& operator=(const StorageController&) = default;


    StorageController(StorageController&&) = default;


    StorageController& operator=(StorageController&&) = default;


    /*!
     * @brief construct an object of class StorageController from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed StorageController object
     */
    static StorageController from_json(const json::Json& json);


    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;


    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return StorageController::collection_name;
    }


    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return StorageController::component;
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


    /*!
     * @brief Set asset tag
     *
     * @param asset_tag storage controller's asset tag
     * */
    void set_asset_tag(const OptionalField<std::string>& asset_tag) {
        m_asset_tag = asset_tag;
    }


    /*!
     * @brief Get asset tag
     *
     * @return String with the storage controller's asset tag
     * */
    const OptionalField<std::string>& get_asset_tag() const {
        return m_asset_tag;
    }


    /*!
     * @brief Set sku
     *
     * @param sku storage controller's sku
     * */
    void set_sku(const OptionalField<std::string>& sku) {
        m_sku = sku;
    }


    /*!
     * @brief Get sku
     *
     * @return String with the storage controller's sku
     * */
    const OptionalField<std::string>& get_sku() const {
        return m_sku;
    }


    /*!
     * @brief Set firmware version
     *
     * @param firmware_version storage controller's firmware version
     * */
    void set_firmware_version(const OptionalField<std::string>& firmware_version) {
        m_firmware_version = firmware_version;
    }


    /*!
     * @brief Get firmware version
     *
     * @return String with the storage controller's firmware version
     * */
    const OptionalField<std::string>& get_firmware_version() const {
        return m_firmware_version;
    }


    /*!
     * @brief Set speed gbps
     *
     * @param speed_gbps storage controller's speed gbps
     * */
    void set_speed_gbps(const OptionalField<double>& speed_gbps) {
        m_speed_gbps = speed_gbps;
    }


    /*!
     * @brief Get speed gbps
     *
     * @return String with the storage controller's speed gbps
     * */
    const OptionalField<double>& get_speed_gbps() const {
        return m_speed_gbps;
    }


    /*!
     * @brief Get supported controller storage protocols
     * @return array with supported controller storage protocols
     */
    const StorageProtocols& get_supported_controller_protocols() const {
        return m_supported_controller_protocols;
    }


    /*!
     * @brief Set supported controller storage protocols
     * @param[in] supported_controller_protocols array with supported types of storage protocols
     */
    void set_supported_controller_protocols(const StorageProtocols& supported_controller_protocols) {
        m_supported_controller_protocols = supported_controller_protocols;
    }


    /*!
     * @brief Add supported controller storage protocol
     * @param[in] supported_controller_protocol supported type of storage protocol
     */
    void add_supported_controller_protocol(const enums::TransportProtocol& supported_controller_protocol) {
        m_supported_controller_protocols.add_entry(supported_controller_protocol);
    }


    /*!
     * @brief Get supported device storage protocols
     * @return array with supported device storage protocols
     */
    const StorageProtocols& get_supported_device_protocols() const {
        return m_supported_device_protocols;
    }


    /*!
     * @brief Set supported device storage protocols
     * @param[in] supported_device_protocols array with supported types of storage protocols
     */
    void set_supported_device_protocols(const StorageProtocols& supported_device_protocols) {
        m_supported_device_protocols = supported_device_protocols;
    }


    /*!
     * @brief Add supported device storage protocol
     * @param[in] supported_device_protocol supported type of storage protocol
     */
    void add_supported_device_protocol(const enums::TransportProtocol& supported_device_protocol) {
        m_supported_device_protocols.add_entry(supported_device_protocol);
    }


    /*!
     * @brief Get storage controller's identifiers
     * @return array with storage controller's identifiers
     */
    const Identifiers& get_identifiers() const {
        return m_identifiers;
    }


    /*!
     * @brief Set storage controller's identifiers
     * @param[in] identifiers array with storage controller's identifiers
     */
    void set_identifiers(const Identifiers& identifiers) {
        m_identifiers = identifiers;
    }


    /*!
     * @brief Add device identifier
     * @param[in] identifier supported type of identifier
     */
    void add_identifier(const attribute::Identifier& identifier) {
        m_identifiers.add_entry(identifier);
    }

private:
    attribute::FruInfo m_fru_info{};
    OptionalField<std::string> m_physical_id{};
    OptionalField<double> m_speed_gbps{};
    OptionalField<std::string> m_firmware_version{};
    OptionalField<std::string> m_sku{};
    OptionalField<std::string> m_asset_tag{};
    Identifiers m_identifiers{};
    StorageProtocols m_supported_controller_protocols{};
    StorageProtocols m_supported_device_protocols{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}

