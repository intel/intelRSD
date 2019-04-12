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
 * @file transport_detail.hpp
 * @brief IpTransportDetail
 * */

#pragma once



#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/pnc.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "agent-framework/module/model/attributes/ipv4_address.hpp"
#include "agent-framework/module/model/attributes/ipv6_address.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>



namespace agent_framework {
namespace model {
namespace attribute {

/*! IpTransportDetail class */
class IpTransportDetail {
public:

    /*! Default constructor */
    explicit IpTransportDetail() {}


    IpTransportDetail(IpTransportDetail&&) = default;


    IpTransportDetail(const IpTransportDetail&) = default;


    IpTransportDetail& operator=(IpTransportDetail&&) = default;


    IpTransportDetail& operator=(const IpTransportDetail&) = default;


    ~IpTransportDetail();


    /*!
     * @brief Gets IPv4 address
     * @return IPv4 address
     * */
    const Ipv4Address& get_ipv4_address() const {
        return m_ipv4_address;
    }


    /*!
     * @brief Sets IPv4 address
     * @param[in] ipv4_address of type std::string
     */
    void set_ipv4_address(const Ipv4Address& ipv4_address) {
        m_ipv4_address = ipv4_address;
    }


    /*!
     * @brief Gets IPv6 address
     * @return IPv6 address
     * */
    const Ipv6Address& get_ipv6_address() const {
        return m_ipv6_address;
    }


    /*!
     * @brief Sets IPv6 address
     * @param[in] ipv6_address of type std::string
     */
    void set_ipv6_address(const Ipv6Address& ipv6_address) {
        m_ipv6_address = ipv6_address;
    }


    /*!
     * @brief Gets port
     * @return Port
     * */
    const OptionalField<std::uint32_t>& get_port() const {
        return m_port;
    }


    /*!
     * @brief Sets port
     * @param[in] port of type uint32_t
     */
    void set_port(const OptionalField<std::uint32_t>& port) {
        m_port = port;
    }


    /*!
     * @brief Gets interface
     * @return interface which is used for transport
     * */
    const OptionalField<Uuid>& get_interface() const {
        return m_interface;
    }


    /*!
     * @brief Sets interface
     * @param[in] interface which is used for transport
     */
    void set_interface(const OptionalField<Uuid>& interface) {
        m_interface = interface;
    }


    /*!
     * @brief Gets transport protocol
     * @return Transport protocol
     * */
    const OptionalField<enums::TransportProtocol>& get_transport_protocol() const {
        return m_transport_protocol;
    }


    /*!
     * @brief Sets transport protocol
     * @param[in] transport_protocol Transport_protocol
     * */
    void set_transport_protocol(const OptionalField<enums::TransportProtocol>& transport_protocol) {
        m_transport_protocol = transport_protocol;
    }


    /*!
     * @brief Converts this object to json representation
     * @return Json representation of this object
     * */
    json::Json to_json() const;


    /*!
     * @brief construct an object of class IpTransportDetail from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed IpTransportDetail object
     */
    static IpTransportDetail from_json(const json::Json& json);


private:
    Ipv4Address m_ipv4_address{};
    Ipv6Address m_ipv6_address{};
    OptionalField<enums::TransportProtocol> m_transport_protocol{};
    OptionalField<Uuid> m_interface{};
    OptionalField<std::uint32_t> m_port{};
};

}
}
}
