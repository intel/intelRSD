/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file acl_vlan_id.hpp
 * @brief VLAN ID with a mask for ACL rules.
 * */
#pragma once

#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <cstdint>

namespace agent_framework {
namespace model {
namespace attribute {

/*! AclVlanId id and mask */
class AclVlanId {
public:

    explicit AclVlanId();

    AclVlanId(const OptionalField<uint32_t>& id,
              const OptionalField<uint32_t>& mask):
        m_id(id),
        m_mask(mask)
    {}

    virtual ~AclVlanId();

    /*! Enable copy */
    AclVlanId(const AclVlanId&) = default;
    AclVlanId& operator=(const AclVlanId&) = default;
    AclVlanId(AclVlanId&&) = default;
    AclVlanId& operator=(AclVlanId&&) = default;

    /*!
     * Set VLAN id
     *
     * @param[in] id VLAN id
     * */
    void set_id(const OptionalField<uint32_t>& id) {
        m_id = id;
    }

    /*!
     * Get VLAN id
     *
     * @return VLAN id
     * */
    const OptionalField<uint32_t>& get_id() const {
        return m_id;
    }

    /*!
     * Set VLAN mask
     *
     * @param[in] mask VLAN mask
     * */
    void set_mask(const OptionalField<uint32_t>& mask) {
        m_mask = mask;
    }

    /*!
     * Get VLAN mask
     *
     * @return VLAN mask
     * */
    const OptionalField<uint32_t>& get_mask() const {
        return m_mask;
    }

    /*!
     * @brief Convert AclVlanId to JSON object
     *
     * @return JSON representation of AclVlanId object
     * */
    json::Json to_json() const;

    /*!
     * @brief construct an object of class AclVlanId from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed Ipv object
     * */
    static AclVlanId from_json(const json::Json& json);

private:

    OptionalField<uint32_t> m_id{};
    OptionalField<uint32_t> m_mask{};
};

}
}
}
