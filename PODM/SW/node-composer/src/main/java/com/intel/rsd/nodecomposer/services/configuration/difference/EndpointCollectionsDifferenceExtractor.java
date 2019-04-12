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

import com.intel.rsd.nodecomposer.externalservices.resources.redfish.EndpointResource;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import lombok.extern.slf4j.Slf4j;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import static java.util.stream.Collectors.joining;
import static java.util.stream.Collectors.toList;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class EndpointCollectionsDifferenceExtractor {
    private static final EndpointEquator EQUATOR = new EndpointEquator();

    public List<Endpoint> getEndpointsToRemove(Collection<Endpoint> expectedComposedNodeEndpoints, Collection<EndpointResource> existingEndpointResources) {
        Collection<Endpoint> existingEndpoints = existingEndpointResources.stream()
            .map(EndpointBuilder::asEndpoint)
            .collect(toList());
        return getEndpointsNotInSecondCollection(existingEndpoints, expectedComposedNodeEndpoints);
    }

    public List<Endpoint> getEndpointsToAdd(Collection<Endpoint> expectedComposedNodeEndpoints, Collection<EndpointResource> existingEndpointResources) {
        Collection<Endpoint> existingEndpoints = existingEndpointResources.stream()
            .map(EndpointBuilder::asEndpoint)
            .collect(toList());
        return getEndpointsNotInSecondCollection(expectedComposedNodeEndpoints, existingEndpoints);
    }

    private List<Endpoint> getEndpointsNotInSecondCollection(Collection<Endpoint> first, Collection<Endpoint> second) {
        List<Endpoint> endpoints = new ArrayList<>();
        for (Endpoint endpoint : first) {
            if (checkIfEndpointIsNotInCollection(endpoint, second)
                && checkIfEndpointIsNotInCollection(endpoint, endpoints)) {
                log.trace("Equator: endpoint to add: " + EndpointEquator.toEquatorString(endpoint));
                log.trace("Equator: list without endpoint: " + first.stream().map(EndpointEquator::toEquatorString).collect(joining(", ")));
                endpoints.add(endpoint);
            }
        }
        return endpoints;
    }

    private boolean checkIfEndpointIsNotInCollection(Endpoint endpoint, Collection<Endpoint> endpoints) {
        return endpoints.stream().noneMatch(el -> EQUATOR.equate(el, endpoint));
    }
}
