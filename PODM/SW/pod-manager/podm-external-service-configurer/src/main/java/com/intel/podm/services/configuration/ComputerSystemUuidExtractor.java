/*
 * Copyright (c) 2018 Intel Corporation
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

package com.intel.podm.services.configuration;

import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.embeddables.Identifier;
import com.intel.podm.client.resources.redfish.EndpointResource;
import com.intel.podm.client.resources.redfish.IdentifierObject;

import javax.enterprise.context.ApplicationScoped;
import java.util.UUID;

import static com.intel.podm.common.types.DurableNameFormat.IQN;
import static com.intel.podm.common.types.DurableNameFormat.NQN;
import static com.intel.podm.common.types.EntityRole.INITIATOR;
import static com.intel.podm.common.utils.IterableHelper.single;
import static java.util.stream.Collectors.toSet;
import static org.apache.commons.lang3.StringUtils.substringAfterLast;

@ApplicationScoped
public class ComputerSystemUuidExtractor {
    public static final String NQN_PREFIX = "nqn.2014-08.org.nvmexpress:uuid";
    public static final String IQN_PREFIX = "iqn.1986-03.com.intel";

    public UUID extractFromInitiatorEndpointResource(EndpointResource initiatorEndpoint) {
        if (!isInitiator(initiatorEndpoint)) {
            throw new IllegalArgumentException("UUID can be extracted only from Initiator endpoints");
        }
        IdentifierObject identifier = single(initiatorEndpoint.getIdentifiers().stream()
            .filter(identifierObject -> NQN.equals(identifierObject.getDurableNameFormat()) || IQN.equals(identifierObject.getDurableNameFormat()))
            .collect(toSet()));

        return UUID.fromString(getUuidFromDurableName(identifier.getDurableName()));
    }

    public UUID extractFromInitiatorEndpoint(Endpoint initiatorEndpoint) {
        if (!initiatorEndpoint.hasRole(INITIATOR)) {
            throw new IllegalArgumentException("UUID can be extracted only from Initiator endpoints");
        }

        Identifier endpointIdentifier = single(initiatorEndpoint.getIdentifiers().stream()
            .filter(identifier -> NQN.equals(identifier.getDurableNameFormat()) || IQN.equals(identifier.getDurableNameFormat()))
            .collect(toSet()));

        return UUID.fromString(getUuidFromDurableName(endpointIdentifier.getDurableName()));
    }

    private boolean isInitiator(EndpointResource endpoint) {
        return endpoint.getConnectedEntityResourceList().stream()
            .anyMatch(connectedEntity -> INITIATOR.equals(connectedEntity.getEntityRole()));
    }

    private String getUuidFromDurableName(String durableName) {
        return substringAfterLast(durableName, ":");
    }
}
