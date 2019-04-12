/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.services.configuration;

import com.intel.rsd.nodecomposer.externalservices.resources.redfish.EndpointResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.IdentifierObject;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Identifier;
import com.intel.rsd.nodecomposer.types.DurableNameFormat;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.List;
import java.util.UUID;

import static com.intel.rsd.nodecomposer.types.DurableNameFormat.IQN;
import static com.intel.rsd.nodecomposer.types.DurableNameFormat.NQN;
import static com.intel.rsd.nodecomposer.types.EntityRole.INITIATOR;
import static com.intel.rsd.nodecomposer.types.Protocol.FPGA_OVER_FABRICS;
import static com.intel.rsd.nodecomposer.utils.Collector.toSingle;
import static java.util.Arrays.asList;
import static org.apache.commons.lang3.StringUtils.substringAfterLast;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class InitiatorEndpointUuidExtractor {
    private static final List<DurableNameFormat> SUPPORTED_QUALIFIED_NAME_FORMATS = asList(NQN, IQN);

    public UUID extractFromInitiatorEndpointResource(EndpointResource initiatorEndpoint) {
        if (!isInitiator(initiatorEndpoint)) {
            throw new IllegalArgumentException("UUID can be extracted only from Initiator endpoints");
        }

        if (FPGA_OVER_FABRICS.equals(initiatorEndpoint.getEndpointProtocolFromOem())) {
            return getUuidFromFpgaEndpointResource(initiatorEndpoint);
        }

        IdentifierObject identifier = initiatorEndpoint.getIdentifiers().stream()
            .filter(identifierObject -> SUPPORTED_QUALIFIED_NAME_FORMATS.contains(identifierObject.getDurableNameFormat()))
            .collect(toSingle());

        return UUID.fromString(getUuidFromDurableName(identifier.getDurableName()));
    }

    public UUID extractFromInitiatorEndpoint(Endpoint initiatorEndpoint) {
        if (!initiatorEndpoint.hasRole(INITIATOR)) {
            throw new IllegalArgumentException("UUID can be extracted only from Initiator endpoints");
        }

        if (FPGA_OVER_FABRICS.equals(initiatorEndpoint.getProtocol())) {
            return getUuidFromFpgaEndpoint(initiatorEndpoint);
        }

        Identifier endpointIdentifier = initiatorEndpoint.getIdentifiers().stream()
            .filter(identifier -> SUPPORTED_QUALIFIED_NAME_FORMATS.contains(identifier.getDurableNameFormat()))
            .collect(toSingle());

        return UUID.fromString(getUuidFromDurableName(endpointIdentifier.getDurableName()));
    }

    private UUID getUuidFromFpgaEndpoint(Endpoint initiatorEndpoint) {
        Identifier endpointIdentifier = initiatorEndpoint.getIdentifiers().stream()
            .filter(identifier -> DurableNameFormat.UUID.equals(identifier.getDurableNameFormat()))
            .collect(toSingle());

        return UUID.fromString(endpointIdentifier.getDurableName());
    }

    private UUID getUuidFromFpgaEndpointResource(EndpointResource initiatorEndpoint) {
        IdentifierObject endpointIdentifier = initiatorEndpoint.getIdentifiers().stream()
            .filter(identifier -> DurableNameFormat.UUID.equals(identifier.getDurableNameFormat()))
            .collect(toSingle());

        return UUID.fromString(endpointIdentifier.getDurableName());
    }

    private boolean isInitiator(EndpointResource endpoint) {
        return endpoint.getConnectedEntityResourceList().stream()
            .anyMatch(connectedEntity -> INITIATOR.equals(connectedEntity.getEntityRole()));
    }

    private String getUuidFromDurableName(String durableName) {
        return substringAfterLast(durableName, ":");
    }
}
