/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.FabricActionsInvoker;
import com.intel.podm.business.redfish.services.actions.ZoneActionsInvoker;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.actions.ZoneActionRequest;
import com.intel.podm.common.types.redfish.NavigationProperty;
import com.intel.podm.common.types.redfish.RedfishZone;
import com.intel.podm.common.types.redfish.ZoneCreationInterface;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Collection;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.business.redfish.Contexts.toContext;
import static java.util.Collections.emptySet;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ZoneActionsService {
    @Inject
    private ZoneActionsInvoker invoker;

    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private FabricActionsInvoker zoneCreator;

    @Transactional(REQUIRES_NEW)
    public Context createZone(Context creationalContext, ZoneCreationInterface representation)
        throws ContextResolvingException, EntityOperationException {
        Fabric fabric = (Fabric) traverser.traverse(creationalContext);
        Zone createdZone = zoneCreator.createZone(fabric, buildCreationRequest(representation));
        fabric.addZone(createdZone);
        return toContext(createdZone);
    }

    @Transactional(REQUIRES_NEW)
    void deleteZone(Context context) throws BusinessApiException {
        Zone zone = (Zone) traverser.traverse(context);
        invoker.updateZone(zone, emptySet());
        invoker.deleteZone(zone);
    }

    @Transactional(REQUIRES_NEW)
    @SuppressWarnings({"unchecked"})
    public void updateZoneEndpointsCollection(Context zoneContext, RedfishZone representation) throws BusinessApiException {
        Zone zone = (Zone) traverser.traverse(zoneContext);

        if (zoneAssociatedWithComposedNode(zone)) {
            throw new ResourceStateMismatchException("Patch action cannot be performed, zone -> " + zone.getId()
                + " endpoints connected with composed node.");
        }

        Set<Endpoint> endpoints = traverser.traverse(representation.getEndpoints());
        invoker.updateZone(zone, endpoints);
    }

    private ZoneActionRequest buildCreationRequest(ZoneCreationInterface representation) throws ContextResolvingException {
        return new ZoneActionRequest(mapSourceEndpointUrisToExternalServiceEndpointUri(representation));
    }

    private Set<URI> mapSourceEndpointUrisToExternalServiceEndpointUri(ZoneCreationInterface representation) throws ContextResolvingException {
        Set<? extends NavigationProperty> navigationProperties = representation.getEndpoints();
        if (navigationProperties == null) {
            return emptySet();
        }

        return retrieveEndpointUris(navigationProperties);
    }

    private Set<URI> retrieveEndpointUris(Set<? extends NavigationProperty> navigationProperties)
        throws ContextResolvingException {

        Set<Context> endpoints = navigationProperties.stream()
            .map(Context.class::cast)
            .collect(toSet());

        return getEndpointUris(endpoints);
    }

    private Set<URI> getEndpointUris(Set<Context> endpointsContext) throws ContextResolvingException {
        Set<Endpoint> endpoints = traverser.traverse(endpointsContext);
        return collectEndpointUris(endpoints);
    }

    private Set<URI> collectEndpointUris(Set<Endpoint> endpoints) {
        return endpoints.stream().map(DiscoverableEntity::getSourceUri).collect(toSet());
    }

    private boolean zoneAssociatedWithComposedNode(Zone zone) {
        return zone.getEndpoints().stream().map(Endpoint::getConnectedEntities)
            .flatMap(Collection::stream)
            .map(ConnectedEntity::getEntityLink)
            .filter(Drive.class::isInstance)
            .map(Drive.class::cast)
            .filter(pcieDrive -> pcieDrive.getMetadata().isAllocated())
            .map(Drive::getComposedNode)
            .anyMatch(Objects::nonNull);
    }
}
