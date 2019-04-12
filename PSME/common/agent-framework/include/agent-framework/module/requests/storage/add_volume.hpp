/*!
 * @brief Storage AddVolume request
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file add_volume.hpp
 */

#pragma once



#include "agent-framework/module/model/model_storage.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include "agent-framework/module/utils/optional_field.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module/constants/regular_expressions.hpp"
#include "agent-framework/module/model/attributes/identifier.hpp"
#include "agent-framework/module/constants/common.hpp"

#include <vector>
#include <string>



namespace agent_framework {
namespace model {
namespace requests {

/*! AddLogicalDrive request */
class AddVolume {
public:

    using Identifiers = agent_framework::model::attribute::Array<agent_framework::model::attribute::Identifier>;


    static std::string get_command() {
        return literals::Command::ADD_VOLUME;
    }


    /*! @brief Constructor */
    explicit AddVolume(
        const OptionalField<enums::VolumeType>& volume_type,
        const Identifiers& identifiers,
        std::int64_t capacity_bytes,
        const attribute::Array<attribute::CapacitySource>& capacity_sources,
        const attribute::Array<enums::AccessCapability>& access_capabilities,
        const OptionalField<bool>& bootable,
        const attribute::Array<attribute::ReplicaInfo>& replica_infos,
        const attribute::Oem& oem = attribute::Oem{}
    );


    /*!
     * @brief Transform request to JSON
     * @return Created JSON value
     */
    json::Json to_json() const;


    /*!
     * @brief Create AddVolume from JSON
     *
     * @param[in] json The input argument
     *
     * @return New AddVolume
     */
    static AddVolume from_json(const json::Json& json);

    /*!
     * @brief Get volume type.
     * @return Volume type
     */
    const OptionalField<enums::VolumeType>& get_volume_type() const {
        return m_volume_type;
    }


    /*!
     * @brief Get identifiers.
     *
     * @return identifiers
     */
    const Identifiers& get_identifiers() const {
        return m_identifiers;
    }

    /*!
     * @brief Set identifiers.
     *
     * @param identifiers
     */
    void set_identifiers(const Identifiers& identifiers) {
        m_identifiers = identifiers;
    }


    /*!
     * @brief Add  identifier
     *
     * @param identifier
     */
    void add_identifier(const attribute::Identifier& identifier) {
        m_identifiers.add_entry(identifier);
    }


    /*!
     * @brief Get capacity of the volume.
     * @return Capacity in bytes.
     */
    std::int64_t get_capacity_bytes() const {
        return m_capacity_bytes;
    }


    /*!
     * @brief Get capacity sources collection of the volume.
     * @return Array of capacity sources.
     */
    const attribute::Array<attribute::CapacitySource>& get_capacity_sources() const {
        return m_capacity_sources;
    }


    /*!
     * @brief Get the volume's access capabilities collection.
     * @return Array of access capability enums.
     */
    const attribute::Array<enums::AccessCapability>& get_access_capabilities() const {
        return m_access_capabilities;
    }


    /*!
    * @brief Indicates if the volume should be marked as bootable after creation.
    * @return Returns true if is bootable.
    * */
    const OptionalField<bool>& get_bootable() const {
        return m_bootable;
    }


    /*!
     * @brief Get replica info collection.
     * @return Collection of replica info.
     */
    const attribute::Array<attribute::ReplicaInfo>& get_replica_infos() const {
        return m_replica_infos;
    }


    /*!
     * @brief Get OEM specific data
     * @return Volume's OEM specific data
     * */
    const attribute::Oem& get_oem() const {
        return m_oem;
    }


