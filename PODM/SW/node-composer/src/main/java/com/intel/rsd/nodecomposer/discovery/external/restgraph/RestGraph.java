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

import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ServiceRootResource;

import java.time.LocalDateTime;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

import static java.util.Collections.unmodifiableCollection;
import static java.util.stream.Collectors.collectingAndThen;
import static java.util.stream.Collectors.toList;

public final class RestGraph {
    private final HashSet<ExternalServiceResource> resources = new HashSet<>();
    private final HashSet<ResourceLink> links = new HashSet<>();
    private final ServiceRootResource serviceRootResource;
    private final LocalDateTime captureTime;

    public RestGraph(ServiceRootResource serviceRoot, Set<ResourceLink> allLinks, LocalDateTime captureTime) {
        this.serviceRootResource = serviceRoot;
        allLinks.forEach(this::add);
        this.captureTime = captureTime;
    }

    public Collection<ResourceLink> getLinks() {
        return unmodifiableCollection(links);
    }

    public Collection<ExternalServiceResource> getResources() {
        return resources.stream().filter(r -> !r.equals(serviceRootResource)).collect(collectingAndThen(toList(), Collections::unmodifiableList));
    }

    public boolean contains(ResourceLink resourceLink) {
        return links.contains(resourceLink);
    }

    public LocalDateTime getCaptureTime() {
        return captureTime;
    }

    private void add(ResourceLink link) {
        resources.add(link.getSource());
        resources.add(link.getTarget());
        links.add(new ResourceLink(link.getSource(), link.getTarget(), link.getLinkName()));
    }
}
