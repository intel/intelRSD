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
 * @file ipv6_address.hpp
 * @brief IPv6 address
 * */
#pragma once

#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! IPv6 address */
class Ipv6Address {
public:

    explicit Ipv6Address();

    Ipv6Address(const std::string& address,
                const std::uint32_t prefix_length,
                const enums::Ipv6AddressOrigin& address_origin,
                const enums::Ipv6AddressState& address_state):
        m_address(address),
        m_prefix_length(prefix_length),
        m_address_origin(address_origin),
        m_address_state(address_state)
    {}

    ~Ipv6Address();

    /*! Enable copy */
    Ipv6Address(const Ipv6Address&) = default;
    Ipv6Address& operator=(const Ipv6Address&) = default;
    Ipv6Address(Ipv6Address&&) = default;
    Ipv6Address& operator=(Ipv6Address&&) = default;

    /*!
     * Sets IP address
     *
     * @param[in] address IP address.
     * */
    void set_address(const OptionalField<std::string>& address) {
        m_address = address;
    }

    /*!
     * Gets IP address.
     * */
    const OptionalField<std::string> get_address() const {
        return m_address;
    }

    /*!
     * Sets prefix length
     *
     * @param[in] prefix_length Prefix length
     * */
    void set_prefix_length(const OptionalField<std::uint32_t>& prefix_length) {
        m_prefix_length = prefix_length;
    }

    /*!
     * Gets prefix length.
     * */
    const OptionalField<std::uint32_t>& get_prefix_length() const {
        return m_prefix_length;
    }

    /*!
     * Sets address origin
     *
     * @param[in] address_origin Address origin
     * */
    void set_address_origin(const OptionalField<enums::Ipv6AddressOrigin>& address_origin) {
        m_address_origin = address_origin;
    }

    /*!
     * Gets IP address origin.
     * */
    const OptionalField<enums::Ipv6AddressOrigin>& get_address_origin() const {
        return m_address_origin;
    }

    /*!
     * Sets address state
     *
     * @param[in] address_state Address state.
     * */
    void set_address_state(const OptionalField<enums::Ipv6AddressState>& address_state) {
        m_address_state = address_state;
    }

    /*!
     * Gets address state.
     * */
    const OptionalField<enums::Ipv6AddressState>& get_address_state() const {
        return m_address_state;
    }

    /*!
     * @brief Convert Ipv6Address to JSON object
     * @return JSON representation of Ipv6Address object
     * */
    json::Json to_json() const;

    /*!
     * @brief construct an object of class Ipv from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed Ipv object
     */
    static Ipv6Address from_json(const json::Json& json);

private:

    OptionalField<std::string> m_address{};
    OptionalField<std::uint32_t> m_prefix_length{};
    OptionalField<enums::Ipv6AddressOrigin> m_address_origin{};
    OptionalField<enums::Ipv6AddressState> m_address_state{};

};

}
}
}