    /*!
     * @brief Returns procedure scheme
     * @return ProcedureValidator scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_STRING,
            literals::Volume::VOLUME_TYPE, VALID_NULLABLE(VALID_ENUM(enums::VolumeType)),
            literals::Volume::IDENTIFIERS, VALID_ARRAY_OF(VALID_ATTRIBUTE(IdentifierProcedure)),
            literals::Volume::ACCESS_CAPABILITIES, VALID_ARRAY_OF(VALID_ENUM(enums::AccessCapability)),
            literals::Volume::BOOTABLE, VALID_NULLABLE(VALID_JSON_BOOLEAN),
            literals::Volume::CAPACITY_BYTES, VALID_NUMERIC_RANGE(INT64, 0, INT64_MAX),
            literals::Volume::CAPACITY_SOURCES, VALID_ARRAY_SIZE_OF(VALID_ATTRIBUTE(CapacitySourceProcedure), 0, 1),
            literals::Volume::REPLICA_INFOS, VALID_ARRAY_SIZE_OF(VALID_ATTRIBUTE(ReplicaInfoProcedure), 0, 1),
            literals::Volume::OEM, VALID_JSON_OBJECT,
            nullptr
        };
        return procedure;
    }


private:
    OptionalField<enums::VolumeType> m_volume_type{};
    Identifiers m_identifiers{};
    std::int64_t m_capacity_bytes{};
    attribute::Array<attribute::CapacitySource> m_capacity_sources{};
    attribute::Array<enums::AccessCapability> m_access_capabilities{};
    OptionalField<bool> m_bootable{};
    attribute::Array<attribute::ReplicaInfo> m_replica_infos{};
    attribute::Oem m_oem{};

    class IdentifierProcedure {
    public:
        static const jsonrpc::ProcedureValidator& get_procedure() {
            static const jsonrpc::ProcedureValidator procedure{
                get_command(),
                jsonrpc::PARAMS_BY_NAME,
                literals::Identifier::DURABLE_NAME, VALID_REGEX(literals::regex::Common::EMPTY_OR_NO_WHITESPACE_STRING),
                literals::Identifier::DURABLE_NAME_FORMAT, VALID_ENUM(enums::IdentifierType),
                nullptr
            };
            return procedure;
        }
    };

    class CapacitySourceProcedure {
    public:
        static const jsonrpc::ProcedureValidator& get_procedure() {
            static jsonrpc::ProcedureValidator procedure{
                jsonrpc::PARAMS_BY_NAME,
                literals::Capacity::CONSUMED_BYTES, VALID_NULLABLE(VALID_NUMERIC_RANGE(INT64, 0, INT64_MAX)),
                literals::Capacity::ALLOCATED_BYTES, VALID_NULLABLE(VALID_NEVER), // Should throw when value is provided
                literals::Capacity::GUARANTEED_BYTES, VALID_NULLABLE(VALID_NEVER), // Should throw when value is provided
                literals::Capacity::PROVISIONED_BYTES, VALID_NULLABLE(VALID_NEVER), // Should throw when value is provided
                literals::CapacitySource::PROVIDING_DRIVES, VALID_NULLABLE(VALID_ARRAY_SIZE_OF(VALID_UUID, 0, 0)),
                literals::CapacitySource::PROVIDING_POOLS, VALID_NULLABLE(VALID_ARRAY_SIZE_OF(VALID_UUID, 0, 1)),
                literals::CapacitySource::PROVIDING_VOLUMES, VALID_NULLABLE(VALID_ARRAY_SIZE_OF(VALID_UUID, 0, 0)),
                nullptr
            };
            return procedure;
        }
    };

    class ReplicaInfoProcedure {
    public:
        static const jsonrpc::ProcedureValidator& get_procedure() {
            static jsonrpc::ProcedureValidator procedure{
                jsonrpc::PARAMS_BY_NAME,
                literals::ReplicaInfo::REPLICA_TYPE, VALID_NULLABLE(VALID_ENUM(enums::ReplicaType)),
                literals::ReplicaInfo::REPLICA, VALID_NULLABLE(VALID_UUID),
                literals::ReplicaInfo::REPLICA_READ_ONLY_ACCESS, VALID_NULLABLE(VALID_ENUM(enums::ReplicaReadOnlyAccess)),
                literals::ReplicaInfo::REPLICA_ROLE, VALID_NULLABLE(VALID_ENUM(enums::ReplicaRole)),
                nullptr
            };
            return procedure;
        }
    };
};

}
}
}
