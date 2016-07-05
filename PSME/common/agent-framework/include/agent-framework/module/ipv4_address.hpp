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
 * @file ipv4_address.hpp
 * @brief IPv4 address
 * */
#ifndef AGENT_FRAMEWORK_MODULE_IPV4_ADDRESS_HPP
#define AGENT_FRAMEWORK_MODULE_IPV4_ADDRESS_HPP

#include <jsonrpccpp/server.h>

#include <string>

namespace agent_framework {
namespace generic {

/*! IPv4 address */
class IPv4Address {
    std::string m_address{};
    std::string m_subnet_mask{};
    std::string m_address_origin{};
    std::string m_gateway{};
public:
    /*!
     * @brief IPv4Address constructor
     *
     * @param[in] address IP address
     * @param[in] subnet_mask Subnet mask
     * @param[in] address_origin Address origin
     * @param[in] gateway Gateway
     * */
    IPv4Address(const std::string& address = {},
            const std::string& subnet_mask = {},
            const std::string& address_origin = {},
            const std::string& gateway = {}) :
            m_address(address),
            m_subnet_mask(subnet_mask),
            m_address_origin(address_origin),
            m_gateway(gateway) {}

    /*! Default assignement operator */
    IPv4Address& operator=(const IPv4Address&) = default;

    /* Default copy constructor */
    IPv4Address(const IPv4Address&) = default;

    /*!
     * Sets IP address
     *
     * @param[in] address IP address.
     * */
    void set_address(const std::string& address) {
        m_address = address;
    }

    /*!
     * Gets IP address.
     * */
    const std::string& get_address() const {
        return m_address;
    }

    /*!
     * Sets subnet mask
     *
     * @param[in] subnet_mask Subnet mask
     * */
    void set_subnet_mask(const std::string& subnet_mask) {
        m_subnet_mask = subnet_mask;
    }

    /*!
     * Gets subnet mask.
     * */
    const std::string& get_subnet_mask() const {
        return m_subnet_mask;
    }

    /*!
     * Sets address origin
     *
     * @param[in] address_origin Address origin
     * */
    void set_address_origin(const std::string& address_origin) {
        m_address_origin = address_origin;
    }

    /*!
     * Gets IP address origin.
     * */
    const std::string& get_address_origin() const {
        return m_address_origin;
    }

    /*!
     * Sets gateway
     *
     * @param[in] gateway Gateway.
     * */
    void set_gateway(const std::string& gateway) {
        m_gateway = gateway;
    }

    /*!
     * Gets gateway.
     * */
    const std::string& get_gateway() const {
        return m_gateway;
    }

    /*!
     * @brief Convert IPv4Address to JSON object
     * @return JSON representation of IPv4Address object
     * */
    Json::Value to_json() const;

    virtual ~IPv4Address();
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_IPV4_ADDRESS_HPP */
