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

package com.intel.podm.discovery.external;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.client.api.resources.redfish.RackscaleServiceRootResource;
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.discovery.external.finalizers.DiscoveryFinalizer;
import com.intel.podm.discovery.external.linker.DomainObjectLinker;
import com.intel.podm.discovery.external.restgraph.ResourceLink;
import com.intel.podm.discovery.external.restgraph.RestGraph;

import javax.ejb.AccessTimeout;
import javax.ejb.Singleton;
import javax.enterprise.inject.Instance;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static java.util.concurrent.TimeUnit.MINUTES;
import static java.util.stream.Collectors.toList;
import static java.util.stream.StreamSupport.stream;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Singleton
@Interceptors(RetryOnRollbackInterceptor.class)
public class DomainObjectGraphMapper {
    @Inject
    private DomainObjectMultiMapper multiMapper;

    @Inject
    private DomainObjectLinker linker;

    @Inject
    private ExternalServiceRepository repository;

    @Inject
    private Instance<DiscoveryFinalizer> discoveryFinalizers;

    @Inject
    private DiscoveryScheduler discoveryScheduler;

    @Transactional(REQUIRES_NEW)
    @AccessTimeout(value = 5, unit = MINUTES)
    @NumberOfRetriesOnRollback(3)
    public void map(RestGraph graph) {
        UUID uuid = findUuid(graph);

        ExternalService service = repository.find(uuid);
        if (service.getIsDirty()) {
            service.setIsDirty(false);
            discoveryScheduler.triggerDiscovery(service.getUuid());
            return;
        }
        Map<ExternalServiceResource, DomainObject> map = multiMapper.map(graph.getResources(), service);

        Set<DomainObject> domainObjects = linkDomainObjects(graph, map);
        finalizeDiscoveryProcess(domainObjects, service);
    }

    private void finalizeDiscoveryProcess(Set<DomainObject> discoveredDomainObjects, ExternalService service) {
        DiscoveryFinalizer finalizer = singleOrNull(stream(discoveryFinalizers.spliterator(), false)
                .filter(discoveryFinalizer -> discoveryFinalizer.isAppropriateForServiceType(service.getServiceType()))
                .collect(toList()));

        if (finalizer != null) {
            finalizer.finalize(discoveredDomainObjects, service);
        }
    }

    private UUID findUuid(RestGraph graph) {
        for (ExternalServiceResource resource : graph.getResources()) {
            if (resource instanceof RackscaleServiceRootResource) {
                return ((RackscaleServiceRootResource) resource).getUuid();
            }
        }

        throw new IllegalStateException("uuid was not found");
    }

    private Set<DomainObject> linkDomainObjects(RestGraph graph, Map<ExternalServiceResource, DomainObject> map) {
        for (ResourceLink link : graph.getLinks()) {
            String linkName = link.getName();
            DomainObject source = map.get(link.getSource());
            DomainObject target = map.get(link.getTarget());

            if (source != null && target != null) {
                linker.link(source, target, linkName);
            }
        }

        Set<DomainObject> discoveredDomainObjects = new HashSet<>();
        for (DomainObject domainObject : map.values()) {
            if (domainObject != null) {
                discoveredDomainObjects.add(domainObject);
            }
        }
        return discoveredDomainObjects;
    }
}
