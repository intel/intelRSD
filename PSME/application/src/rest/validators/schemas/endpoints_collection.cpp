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
 * */

#include "psme/rest/validators/schemas/endpoints_collection.hpp"
#include "psme/rest/validators/schemas/common.hpp"
#include "psme/rest/constants/constants.hpp"

#include "agent-framework/module/constants/regular_expressions.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/pnc.hpp"



using namespace psme::rest;
using namespace psme::rest::validators::schema;
using namespace agent_framework::model;


const jsonrpc::ProcedureValidator& EndpointsCollectionPostSchema::EntitySchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Endpoint::ENTITY_LINK, VALID_OPTIONAL(VALID_NULLABLE(VALID_ATTRIBUTE(SimpleObjectSchema))),
        constants::Endpoint::ENTITY_ROLE, VALID_ENUM(enums::EntityRole),
        constants::Common::IDENTIFIERS,
        VALID_OPTIONAL(VALID_NULLABLE(VALID_ARRAY_OF(VALID_ATTRIBUTE(IdentifierSchema)))),
        constants::Common::OEM, VALID_OPTIONAL(VALID_ATTRIBUTE(OemSchema)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& EndpointsCollectionPostSchema::EntitySchema::OemSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::RACKSCALE, VALID_OPTIONAL(VALID_ATTRIBUTE(RackScaleSchema)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator&
EndpointsCollectionPostSchema::EntitySchema::OemSchema::RackScaleSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::LUN, VALID_OPTIONAL(VALID_NUMERIC_TYPED(INT64)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& EndpointsCollectionPostSchema::OemSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::RACKSCALE, VALID_OPTIONAL(VALID_ATTRIBUTE(RackScaleSchema)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& EndpointsCollectionPostSchema::OemSchema::RackScaleSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Endpoint::AUTHENTICATION, VALID_OPTIONAL(VALID_ATTRIBUTE(AuthSchema)),
        constants::Endpoint::ENDPOINT_PROTOCOL, VALID_OPTIONAL(VALID_NULLABLE(VALID_ENUM(enums::OemProtocol))),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& EndpointsCollectionPostSchema::IpTransportDetailSchema::IPv6Schema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::IpAddress::ADDRESS, VALID_REGEX(literals::regex::IPv6Addresses::ADDRESS),
        constants::IpAddress::ADDRESS_ORIGIN, VALID_OPTIONAL(VALID_ENUM(enums::Ipv6AddressOrigin)),
        constants::IpAddress::PREFIX_LENGTH, VALID_OPTIONAL(VALID_NUMERIC_TYPED(INT32)),
        constants::IpAddress::ADDRESS_STATE, VALID_OPTIONAL(VALID_ENUM(enums::Ipv6AddressState)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& EndpointsCollectionPostSchema::IpTransportDetailSchema::IPv4Schema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::IpAddress::ADDRESS, VALID_REGEX(literals::regex::IPAddresses::ADDRESS),
        constants::IpAddress::ADDRESS_ORIGIN, VALID_OPTIONAL(VALID_ENUM(enums::Ipv4AddressOrigin)),
        constants::IpAddress::GATEWAY, VALID_OPTIONAL(VALID_REGEX(literals::regex::IPAddresses::GATEWAY)),
        constants::IpAddress::SUBNET_MASK, VALID_OPTIONAL(VALID_REGEX(literals::regex::IPAddresses::SUBNET_MASK)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& EndpointsCollectionPostSchema::IpTransportDetailSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Endpoint::IPV4_ADDRESS, VALID_OPTIONAL(VALID_ATTRIBUTE(IPv4Schema)),
        constants::Endpoint::IPV6_ADDRESS, VALID_OPTIONAL(VALID_ATTRIBUTE(IPv6Schema)),
        constants::Endpoint::PORT, VALID_OPTIONAL(VALID_NUMERIC_TYPED(INT32)),
        constants::Endpoint::TRANSPORT_PROTOCOL, VALID_ENUM(enums::TransportProtocol),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator&
EndpointsCollectionPostSchema::OemSchema::RackScaleSchema::AuthSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Endpoint::USERNAME,
        VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(literals::regex::Common::NO_WHITESPACE_STRING))),
        constants::Endpoint::PASSWORD,
        VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(literals::regex::Common::NO_WHITESPACE_STRING))),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& EndpointsCollectionPostSchema::LinksSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::OEM, VALID_OPTIONAL(VALID_ATTRIBUTE(OemSchema)),
        constants::Endpoint::PORTS, VALID_OPTIONAL(VALID_NULLABLE(VALID_ARRAY_OF(VALID_ATTRIBUTE(SimpleObjectSchema)))),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& EndpointsCollectionPostSchema::LinksSchema::OemSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::RACKSCALE, VALID_OPTIONAL(VALID_ATTRIBUTE(RackScaleSchema)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator&
EndpointsCollectionPostSchema::LinksSchema::OemSchema::RackScaleSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Endpoint::INTERFACES,
        VALID_OPTIONAL(VALID_NULLABLE(VALID_ARRAY_OF(VALID_NULLABLE(VALID_ATTRIBUTE(SimpleObjectSchema))))),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& EndpointsCollectionPostSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Endpoint::ENDPOINT_PROTOCOL, VALID_OPTIONAL(VALID_ENUM(enums::TransportProtocol)),
        constants::Common::IDENTIFIERS,
        VALID_OPTIONAL(VALID_NULLABLE(VALID_ARRAY_OF(VALID_ATTRIBUTE(IdentifierSchema)))),
        constants::Endpoint::CONNECTED_ENTITIES, VALID_ARRAY_OF(VALID_ATTRIBUTE(EntitySchema)),
        constants::Endpoint::IP_TRANSPORT_DETAILS,
        VALID_OPTIONAL(VALID_NULLABLE(VALID_ARRAY_OF(VALID_ATTRIBUTE(IpTransportDetailSchema)))),
        constants::Common::LINKS, VALID_OPTIONAL(VALID_ATTRIBUTE(LinksSchema)),
        constants::Common::OEM, VALID_OPTIONAL(VALID_ATTRIBUTE(OemSchema)),
        nullptr
    };
    return procedure;
}
