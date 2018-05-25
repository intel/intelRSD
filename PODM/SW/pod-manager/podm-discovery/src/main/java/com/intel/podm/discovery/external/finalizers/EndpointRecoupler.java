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

package com.intel.podm.discovery.external.finalizers;

import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.embeddables.Identifier;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.Objects;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.business.entities.redfish.base.StatusControl.statusOf;
import static java.lang.String.format;

@Dependent
class EndpointRecoupler extends Recoupler<Endpoint> {
    @Inject
    private Logger logger;

    @Override
    protected void reattach(ComposedNode node, Collection<Endpoint> assets) {
        UUID associatedStorageServiceUuid = node.getAssociatedStorageServiceUuid();
        Set<Identifier> associatedEndpointDurableName = node.getAssociatedEndpointIdentifiers();

        Endpoint endpointToReattach = assets.stream()
            .filter(endpoint -> endpoint.getService().getUuid() != null)
            .filter(endpoint -> Objects.equals(endpoint.getService().getUuid(), associatedStorageServiceUuid))
            .filter(endpoint -> associatedEndpointDurableName.containsAll(endpoint.getIdentifiers()))
            .findFirst()
            .orElse(null);

        if (endpointToReattach != null) {
            endpointToReattach.getMetadata().setAllocated(true);
            node.addEndpoint(endpointToReattach);
        } else {
            logger.w(format("Endpoint with id %s associated with composed node has not been found, on service with UUID: %s",
                associatedEndpointDurableName, associatedStorageServiceUuid));
        }
    }

    @Override
    protected boolean verify(ComposedNode node) {
        return !composedNodeContainsAssociatedEndpoints(node) || composedNodeContainsEnabledEndpoints(node);
    }

    private boolean composedNodeContainsEnabledEndpoints(ComposedNode node) {
        return !node.getEndpoints().isEmpty()
            && node.getEndpoints().stream().allMatch(endpoint -> statusOf(endpoint).isEnabled().isHealthy().verify());
    }

    private boolean composedNodeContainsAssociatedEndpoints(ComposedNode node) {
        return node.getAssociatedStorageServiceUuid() != null
            && !node.getAssociatedEndpointIdentifiers().isEmpty();
    }
}
