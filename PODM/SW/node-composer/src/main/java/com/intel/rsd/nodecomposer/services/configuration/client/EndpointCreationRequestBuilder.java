/*
 * Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.intel.rsd.nodecomposer.services.configuration.client;

import com.intel.rsd.nodecomposer.externalservices.actions.EndpointCreationRequest;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.IpTransportDetails;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Identifier;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.IpV4Address;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.IpV6Address;
import com.intel.rsd.nodecomposer.types.DurableNameFormat;
import com.intel.rsd.nodecomposer.types.Protocol;

import java.util.Collection;
import java.util.Set;

import static com.intel.rsd.collections.IterableHelper.any;
import static com.intel.rsd.nodecomposer.types.DurableNameFormat.findByProtocol;
import static java.util.stream.Collectors.toSet;

@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public final class EndpointCreationRequestBuilder {
    private EndpointCreationRequestBuilder() {
    }

    public static EndpointCreationRequest toEndpointCreationRequest(Endpoint endpoint) {
        EndpointCreationRequest request = new EndpointCreationRequest();
        EndpointCreationRequest.ConnectedEntity connectedEntity = new EndpointCreationRequest.ConnectedEntity();
        // All connected entities within one Endpoint should have identical role.
        ConnectedEntity conn = any(endpoint.getConnectedEntities());
        connectedEntity.setEntityRole(conn.getEntityRole());
        request.addConnectedEntity(connectedEntity);
        request.setIdentifiers(mapIdentifiersByProtocol(endpoint.getProtocol(), endpoint.getIdentifiers()));
        request.setIpTransportDetails(mapTransports(endpoint.getIpTransportDetails()));

        return request;
    }

    private static Collection<EndpointCreationRequest.IpTransportDetails> mapTransports(Set<IpTransportDetails> transports) {
        return transports.stream().map(EndpointCreationRequestBuilder::mapTransport).collect(toSet());
    }

    private static EndpointCreationRequest.IpTransportDetails mapTransport(IpTransportDetails transport) {
        EndpointCreationRequest.IpTransportDetails mappedTransport = new EndpointCreationRequest.IpTransportDetails();
        mappedTransport.setTransportProtocol(transport.getTransportProtocol());
        mappedTransport.setPort(transport.getPort());
        if (transport.getIpV4Address() != null) {
            mappedTransport.setIpv4Address(mapIp4Address(transport.getIpV4Address()));
        }
        if (transport.getIpV6Address() != null) {
            mappedTransport.setIpv6Address(mapIp6Address(transport.getIpV6Address()));
        }

        return mappedTransport;
    }

    private static EndpointCreationRequest.IpV4AddressObject mapIp4Address(IpV4Address ipv4Address) {
        EndpointCreationRequest.IpV4AddressObject mappedIpV4Address = new EndpointCreationRequest.IpV4AddressObject();
        mappedIpV4Address.setAddress(ipv4Address.getAddress());
        mappedIpV4Address.setAddressOrigin(ipv4Address.getAddressOrigin());
        mappedIpV4Address.setGateway(ipv4Address.getGateway());
        mappedIpV4Address.setSubnetMask(ipv4Address.getSubnetMask());

        return mappedIpV4Address;
    }

    private static EndpointCreationRequest.IpV6AddressObject mapIp6Address(IpV6Address ipv6Address) {
        EndpointCreationRequest.IpV6AddressObject mappedIp6Address = new EndpointCreationRequest.IpV6AddressObject();
        mappedIp6Address.setAddress(ipv6Address.getAddress());
        mappedIp6Address.setAddressOrigin(ipv6Address.getAddressOrigin());
        mappedIp6Address.setAddressState(ipv6Address.getAddressState());
        mappedIp6Address.setPrefixLength(ipv6Address.getPrefixLength());

        return mappedIp6Address;
    }

    private static Set<EndpointCreationRequest.Identifier> mapIdentifiersByProtocol(Protocol protocol, Set<Identifier> identifiers) {
        DurableNameFormat expectedDurableNameForEndpoint = getDurableNameFormat(protocol);
        return identifiers.stream()
            .filter(identifierObject -> expectedDurableNameForEndpoint.equals(identifierObject.getDurableNameFormat()))
            .map(EndpointCreationRequestBuilder::mapIdentifier)
            .collect(toSet());
    }

    private static DurableNameFormat getDurableNameFormat(Protocol fabricType) {
        return findByProtocol(fabricType)
            .orElseThrow(() -> new RuntimeException("Cannot determine durable name format for fabric type: " + fabricType));
    }

    private static EndpointCreationRequest.Identifier mapIdentifier(Identifier identifier) {
        EndpointCreationRequest.Identifier mappedIdentifier = new EndpointCreationRequest.Identifier();
        mappedIdentifier.setDurableName(identifier.getDurableName());
        mappedIdentifier.setDurableNameFormat(identifier.getDurableNameFormat());

        return mappedIdentifier;
    }
}
