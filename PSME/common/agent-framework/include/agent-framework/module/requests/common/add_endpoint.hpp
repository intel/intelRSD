/*!
 * @brief Definition of AddEndpoint class.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file add_endpoint.hpp
 */

#pragma once



#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/module/model/attributes/array.hpp"
#include "agent-framework/module/model/attributes/oem.hpp"
#include "agent-framework/module/model/attributes/ip_transport_detail.hpp"
#include "agent-framework/module/model/attributes/identifier.hpp"
#include "agent-framework/module/model/attributes/connected_entity.hpp"
#include "agent-framework/module/utils/optional_field.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module/constants/regular_expressions.hpp"

#include <string>



namespace agent_framework {
namespace model {
namespace requests {

/*! AddEndpoint request */
class AddEndpoint {
public:

    using IpTransportDetails = agent_framework::model::attribute::Array<agent_framework::model::attribute::IpTransportDetail>;
    using Identifiers = agent_framework::model::attribute::Array<agent_framework::model::attribute::Identifier>;
    using ConnectedEntities = agent_framework::model::attribute::Array<agent_framework::model::attribute::ConnectedEntity>;
    using Ports = agent_framework::model::attribute::Array<Uuid>;


    explicit AddEndpoint(const Uuid& fabric,
                         const IpTransportDetails& transports,
                         const Identifiers& identifiers,
                         const ConnectedEntities& connected_entities,
                         const OptionalField<std::string>& username,
                         const OptionalField<std::string>& password,
                         const Ports& ports,
                         const attribute::Oem& oem);


    static std::string get_command() {
        return literals::Command::ADD_ENDPOINT;
    }


    /*!
     * @brief Get IP transport details.
     *
     * @return IP transport details
     */
    const IpTransportDetails& get_ip_transport_details() const {
        return m_ip_transport_details;
    }


    /*!
     * @brief Get identifiers.
     *
     * @return identifiers
     */
    const Identifiers& get_identifiers() const {
        return m_identifiers;
    }


    /*!
     * @brief Get fabric Uuid
     *
     * @return fabric Uuid
     */
    const Uuid& get_fabric() const {
        return m_fabric;
    }


    /*!
     * @brief Get ports Uuids
     *
     * @return Array of ports Uuids
     */
    const Ports& get_ports() const {
        return m_ports;
    }


    /*!
     * @brief Get connected entities.
     *
     * @return connected entities.
     */
    const ConnectedEntities& get_connected_entities() const {
        return m_connected_entities;
    }


    /*!
     * @brief Get username.
     *
     * @return username
     */
    const OptionalField<std::string>& get_username() const {
        return m_username;
    }


    /*!
     * @brief Get password.
     *
     * @return password
     */
    const OptionalField<std::string>& get_password() const {
        return m_password;
    }


    /*!
     * @brief Get oem from request
     *
     * @return oem
     */
    const attribute::Oem& get_oem() const {
        return m_oem;
    }


    /*!
     * @brief Transform request to Json
     * @return created Json value
     */
    json::Json to_json() const;


    /*!
     * @brief create AddEndpoint from Json
     * @param[in] json the input argument
     * @return new AddEndpoint
     */
    static AddEndpoint from_json(const json::Json& json);


