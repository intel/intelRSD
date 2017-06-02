/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.actions.ActionException;
import com.intel.podm.actions.ZoneUpdateInvoker;
import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.EntityType;
import com.intel.podm.common.types.redfish.RedfishZone;

import javax.ejb.Stateless;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.Objects;
import java.util.Set;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Stateless
public class ZoneActionService {
    @Inject
    private ZoneUpdateInvoker invoker;

    @Inject
    private EntityTreeTraverser traverser;

    @Transactional(REQUIRES_NEW)
    public void updateZoneEndpointsCollection(Context zoneContext, RedfishZone representation) throws BusinessApiException {
        Zone zone = (Zone) traverser.traverse(zoneContext);

        if (zoneAssociatedWithComposedNode(zone)) {
            throw new ResourceStateMismatchException("Patch action cannot be performed, zone -> " + zone.getId()
                + " endpoints connected with composed node.");
        }

        Set<Endpoint> endpoints = traverser.traverse(representation.getEndpoints());
        try {
            invoker.updateZone(zone, endpoints);
        } catch (ActionException e) {
            throw new BusinessApiException("Zone Update task could not be completed!" + e.getMessage());
        }
    }

    private boolean zoneAssociatedWithComposedNode(Zone zone) {
        return zone.getEndpoints().stream().map(Endpoint::getConnectedEntities)
            .flatMap(Collection::stream)
            .filter(connectedEntity -> connectedEntity.getEntityType() == EntityType.DRIVE)
            .map(ConnectedEntity::getEntityLink)
            .filter(Objects::nonNull)
            .filter(entity -> entity instanceof Drive)
            .map(Drive.class::cast)
            .filter(pcieDrive -> pcieDrive.getMetadata().isAllocated())
            .map(Drive::getComposedNode)
            .anyMatch(Objects::nonNull);
    }
}
