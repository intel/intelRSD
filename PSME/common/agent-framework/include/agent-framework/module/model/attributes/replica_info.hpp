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
 * @file replica_info.hpp
 * @brief ReplicaInfo
 * */

#pragma once



#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

namespace agent_framework {
namespace model {
namespace attribute {


/*! ReplicaInfo class */
class ReplicaInfo {
public:

    /*! Default constructor */
    explicit ReplicaInfo() {}
    ReplicaInfo(ReplicaInfo&&) = default;
    ReplicaInfo(const ReplicaInfo&) = default;
    ReplicaInfo& operator=(ReplicaInfo&&) = default;
    ReplicaInfo& operator=(const ReplicaInfo&) = default;

    virtual ~ReplicaInfo();



    /*!
     * @brief Gets replica UUID
     * @return replica UUID
     * */
    const OptionalField<Uuid>& get_replica() const {
        return m_replica;
    }

    /*!
     * @brief Sets replica UUID
     * @param[in] replica UUID of type string
     */
    void set_replica(const OptionalField<Uuid>& replica) {
        m_replica = replica;
    }

    /*!
     * @brief Gets replica read only access mode
     * @return replica_read_only_access replica read only access mode
     */
    const OptionalField<enums::ReplicaReadOnlyAccess>& get_replica_read_only_access() const {
        return m_replica_read_only_access;
    }

    /*!
     * @brief Sets replica read only access mode
     * @param[in] replica_read_only_access replica read only access mode
     */
    void set_replica_read_only_access(const OptionalField<enums::ReplicaReadOnlyAccess>& replica_read_only_access) {
        m_replica_read_only_access = replica_read_only_access;
    }

    /*!
     * @brief Gets replica type
     * @return replica_type replica type
     */
    const OptionalField<enums::ReplicaType>& get_replica_type() const {
        return m_replica_type;
    }

    /*!
     * @brief Sets replica type
     * @param[in] replica_type replica type
     */
    void set_replica_type(const OptionalField<enums::ReplicaType>& replica_type) {
        m_replica_type = replica_type;
    }

    /*!
     * @brief Gets replica role
     * @return replica_role replica role
     */
    const OptionalField<enums::ReplicaRole>& get_replica_role() const {
        return m_replica_role;
    }

    /*!
     * @brief Sets replica role
     * @param[in] replica_role replica role
     */
    void set_replica_role(const OptionalField<enums::ReplicaRole>& replica_role) {
        m_replica_role = replica_role;
    }

    /*!
     * Converts this object to json representation
     * @return Json representation of this object
     * */
    json::Json to_json() const;

    /*!
     * @brief construct an object of class ReplicaInfo from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed ReplicaInfo object
     */
    static ReplicaInfo from_json(const json::Json& json);

private:
    OptionalField<enums::ReplicaReadOnlyAccess> m_replica_read_only_access{};
    OptionalField<enums::ReplicaType> m_replica_type{};
    OptionalField<enums::ReplicaRole> m_replica_role{};
    OptionalField<Uuid> m_replica{};
};

}
}
}
