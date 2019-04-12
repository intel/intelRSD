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

package com.intel.rsd.nodecomposer.services.configuration.difference;

import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ConnectedEntityResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.EndpointResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.IdentifierObject;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.IpTransportDetailsResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.IpV4AddressObject;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.IpV6AddressObject;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.IpTransportDetails;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Identifier;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.IpV4Address;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.IpV6Address;
import com.intel.rsd.nodecomposer.types.Protocol;

import java.util.List;
import java.util.Set;

import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromUri;
import static com.intel.rsd.nodecomposer.types.Protocol.OEM;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
final class EndpointBuilder {
    private EndpointBuilder() {
    }

    static Endpoint asEndpoint(EndpointResource endpointResource) {
        requiresNonNull(endpointResource, "endpointResource");
        Endpoint endpoint = new Endpoint();
        endpoint.setUri(oDataIdFromUri(endpointResource.getUri()));
        mapConnectedEntities(endpointResource, endpoint);
        mapTransports(endpointResource.getTransportResourceList(), endpoint);
        mapIdentifiers(endpointResource.getIdentifiers(), endpoint);
        mapProtocol(endpointResource, endpoint);

        return endpoint;
    }

    private static void mapProtocol(EndpointResource endpointResource, Endpoint endpoint) {
        Protocol protocolFromResource = endpointResource.getProtocol();
        Protocol protocol = OEM.equals(protocolFromResource) ? endpointResource.getEndpointProtocolFromOem() : protocolFromResource;
        endpoint.setProtocol(protocol);
    }

    private static void mapConnectedEntities(EndpointResource endpointResource, Endpoint endpoint) {
        for (ConnectedEntityResource connectedEntityResource : endpointResource.getConnectedEntityResourceList()) {
            ConnectedEntity connectedEntity = new ConnectedEntity();
            connectedEntity.setEntityRole(connectedEntityResource.getEntityRole());
            endpoint.addConnectedEntity(connectedEntity);
        }
    }

    private static void mapTransports(List<IpTransportDetailsResource> ipTransportDetailsList, Endpoint endpoint) {
        for (IpTransportDetailsResource ipTransportDetails : ipTransportDetailsList) {
            IpTransportDetails transport = new IpTransportDetails();
            transport.setTransportProtocol(ipTransportDetails.getTransportProtocol());
            transport.setPort(ipTransportDetails.getPort());
            mapIpv4Address(transport, ipTransportDetails.getIpv4Address());
            mapIpv6Address(transport, ipTransportDetails.getIpv6Address());

            endpoint.addIpTransportDetails(transport);
        }
    }

    private static void mapIpv4Address(IpTransportDetails transport, IpV4AddressObject ipv4Address) {
        if (ipv4Address != null) {
            IpV4Address address = new IpV4Address();
            address.setAddress(ipv4Address.getAddress());
            address.setAddressOrigin(ipv4Address.getAddressOrigin());
            address.setGateway(ipv4Address.getGateway());
            address.setOem(ipv4Address.getOem());
            address.setSubnetMask(ipv4Address.getSubnetMask());
            if (!address.equals(new IpV4Address())) {
                transport.setIpV4Address(address);
            }
        }
    }

    private static void mapIpv6Address(IpTransportDetails transport, IpV6AddressObject ipv6Address) {
        if (ipv6Address != null) {
            IpV6Address address = new IpV6Address();
            address.setAddress(ipv6Address.getAddress());
            address.setAddressOrigin(ipv6Address.getAddressOrigin());
            address.setAddressState(ipv6Address.getAddressState());
            address.setOem(ipv6Address.getOem());
            address.setPrefixLength(ipv6Address.getPrefixLength());
            if (!address.equals(new IpV6Address())) {
                transport.setIpV6Address(address);
            }
        }
    }

    private static void mapIdentifiers(Set<IdentifierObject> identifiers, Endpoint endpoint) {
        for (IdentifierObject identifierObject : identifiers) {
            Identifier identifier = new Identifier();
            identifier.setDurableName(identifierObject.getDurableName());
            identifier.setDurableNameFormat(identifierObject.getDurableNameFormat());
            endpoint.addIdentifier(identifier);
        }
    }
}
