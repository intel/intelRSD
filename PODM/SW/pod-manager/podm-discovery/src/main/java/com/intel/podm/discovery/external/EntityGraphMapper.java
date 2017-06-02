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

package com.intel.podm.discovery.external;

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.discovery.external.finalizers.DiscoveryFinalizer;
import com.intel.podm.discovery.external.linker.EntityLink;
import com.intel.podm.discovery.external.linker.EntityLinks;
import com.intel.podm.discovery.external.restgraph.ResourceLink;
import com.intel.podm.discovery.external.restgraph.RestGraph;

import javax.ejb.AccessTimeout;
import javax.ejb.Singleton;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static com.intel.podm.common.utils.Maps.filterNonNullValues;
import static com.intel.podm.common.utils.Maps.inverse;
import static java.util.concurrent.TimeUnit.MINUTES;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Singleton
@Interceptors(RetryOnRollbackInterceptor.class)
public class EntityGraphMapper {
    @Inject
    private EntityMultiMapper multiMapper;

    @Inject
    private EntityLinks links;

    @Inject
    private ExternalServiceRepository repository;

    @Inject
    private DiscoveryFinalizer discoveryFinalizer;

    @Transactional(REQUIRES_NEW)
    @AccessTimeout(value = 5, unit = MINUTES)
    @NumberOfRetriesOnRollback(3)
    @TimeMeasured(tag = "[Discovery]")
    public void map(RestGraph graph) {
        requiresNonNull(graph, "graph");

        ExternalService service = repository.find(graph.findServiceUuid());

        Map<ExternalServiceResource, Entity> map = filterNonNullValues(multiMapper.map(graph.getResources(), service));
        updateLinks(graph, map);

        Set<Entity> entities = new HashSet<>(map.values());
        discoveryFinalizer.finalizeDiscovery(entities, service);
    }

    private void updateLinks(RestGraph graph, Map<ExternalServiceResource, Entity> map) {
        for (ResourceLink link : graph.getLinks()) {
            EntityLink entityLink = toEntityLink(link, map);

            if (entityLink != null) {
                links.link(entityLink);
            }
        }

        Collection<Entity> entities = map.values();
        for (EntityLink entityLink : links.getLinksThatMayBeRemoved(entities)) {
            ResourceLink resourceLink = toResourceLink(entityLink, inverse(map));

            if (resourceLink != null && !graph.contains(resourceLink)) {
                links.unlink(entityLink);
            }
        }
    }

    private EntityLink toEntityLink(ResourceLink link, Map<ExternalServiceResource, Entity> map) {
        String linkName = link.getName();
        Entity source = map.get(link.getSource());
        Entity target = map.get(link.getTarget());

        return source != null && target != null
            ? new EntityLink(source, target, linkName)
            : null;
    }

    private ResourceLink toResourceLink(EntityLink link, Map<Entity, ExternalServiceResource> map) {
        String linkName = link.getName();
        ExternalServiceResource source = map.get(link.getSource());
        ExternalServiceResource target = map.get(link.getTarget());

        return source != null && target != null
            ? new ResourceLink(source, target, linkName)
            : null;
    }
}
