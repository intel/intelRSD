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
 * @file volume.hpp
 * @brief Declaration of Volume class
 * */

#pragma once



#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/common.hpp"



namespace agent_framework {
namespace model {

class Volume : public Resource {
public:

    using EncryptionTypes = attribute::Array<enums::EncryptionType>;
    using CapacitySources = attribute::Array<attribute::CapacitySource>;
    using Identifiers = attribute::Array<attribute::Identifier>;
    using ReplicaInfos = attribute::Array<attribute::ReplicaInfo>;
    using AccessCapabilities = attribute::Array<enums::AccessCapability>;


    explicit Volume(const Uuid& parent_uuid = {},
                    enums::Component parent_type = enums::Component::None);


    virtual ~Volume();


    Volume(const Volume&) = default;


    Volume& operator=(const Volume&) = default;


    Volume(Volume&&) = default;


    Volume& operator=(Volume&&) = default;


    /*!
     * @brief Construct an object of class Volume from JSON
     * @param json the json::Json deserialized to object
     * @return The newly constructed Volume object
     * */
    static Volume from_json(const json::Json& json);


    /*!
     * @brief Transform the object to JSON
     * @return The object serialized to json::Json
     * */
    json::Json to_json() const;


    /*!
     * @brief Get collection name
     * @return Collection name
     */
    static enums::CollectionName get_collection_name() {
        return Volume::collection_name;
    }


    /*!
     * @brief Get component name
     * @return Component name
     */
    static enums::Component get_component() {
        return Volume::component;
    }


    /*!
     * @brief Get capacity
     * @return capacity
     */
    const attribute::Capacity& get_capacity() const {
        return m_capacity;
    }


    /*!
     * @brief Set capacity
     * @param[in] capacity capacity
     */
    void set_capacity(const attribute::Capacity& capacity) {
        m_capacity = capacity;
    }


    /*!
     * @brief Get Capacity Sources
     * @return capacity sources
     */
    const CapacitySources& get_capacity_sources() const {
        return m_capacity_sources;
    }


    /*!
     * @brief Set capacity sources
     * @param[in] capacity_sources Capacity Sources
     */
    void set_capacity_sources(const CapacitySources& capacity_sources) {
        m_capacity_sources = capacity_sources;
    }


    /*!
     * @brief Add capacity source
     * @param[in] capacity_source Capacity source to add
     * */
    void add_capacity_source(const attribute::CapacitySource& capacity_source) {
        m_capacity_sources.add_entry(capacity_source);
    }


    /*!
     * @brief Gets block size bytes
     * @return block_size bytes
     * */
    const OptionalField<std::uint64_t>& get_block_size_bytes() const {
        return m_block_size_bytes;
    }


    /*!
     * @brief Sets block size bytes
     * @param[in] block_size_bytes of type uint64_t
     */
    void set_block_size_bytes(const OptionalField<std::uint64_t>& block_size_bytes) {
        m_block_size_bytes = block_size_bytes;
    }


    /*!
     * @brief Gets optimum IO size bytes
     * @return optimum_io_size bytes
     * */
    const OptionalField<std::uint64_t>& get_optimum_io_size_bytes() const {
        return m_optimum_io_size_bytes;
    }


    /*!
     * @brief Sets optimum IO size bytes
     * @param[in] optimum_io_size_bytes of type uint64_t
     */
    void set_optimum_io_size_bytes(const OptionalField<std::uint64_t>& optimum_io_size_bytes) {
        m_optimum_io_size_bytes = optimum_io_size_bytes;
    }


    /*!
     * @brief Gets volume type
     * @return Volume type
     * */
    const OptionalField<enums::VolumeType>& get_volume_type() const {
        return m_volume_type;
    }


    /*!
     * @brief Sets volume type
     * @param[in] volume_type Volume type
     * */
    void set_volume_type(const OptionalField<enums::VolumeType>& volume_type) {
        m_volume_type = volume_type;
    }


    /*!
     * @brief Gets encryption state
     * @return Encryption state
     * */
    const OptionalField<bool>& is_encrypted() const {
        return m_encrypted;
    }


    /*!
     * @brief Sets encryption state
     * @param[in] encrypted Encryption state
     * */
    void set_encrypted(const OptionalField<bool>& encrypted) {
        m_encrypted = encrypted;
    }


    /*!
     * @brief Get volume encryption types
     * @return array with encryption_types
     */
    const EncryptionTypes& get_encryption_types() const {
        return m_encryption_types;
    }


    /*!
     * @brief Set encryption types
     * @param[in] encryption_types array with encryption types
     */
    void set_encryption_types(const EncryptionTypes& encryption_types) {
        m_encryption_types = encryption_types;
    }


    /*!
     * @brief Add encryption type
     * @param[in] encryption_type supported type of encryption
     */
    void add_encryption_type(const enums::EncryptionType& encryption_type) {
        m_encryption_types.add_entry(encryption_type);
    }


