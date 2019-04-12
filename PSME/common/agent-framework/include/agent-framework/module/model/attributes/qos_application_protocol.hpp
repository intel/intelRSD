/*!
 * @brief Declaration of Network QoS Application Protocol class.
 *        Used as DCBX Shared Configuration.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file qos_application_protocol.hpp
 */

#pragma once
#include "agent-framework/module/utils/utils.hpp"
#include "agent-framework/module/enum/network.hpp"
#include "json-wrapper/json-wrapper.hpp"

namespace agent_framework {
namespace model {
namespace attribute {

/*! QoS DCBX Application Protocol */
class QosApplicationProtocol {
public:
    explicit QosApplicationProtocol();

    QosApplicationProtocol(const enums::TransportLayerProtocol protocol,
                           const std::uint32_t port,
                           const std::uint32_t priority):
        m_protocol(protocol),
        m_port(port),
        m_priority(priority)
    {}

    ~QosApplicationProtocol();

    /*! Enable copy */
    QosApplicationProtocol(const QosApplicationProtocol&) = default;
    QosApplicationProtocol& operator=(const QosApplicationProtocol&) = default;
    QosApplicationProtocol(QosApplicationProtocol&&) = default;
    QosApplicationProtocol& operator=(QosApplicationProtocol&&) = default;


    /*!
    * @brief Sets Priority
    * @param[in] priority Priority
    */
    void set_priority(const OptionalField<std::uint32_t>& priority) {
        m_priority = priority;
    }


    /*!
     * @brief Gets Priority
     * @return Priority
     */
    const OptionalField<std::uint32_t>& get_priority() const {
        return m_priority;
    }


    /*!
     * @brief Sets Protocol
     * @param[in] protocol Protocol
     */
    void set_protocol(const OptionalField<enums::TransportLayerProtocol>& protocol) {
        m_protocol = protocol;
    }


    /*!
     * @brief Gets Protocol
     * @return Protocol
     */
    const OptionalField<enums::TransportLayerProtocol>& get_protocol() const {
        return m_protocol;
    }


    /*!
     * @brief Sets Port
     * @param[in] port Port number
     */
    void set_port(const OptionalField<std::uint32_t>& port) {
        m_port = port;
    }


    /*!
     * @brief Gets Port
     * @return Port number
     */
    const OptionalField<std::uint32_t>& get_port() const {
        return m_port;
    }


    /*!
     * @brief Converts QosApplicationProtocol to JSON object
     * @return JSON representation of QosApplicationProtocol object
     */
    json::Json to_json() const;


    /*!
     * @brief Constructs an object of class QosApplicationProtocol from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed QosApplicationProtocol object
     */
    static QosApplicationProtocol from_json(const json::Json& json);

private:
    OptionalField<enums::TransportLayerProtocol> m_protocol{};
    OptionalField<std::uint32_t> m_port{};
    OptionalField<std::uint32_t> m_priority{};
};

}
}
}