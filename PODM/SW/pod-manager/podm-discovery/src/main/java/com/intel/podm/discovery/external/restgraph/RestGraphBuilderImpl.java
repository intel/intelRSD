/*
 * Copyright (c) 2015-2017 Intel Corporation
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

import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ExternalServiceReader;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.redfish.ServiceRootResource;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.synchronization.CancelableChecker;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.ArrayDeque;
import java.util.Collection;
import java.util.HashSet;
import java.util.Queue;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requires;
import static java.util.stream.Collectors.toSet;
import static java.util.stream.StreamSupport.stream;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
class RestGraphBuilderImpl implements RestGraphBuilder {
    @Inject
    private ResourceLinkExtractor extractor;

    private CancelableChecker cancelableChecker;

    public void setCancelableChecker(CancelableChecker cancelableChecker) {
        this.cancelableChecker = cancelableChecker;
    }

    @Override
    @TimeMeasured(tag = "[Discovery]")
    @Transactional(MANDATORY)
    public RestGraph build(ExternalServiceReader client) throws WebClientRequestException {
        ServiceRootResource serviceRoot = client.getServiceRoot();
        requires(serviceRoot.getUuid() != null, "Service being discovered must have UUID assigned.");

        Queue<ExternalServiceResource> queue = new ArrayDeque<>();
        queue.add(serviceRoot);

        RestGraph graph = new RestGraph(serviceRoot);
        Set<URI> visited = new HashSet<>();
        Set<ResourceLink> allLinks = new HashSet<>();
        while (!queue.isEmpty()) {
            cancelableChecker.check();
            ExternalServiceResource current = queue.poll();
            visited.add(current.getUri());

            Collection<ResourceLink> relatedLinks = extractor.extractFrom(current);
            Collection<? extends ExternalServiceResource> notVisited = filterNotVisited(relatedLinks, visited);

            allLinks.addAll(relatedLinks);
            queue.addAll(notVisited);
        }

        graph.addAll(allLinks);
        return graph;
    }

    private Collection<ExternalServiceResource> filterNotVisited(Iterable<ResourceLink> links, Set<URI> visited) {
        return stream(links.spliterator(), false)
            .map(ResourceLink::getTarget)
            .filter(resource -> !visited.contains(resource.getUri()))
            .collect(toSet());
    }
}
