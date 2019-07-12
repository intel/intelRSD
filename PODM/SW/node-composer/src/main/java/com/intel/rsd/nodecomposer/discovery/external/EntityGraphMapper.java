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

package com.intel.rsd.nodecomposer.discovery.external;

import com.intel.rsd.nodecomposer.discovery.external.finalizers.DiscoveryFinalizer;
import com.intel.rsd.nodecomposer.discovery.external.linker.EntityLink;
import com.intel.rsd.nodecomposer.discovery.external.linker.EntityLinks;
import com.intel.rsd.nodecomposer.discovery.external.restgraph.ResourceLink;
import com.intel.rsd.nodecomposer.discovery.external.restgraph.RestGraph;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResource;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.Collection;
import java.util.HashSet;
import java.util.Map;
import java.util.Objects;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static com.intel.rsd.nodecomposer.utils.Maps.filterNonNullValues;
import static com.intel.rsd.nodecomposer.utils.Maps.inverse;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class EntityGraphMapper {
    private final DiscoveryFinalizer discoveryFinalizer;
    private final EntityMultiMapper multiMapper;
    private final EntityLinks links;

    @Autowired
    public EntityGraphMapper(DiscoveryFinalizer discoveryFinalizer,
                             EntityMultiMapper multiMapper,
                             EntityLinks links) {
        this.discoveryFinalizer = discoveryFinalizer;
        this.multiMapper = multiMapper;
        this.links = links;
    }

    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[Discovery]")
    public void map(RestGraph graph) {
        requiresNonNull(graph, "graph");

        Map<ExternalServiceResource, DiscoverableEntity> map = filterNonNullValues(multiMapper.map(graph.getResources()));

        updateLinks(graph, map);

        discoveryFinalizer.finalizeDiscovery(new HashSet<>(map.values()));
    }

    @TimeMeasured(tag = "[Discovery]")
    private void updateLinks(RestGraph graph, Map<ExternalServiceResource, DiscoverableEntity> map) {
        graph.getLinks().stream()
            .map(link -> toEntityLink(link, map))
            .filter(Objects::nonNull)
            .forEach(links::link);

        Collection<DiscoverableEntity> entities = map.values();
        for (EntityLink entityLink : links.getLinksThatMayBeRemoved(entities)) {
            ResourceLink resourceLink = toResourceLink(entityLink, inverse(map));

            if (resourceLink != null && !graph.contains(resourceLink)) {
                links.unlink(entityLink);
            }
        }
    }

    private EntityLink toEntityLink(ResourceLink link, Map<ExternalServiceResource, DiscoverableEntity> map) {
        String linkName = link.getLinkName();
        Entity source = map.get(link.getSource());
        Entity target = map.get(link.getTarget());

        return (source != null && target != null)
            ? new EntityLink(source, target, linkName)
            : null;
    }

    @SuppressWarnings("SuspiciousMethodCalls")
    private ResourceLink toResourceLink(EntityLink link, Map<DiscoverableEntity, ExternalServiceResource> map) {
        String linkName = link.getName();
        ExternalServiceResource source = map.get(link.getSource());
        ExternalServiceResource target = map.get(link.getTarget());

        return (source != null && target != null)
            ? new ResourceLink(source, target, linkName)
            : null;
    }
}
