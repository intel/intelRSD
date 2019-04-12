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
 * @file ipv4_address.hpp
 * @brief IPv4 address
 * */
#pragma once

#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! IPv4 address */
class Ipv4Address {

public:

    explicit Ipv4Address();

    Ipv4Address(const OptionalField<std::string>& address,
                const OptionalField<std::string>& subnet_mask = {},
                const OptionalField<enums::Ipv4AddressOrigin>& address_origin = {},
                const OptionalField<std::string>& gateway = {}):
        m_address(address),
        m_subnet_mask(subnet_mask),
        m_address_origin(address_origin),
        m_gateway(gateway) {}

    ~Ipv4Address();

    /*! Enable copy */
    Ipv4Address(const Ipv4Address&) = default;
    Ipv4Address& operator=(const Ipv4Address&) = default;
    Ipv4Address(Ipv4Address&&) = default;
    Ipv4Address& operator=(Ipv4Address&&) = default;

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
    const OptionalField<std::string>& get_address() const {
        return m_address;
    }

    /*!
     * Sets subnet mask
     *
     * @param[in] subnet_mask Subnet mask
     * */
    void set_subnet_mask(const OptionalField<std::string>& subnet_mask) {
        m_subnet_mask = subnet_mask;
    }

    /*!
     * Gets subnet mask.
     * */
    const OptionalField<std::string>& get_subnet_mask() const {
        return m_subnet_mask;
    }

    /*!
     * Sets address origin
     *
     * @param[in] address_origin Address origin
     * */
    void set_address_origin(const OptionalField<enums::Ipv4AddressOrigin>& address_origin) {
        m_address_origin = address_origin;
    }

    /*!
     * Gets IP address origin.
     * */
    const OptionalField<enums::Ipv4AddressOrigin>& get_address_origin() const {
        return m_address_origin;
    }

    /*!
     * Sets gateway
     *
     * @param[in] gateway Gateway.
     * */
    void set_gateway(const OptionalField<std::string>& gateway) {
        m_gateway = gateway;
    }

    /*!
     * Gets gateway.
     * */
    const OptionalField<std::string>& get_gateway() const {
        return m_gateway;
    }

    /*!
     * @brief Convert Ipv4Address to JSON object
     * @return JSON representation of Ipv4Address object
     * */
    json::Json to_json() const;

    /*!
     * @brief construct an object of class Ipv from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed Ipv object
     */
    static Ipv4Address from_json(const json::Json& json);

private:

    OptionalField<std::string> m_address{};
    OptionalField<std::string> m_subnet_mask{};
    OptionalField<enums::Ipv4AddressOrigin> m_address_origin{};
    OptionalField<std::string> m_gateway{};

};

}
}
}
