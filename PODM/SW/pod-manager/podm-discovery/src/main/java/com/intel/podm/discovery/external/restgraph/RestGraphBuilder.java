/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.discovery.external.restgraph;

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ExternalServiceReader;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.client.api.resources.redfish.ChassisResource;
import com.intel.podm.client.api.resources.redfish.RackscaleServiceRootResource;
import com.intel.podm.discovery.external.ExternalServiceRepository;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.ArrayDeque;
import java.util.Collection;
import java.util.Objects;
import java.util.Queue;
import java.util.Set;
import java.util.function.Predicate;

import static com.google.common.base.Preconditions.checkArgument;
import static com.google.common.collect.Sets.newHashSet;
import static com.intel.podm.common.types.ChassisType.DRAWER;
import static com.intel.podm.common.types.ServiceType.RMM;
import static java.util.stream.Collectors.toList;
import static java.util.stream.StreamSupport.stream;
import static javax.transaction.Transactional.TxType.REQUIRED;

/**
 * Visits all resources from rssClient and creates graph from them
 */
@Dependent
@Transactional(REQUIRED)
public class RestGraphBuilder {
    @Inject
    private ExternalServiceRepository repository;

    @Inject
    private ResourceLinkExtractor extractor;

    public RestGraph build(ExternalServiceReader client) throws ExternalServiceApiReaderException {
        RackscaleServiceRootResource serviceRoot = client.getServiceRoot();
        checkArgument(serviceRoot.getUuid() != null, "Service being discovered must have UUID assigned.");

        Queue<ExternalServiceResource> queue = new ArrayDeque<>();
        queue.add(serviceRoot);

        RestGraph graph = new RestGraph();
        Set<URI> visited = newHashSet();

        while (!queue.isEmpty()) {
            ExternalServiceResource current = queue.poll();
            visited.add(current.getUri());

            Iterable<ResourceLink> relatedLinks = extractor.extractFrom(current);
            Iterable<ResourceLink> filteredLinks = filterDrawerChasissFromRmm(relatedLinks, repository.find(serviceRoot.getUuid()));
            Collection<? extends ExternalServiceResource> notVisited = filterNotVisited(filteredLinks, visited);

            graph.addAll(filteredLinks);
            queue.addAll(notVisited);
        }

        return graph;
    }

    /**
     * During RMM discovery process, we need to remove Chassis of type Drawer
     * to avoid useless domain objects mapping
     * @param relatedLinks
     * @param serviceType
     * @return
     */
    private Iterable<ResourceLink> filterDrawerChasissFromRmm(Iterable<ResourceLink> relatedLinks, ExternalService service) {
        boolean isRmmService = Objects.equals(service.getServiceType(), RMM);
        Predicate<ResourceLink> isRmmDrawerChassis = resourceLink -> isRmmService
                && (isDrawerChassis(resourceLink.getSource()) || isDrawerChassis(resourceLink.getTarget()));

        return stream(relatedLinks.spliterator(), false)
                .filter(isRmmDrawerChassis.negate())
                .collect(toList());
    }

    private boolean isDrawerChassis(ExternalServiceResource resource) {
        return resource != null
                && resource instanceof ChassisResource
                && Objects.equals(((ChassisResource) resource).getChassisType(), DRAWER);
    }

    private Collection<ExternalServiceResource> filterNotVisited(Iterable<ResourceLink> links, Set<URI> visited) {
        return stream(links.spliterator(), false)
                .map(ResourceLink::getTarget)
                .filter(resource -> !visited.contains(resource.getUri()))
                .collect(toList());
    }
}
