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
 * @file acl_port.hpp
 * @brief Port with a mask for ACL rules.
 * */
#pragma once



#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <cstdint>

namespace agent_framework {
namespace model {
namespace attribute {

/*! AclPort port and mask */
class AclPort {
public:

    explicit AclPort();

    AclPort(const OptionalField<uint32_t>& port,
            const OptionalField<uint32_t>& mask):
        m_port(port),
        m_mask(mask)
    {}

    virtual ~AclPort();

    /*! Enable copy */
    AclPort(const AclPort&) = default;
    AclPort& operator=(const AclPort&) = default;
    AclPort(AclPort&&) = default;
    AclPort& operator=(AclPort&&) = default;

    /*!
     * Set port
     *
     * @param[in] port Port
     * */
    void set_port(const OptionalField<uint32_t>& port) {
        m_port = port;
    }

    /*!
     * Get port
     *
     * @return Port
     * */
    const OptionalField<uint32_t>& get_port() const {
        return m_port;
    }

    /*!
     * Set Port mask
     *
     * @param[in] mask Port mask
     * */
    void set_mask(const OptionalField<uint32_t>& mask) {
        m_mask = mask;
    }

    /*!
     * Get Port mask
     *
     * @return Mask
     * */
    const OptionalField<uint32_t>& get_mask() const {
        return m_mask;
    }

    /*!
     * @brief Convert AclPort to JSON object
     *
     * @return JSON representation of AclPort object
     * */
    json::Json to_json() const;

    /*!
     * @brief construct an object of class AclPort from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed Ipv object
     */
    static AclPort from_json(const json::Json& json);

private:
    OptionalField<uint32_t> m_port{};
    OptionalField<uint32_t> m_mask{};
};

}
}
}