    /*!
     * @brief Get volume's identifiers
     * @return array with volume's identifiers
     */
    const Identifiers& get_identifiers() const {
        return m_identifiers;
    }


    /*!
     * @brief Set volume's identifiers
     * @param[in] identifiers array with volume's identifiers
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


    /*!
     * @brief Gets bootable status
     * @return optimum_io_size bytes
     * */
    const OptionalField<bool>& is_bootable() const {
        return m_bootable;
    }


    /*!
     * @brief Sets bootable status
     * @param[in] bootable of type boolean
     */
    void set_bootable(const OptionalField<bool>& bootable) {
        m_bootable = bootable;
    }


    /*!
     * @brief Get volume's array of replica information
     * @return array with volume's replica information
     */
    const ReplicaInfos& get_replica_infos() const {
        return m_replica_infos;
    }


    /*!
     * @brief Set volume's array of replica information
     * @param[in] replica_infos array with volume's replica information
     */
    void set_replica_infos(const ReplicaInfos& replica_infos) {
        m_replica_infos = replica_infos;
    }


    /*!
     * @brief Add replica information
     * @param[in] replica_info replica information
     */
    void add_replica_info(const attribute::ReplicaInfo& replica_info) {
        m_replica_infos.add_entry(replica_info);
    }


    /*!
     * @brief Get volume's array of access capabilities
     * @return An array with volume's access capabilities
     */
    const AccessCapabilities& get_access_capabilities() const {
        return m_access_capabilities;
    }


    /*!
     * @brief Set volume's array of access capabilities
     * @param[in] access_capabilities An array with volume's access capabilities
     */
    void set_access_capabilities(const AccessCapabilities& access_capabilities) {
        m_access_capabilities = access_capabilities;
    }


    /*!
     * @brief Add access capability
     * @param[in] access_capability Access capability of the volume
     * */
    void add_access_capability(const enums::AccessCapability& access_capability) {
        m_access_capabilities.add_entry(access_capability);
    }

    /*!
     * @brief Check if volume is being initialized
     * @return True if volume is being initalized
     */
    bool get_is_being_initialized() const {
        return m_is_being_initialized;
    }

    /*!
     * @brief Sets is being initialized flag
     * @param is_being_initialized New flag value
     */
    void set_is_being_initialized(bool is_being_initialized) {
        m_is_being_initialized = is_being_initialized;
    }

    /*!
     * @brief Checks if volume is connected to any endpoint from all available
     * @param volume_uuid UUID of the volume
     * @return Returns true if volume is connected to endpoint, false otherwise
     */
    static bool is_volume_connected_to_endpoint(const Uuid& volume_uuid);

    /*!
     * @brief Checks if volume is connected to endpoint
     * @param endpoint_uuids Requested collection of endpoints
     * @param volume_uuid UUID of the volume to check
     * @return Returns true if volume is connected to endpoint, false otherwise
     */
    static bool is_volume_connected_to_endpoint(const std::vector<Uuid>& endpoint_uuids, const Uuid& volume_uuid);

    /*!
     * @brief Check if volume is shared over fabric's zones.
     * @param volume_uuid UUID of the volume.
     * @return Returns true if volume is shared, false otherwise.
     */
    static bool is_volume_shared_over_fabrics(const Uuid& volume_uuid);


    /*!
     * @brief Check if volume is a snapshot replica for another volume.
     * @param volume_uuid UUID of the volume.
     * @return Returns true if volume is snapshot, false otherwise.
     */
    static bool is_volume_snapshot(const Uuid& volume_uuid);


    /*!
     * @brief Check if volume is a snapshot replica for another volume.
     * @param volume Reference to volume.
     * @return Returns true if volume is snapshot, false otherwise.
     */
    static bool is_volume_snapshot(const Volume& volume);

    /*!
     * @brief Check if volume has replica and it is a source for any of these replicas
     * @param volume Reference to volume.
     * @return Returns true if volume is source, false otherwise.
     */
    static bool is_source_for_another_volume(const Volume& volume);


private:
    attribute::Capacity m_capacity{};
    CapacitySources m_capacity_sources{};
    OptionalField<std::uint64_t> m_block_size_bytes{};
    OptionalField<std::uint64_t> m_optimum_io_size_bytes{};
    OptionalField<enums::VolumeType> m_volume_type{};
    OptionalField<bool> m_encrypted{};
    EncryptionTypes m_encryption_types{};
    Identifiers m_identifiers{};
    OptionalField<bool> m_bootable{};
    ReplicaInfos m_replica_infos{};
    AccessCapabilities m_access_capabilities{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

    // Agents' specific fields
    bool m_is_being_initialized{false}; // flag to block multiple initialization requests in the same time
};

}
}
