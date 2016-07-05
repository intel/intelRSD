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
 * @file ipv6_address.hpp
 * @brief IPv6 address
 * */
#ifndef AGENT_FRAMEWORK_MODULE_IPV6_ADDRESS_HPP
#define AGENT_FRAMEWORK_MODULE_IPV6_ADDRESS_HPP

#include <jsonrpccpp/server.h>

#include <string>

namespace agent_framework {
namespace generic {

/*! IPv6 address */
class IPv6Address {
    std::string m_address{};
    std::uint32_t m_prefix_length{};
    std::string m_address_origin{};
    std::string m_address_state{};
public:
    /*!
     * @brief IPv6Address constructor
     *
     * @param[in] address IP address
     * @param[in] prefix_length Prefix length
     * @param[in] address_origin Address origin
     * @param[in] address_state Address state
     * */
    IPv6Address(const std::string& address = {},
                std::uint32_t prefix_length = {},
                const std::string& address_origin = {},
                const std::string& address_state = {}) :
            m_address(address),
            m_prefix_length(prefix_length),
            m_address_origin(address_origin),
            m_address_state(address_state) {}

    /*! Default assignement operator */
    IPv6Address& operator=(const IPv6Address&) = default;

    /* Default copy constructor */
    IPv6Address(const IPv6Address&) = default;

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
     * Sets prefix length
     *
     * @param[in] prefix_length Prefix length
     * */
    void set_prefix_length(std::uint32_t prefix_length) {
        m_prefix_length = prefix_length;
    }

    /*!
     * Gets prefix length.
     * */
    std::uint32_t get_prefix_length() const {
        return m_prefix_length;
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
     * Sets address state
     *
     * @param[in] address_state Address state.
     * */
    void set_address_state(const std::string& address_state) {
        m_address_state = address_state;
    }

    /*!
     * Gets address state.
     * */
    const std::string& get_address_state() const {
        return m_address_state;
    }

    /*!
     * @brief Convert IPv6Address to JSON object
     * @return JSON representation of IPv6Address object
     * */
    Json::Value to_json() const;

    virtual ~IPv6Address();
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_IPV6_ADDRESS_HPP */
