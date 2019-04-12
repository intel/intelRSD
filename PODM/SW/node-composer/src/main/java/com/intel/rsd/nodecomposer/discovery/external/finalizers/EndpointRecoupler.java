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

package com.intel.rsd.nodecomposer.discovery.external.finalizers;

import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.net.URI;
import java.util.Collection;
import java.util.Set;

import static com.intel.rsd.nodecomposer.persistence.redfish.base.StatusControl.statusOf;
import static java.lang.String.format;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
class EndpointRecoupler extends Recoupler<Endpoint> {
    @Override
    protected void reattach(ComposedNode node, Collection<Endpoint> assets) {
        val associatedEndpointsUris = node.getAssociatedEndpointsUris();

        val endpointsToReattach = assets.stream()
            .filter(endpoint -> associatedEndpointsUris.contains(endpoint.getUri().toUri()))
            .collect(toList());

        if (!associatedEndpointsUris.isEmpty() && endpointsToReattach.isEmpty()) {
            log.warn(format("Endpoints with URIs %s associated with composed node were not found.", associatedEndpointsUris));
            return;
        }

        for (Endpoint endpoint : endpointsToReattach) {
            node.attachAsset(endpoint);
        }
    }

    @Override
    protected boolean verify(ComposedNode node) {
        return !composedNodeContainsAssociatedEndpoints(node) || composedNodeEndpointsCollectionIsConsistent(node);
    }

    private boolean composedNodeEndpointsCollectionIsConsistent(ComposedNode node) {
        return !node.getEndpoints().isEmpty()
            && allEndpointsAreConnectedToNode(node)
            && allEndpointsAreHealthy(node);
    }

    private boolean allEndpointsAreConnectedToNode(ComposedNode node) {
        return node.getAssociatedEndpointsUris().size() == node.getEndpoints().size()
            && composedNodeContainsAllAssociatedEndpoints(node);
    }

    private boolean composedNodeContainsAllAssociatedEndpoints(ComposedNode node) {
        Set<URI> composedNodeEndpointsUris = node.getEndpoints().stream()
            .filter(Endpoint::isAchievable)
            .map(endpoint -> endpoint.getUri().toUri())
            .collect(toSet());

        return composedNodeEndpointsUris.containsAll(node.getAssociatedEndpointsUris());
    }

    private boolean allEndpointsAreHealthy(ComposedNode node) {
        return node.getEndpoints().stream().allMatch(endpoint -> statusOf(endpoint).isEnabled().isHealthy().verify());
    }

    private boolean composedNodeContainsAssociatedEndpoints(ComposedNode node) {
        return !node.getAssociatedEndpointsUris().isEmpty();
    }
}
