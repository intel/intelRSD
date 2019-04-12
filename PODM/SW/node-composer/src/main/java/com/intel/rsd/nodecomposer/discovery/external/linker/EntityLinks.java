/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.discovery.external.linker;

import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Zone;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.Objects;
import java.util.stream.Stream;

import static com.intel.rsd.nodecomposer.utils.Contracts.requires;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.util.stream.Collectors.toSet;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

/**
 * Class exposes interface that allows to manage links between entities.
 * <p>
 * implementation has some limitations. It assumes that only links between zones and endpoints may be removed.
 * It was introduced to support scenarios where external firmware update tools reattach drive to management host.
 * To support more cases it rather should be redesigned with other classes instead of adding new control flow statements to this class.
 */
@Component
@Scope(SCOPE_PROTOTYPE)
public class EntityLinks {
    private static final String ENDPOINT_IN_ZONE = "endpointInZone";

    private final EntityLinker linker;

    @Autowired
    public EntityLinks(EntityLinker linker) {
        this.linker = linker;
    }

    public Collection<EntityLink> getLinksThatMayBeRemoved(Collection<DiscoverableEntity> entities) {
        requiresNonNull(entities, "entities");

        return entities
            .stream()
            .filter(Zone.class::isInstance)
            .map(Zone.class::cast)
            .flatMap(this::getZonesToEndpointLinks)
            .collect(toSet());
    }

    public void unlink(EntityLink entityLink) {
        requiresNonNull(entityLink, "entityLink");
        requires(entityLink.getSource() instanceof Zone, "entityLink.source should be instance of Zone");
        requires(entityLink.getSource() instanceof Zone, "entityLink.target should be instance of Endpoint");
        requires(Objects.equals(entityLink.getName(), ENDPOINT_IN_ZONE), "entityLink.name should be equal to 'endpointInZone'");

        Zone zone = (Zone) entityLink.getSource();
        Endpoint endpoint = (Endpoint) entityLink.getTarget();
        zone.unlinkEndpoint(endpoint);
    }

    public void link(EntityLink entityLink) {
        requiresNonNull(entityLink, "entityLink");
        linker.link(entityLink.getSource(), entityLink.getTarget(), entityLink.getName());
    }

    private Stream<EntityLink> getZonesToEndpointLinks(Zone zone) {
        return zone
            .getEndpoints()
            .stream()
            .map(endpoint -> new EntityLink(zone, endpoint, ENDPOINT_IN_ZONE));
    }
}
