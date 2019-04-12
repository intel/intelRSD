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
 * @file acl_address.hpp
 * @brief Address (IP or MAC) with a mask for ACL rules.
 * */
#pragma once



#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>



namespace agent_framework {
namespace model {
namespace attribute {

/*! AclAddress Address and mask */
class AclAddress {
public:

    explicit AclAddress();


    AclAddress(const OptionalField<std::string>& address,
               const OptionalField<std::string>& mask) :
        m_address(address),
        m_mask(mask) {}


    virtual ~AclAddress();


    /*! Enable copy */
    AclAddress(const AclAddress&) = default;


    AclAddress& operator=(const AclAddress&) = default;


    AclAddress(AclAddress&&) = default;


    AclAddress& operator=(AclAddress&&) = default;


    /*!
     * Set address
     *
     * @param[in] address Address.
     * */
    void set_address(const OptionalField<std::string>& address) {
        m_address = address;
    }


    /*!
     * Get Address.
     * @return Address
     * */
    const OptionalField<std::string>& get_address() const {
        return m_address;
    }


    /*!
     * Set Address mask
     *
     * @param[in] mask Address mask
     * */
    void set_mask(const OptionalField<std::string>& mask) {
        m_mask = mask;
    }


    /*!
     * Get Address mask.
     * @return Mask
     * */
    const OptionalField<std::string>& get_mask() const {
        return m_mask;
    }


    /*!
     * @brief Convert AclAddress to JSON object
     * @return JSON representation of AclAddress object
     * */
    json::Json to_json() const;


    /*!
     * @brief construct an object of class AclAddress from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed Ipv object
     */
    static AclAddress from_json(const json::Json& json);


private:
    OptionalField<std::string> m_address{};
    OptionalField<std::string> m_mask{};
};

}
}
}
