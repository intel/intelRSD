/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.discovery.external.restgraph;

import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ServiceRootResource;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.net.URI;
import java.util.ArrayDeque;
import java.util.Collection;
import java.util.HashSet;
import java.util.Queue;
import java.util.Set;

import static com.intel.rsd.nodecomposer.utils.Contracts.requires;
import static java.util.stream.Collectors.toSet;
import static java.util.stream.StreamSupport.stream;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
@Slf4j
public class RestGraphBuilder {
    private final ResourceLinkExtractor extractor;
    private final WebClientBuilder webClientBuilder;

    @Autowired
    public RestGraphBuilder(ResourceLinkExtractor extractor, WebClientBuilder webClientBuilder) {
        this.extractor = extractor;
        this.webClientBuilder = webClientBuilder;
    }

    @TimeMeasured(tag = "[Discovery]")
    @SuppressWarnings({"checkstyle:MethodLength", "checkstyle:ExecutableStatementCount"})
    public RestGraph build(URI baseUri) throws WebClientRequestException {
        WebClient webClient = webClientBuilder.createResourceManagerInstance().cachable().retryable().build();
        ServiceRootResource serviceRoot = (ServiceRootResource) webClient.get(webClient.getBaseUri());
        requires(serviceRoot.getUuid() != null, "Service being discovered must have UUID assigned.");

        Queue<ExternalServiceResource> queue = new ArrayDeque<>();
        queue.add(serviceRoot);

        RestGraph graph = new RestGraph(serviceRoot);
        Set<URI> visited = new HashSet<>();
        Set<ResourceLink> allLinks = new HashSet<>();
        while (!queue.isEmpty()) {
            ExternalServiceResource current = queue.poll();
            visited.add(current.getUri());

            Collection<ResourceLink> relatedLinks = extractor.extractFrom(current);
            Collection<? extends ExternalServiceResource> notVisited = filterNotVisited(relatedLinks, visited);

            allLinks.addAll(relatedLinks);
            queue.addAll(notVisited);
        }

        log.info("Crawled {} resources", allLinks.size());
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