    /*!
     * @brief Returns procedure scheme
     * @return ProcedureValidator scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_OBJECT,
            literals::Endpoint::FABRIC, VALID_UUID,
            literals::Endpoint::IP_TRANSPORT_DETAILS, VALID_ARRAY_OF(VALID_ATTRIBUTE(IpTransportDetailSchema)),
            literals::Endpoint::IDENTIFIERS, VALID_ARRAY_OF(VALID_ATTRIBUTE(IdentifierSchema)),
            literals::Endpoint::CONNECTED_ENTITIES, VALID_ARRAY_OF(VALID_ATTRIBUTE(ConnectedEntitySchema)),
            literals::Endpoint::USERNAME, VALID_NULLABLE(VALID_JSON_STRING),
            literals::Endpoint::PASSWORD, VALID_NULLABLE(VALID_JSON_STRING),
            literals::Endpoint::PORTS, VALID_OPTIONAL(VALID_NULLABLE(VALID_ARRAY_OF(VALID_UUID))),
            literals::Endpoint::OEM, VALID_OPTIONAL(jsonrpc::JSON_OBJECT),
            nullptr
        };
        return procedure;
    }


private:
    class IpTransportDetailSchema {
    public:
        static const jsonrpc::ProcedureValidator& get_procedure() {
            static const jsonrpc::ProcedureValidator procedure{
                get_command(),
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                literals::IpTransportDetail::PROTOCOL, VALID_NULLABLE(VALID_ENUM(enums::TransportProtocol)),
                literals::IpTransportDetail::IPV4_ADDRESS, VALID_NULLABLE(VALID_ATTRIBUTE(IPv4AddressSchema)),
                literals::IpTransportDetail::IPV6_ADDRESS, VALID_NULLABLE(VALID_ATTRIBUTE(IPv6AddressSchema)),
                literals::IpTransportDetail::PORT, VALID_NULLABLE(VALID_NUMERIC_TYPED(UINT32)),
                literals::IpTransportDetail::INTERFACE, VALID_NULLABLE(VALID_UUID),
                nullptr
            };
            return procedure;
        }
    };

    class IPv4AddressSchema {
    public:
        static const jsonrpc::ProcedureValidator& get_procedure() {
            static const jsonrpc::ProcedureValidator procedure{
                get_command(),
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                literals::Ipv4Address::ADDRESS, VALID_REGEX(literals::regex::IPAddresses::ADDRESS),
                literals::Ipv4Address::SUBNET_MASK, VALID_NULLABLE(VALID_REGEX(literals::regex::IPAddresses::ADDRESS)),
                literals::Ipv4Address::ADDRESS_ORIGIN, VALID_NULLABLE(VALID_ENUM(enums::Ipv4AddressOrigin)),
                literals::Ipv4Address::GATEWAY, VALID_NULLABLE(VALID_REGEX(literals::regex::IPAddresses::ADDRESS)),
                nullptr
            };
            return procedure;
        }
    };

    class IPv6AddressSchema {
    public:
        static const jsonrpc::ProcedureValidator& get_procedure() {
            static const jsonrpc::ProcedureValidator procedure{
                get_command(),
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                literals::Ipv6Address::ADDRESS, VALID_NULLABLE(VALID_REGEX(literals::regex::IPv6Addresses::ADDRESS)),
                literals::Ipv6Address::PREFIX_LENGTH, VALID_NULLABLE(VALID_NUMERIC_TYPED(UINT32)),
                literals::Ipv6Address::ADDRESS_ORIGIN, VALID_NULLABLE(VALID_ENUM(enums::Ipv6AddressOrigin)),
                literals::Ipv6Address::ADDRESS_STATE, VALID_NULLABLE(VALID_ENUM(enums::Ipv6AddressState)),
                nullptr
            };
            return procedure;
        }
    };

    class IdentifierSchema {
    public:
        static const jsonrpc::ProcedureValidator& get_procedure() {
            static const jsonrpc::ProcedureValidator procedure{
                get_command(),
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                literals::Identifier::DURABLE_NAME, VALID_REGEX(literals::regex::Common::EMPTY_OR_NO_WHITESPACE_STRING),
                literals::Identifier::DURABLE_NAME_FORMAT, VALID_ENUM(enums::IdentifierType),
                nullptr
            };
            return procedure;
        }
    };

    class ConnectedEntitySchema {
    public:
        static const jsonrpc::ProcedureValidator& get_procedure() {
            static const jsonrpc::ProcedureValidator procedure{
                get_command(),
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                literals::ConnectedEntity::ROLE, VALID_ENUM(enums::EntityRole),
                literals::ConnectedEntity::ENTITY, VALID_NULLABLE(VALID_UUID),
                literals::ConnectedEntity::IDENTIFIERS, VALID_ARRAY_OF(VALID_ATTRIBUTE(IdentifierSchema)),
                literals::ConnectedEntity::LUN, VALID_NULLABLE(VALID_NUMERIC_TYPED(INT64)),
                nullptr
            };
            return procedure;
        }
    };

    Uuid m_fabric{};
    Ports m_ports{};
    IpTransportDetails m_ip_transport_details{};
    Identifiers m_identifiers{};
    ConnectedEntities m_connected_entities{};
    OptionalField<std::string> m_username{};
    OptionalField<std::string> m_password{};
    attribute::Oem m_oem{};
};

}
}
}
