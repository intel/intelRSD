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
 * @file next_hop.hpp
 *
 * @brief Next hop
 * */
#ifndef AGENT_FRAMEWORK_MODULE_MODEL_ATTRIBUTES_NEXT_HOP_HPP
#define AGENT_FRAMEWORK_MODULE_MODEL_ATTRIBUTES_NEXT_HOP_HPP

#include "agent-framework/module-ref/types/common.hpp"
#include <json/json.h>

namespace agent_framework {
namespace model {
namespace attribute {

/*! Next hop class */
class NextHop {
public:

    using String = types::String;
    using Number = types::Number;

    explicit NextHop();

    NextHop(const Number& metric,
            const String& port_identifier,
            const String& ipv4_address,
            const String& ipv6_address):
            m_metric{metric},
            m_port_identifier{port_identifier},
            m_ipv4_address{ipv4_address},
            m_ipv6_address{ipv6_address}
            {}

    ~NextHop();

    /*! Enable copy */
    NextHop(const NextHop&) = default;
    NextHop& operator=(const NextHop&) = default;

    /*! Enable move */
    NextHop(NextHop&&) = default;
    NextHop& operator=(NextHop&&) = default;

    /*!
     * Sets metric.
     *
     * @param[in] metric Metric.
     * */
    void set_metric(Number metric) {
        m_metric = metric;
    }

    /*!
     * Gets metric
     * */
    Number get_metric() const {
        return m_metric;
    }

    /*!
     * Sets port identifier
     *
     * @param[in] port_identifier Port identifier
     * */
    void set_port_identifier(const String& port_identifier) {
            m_port_identifier = port_identifier;
        }

    /*!
     * Gets port identifier
     * */
    const String& get_port_identifier() const {
        return m_port_identifier;
    }

    /*!
     * Sets IPv4 address
     *
     * @param[in] ipv4_address IPv4 address
     * */
    void set_ipv4_address(const String& ipv4_address) {
        m_ipv4_address = ipv4_address;
    }

    /*!
     * Gets IPv4 address
     * */
    const String& get_ipv4_address() const {
        return m_ipv4_address;
    }

    /*!
     * Sets IPv6 address
     *
     * @param[in] ipv6_address IPv4 address
     * */
    void set_ipv6_address(const String& ipv6_address) {
        m_ipv6_address = ipv6_address;
    }

    /*!
      * Gets IPv6 address
     * */
    const String& get_ipv6_address() const {
        return m_ipv6_address;
    }

  /*!
     * Converts this to json representation.
     *
     * //return json representation of this class.
     * */
    Json::Value to_json() const;

    /*!
     * Converts Json to a NextHop object
     * */
    static NextHop from_json(const Json::Value& json);


private:
    Number m_metric{};
    String m_port_identifier{};
    String m_ipv4_address{};
    String m_ipv6_address{};
};

}
}
}

#endif /* AGENT_FRAMEWORK_MODULE_MODEL_ATTRIBUTES_NEXT_HOP_HPP */
