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

package com.intel.podm.actions;

import com.intel.podm.business.entities.dao.DiscoverableEntityDao;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.actions.ZoneResourceActions;
import com.intel.podm.client.api.actions.ZoneResourceActionsFactory;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.ZoneResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.ZoneUpdateDefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.stream.StreamSupport;

import static com.intel.podm.common.utils.Contracts.requires;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ZoneUpdateInvoker {
    @Inject
    private ZoneResourceActionsFactory zoneResourceActionsFactory;

    @Inject
    private DiscoverableEntityDao discoverableEntityDao;

    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    public void updateZone(Zone zone, Set<Endpoint> newEndpointsCollection) throws ActionException {
        ExternalService service = zone.getService();
        requires(service != null, "There is no Service associated with selected zone");

        URI baseUri = service.getBaseUri();
        URI zoneUri = zone.getSourceUri();

        try (ZoneResourceActions actions = zoneResourceActionsFactory.create(baseUri)) {
            ZoneResource zoneResource = actions.update(zoneUri, new ZoneUpdateDefinition(collectEndpointUris(newEndpointsCollection)));
            zone.clearEndpointsCollection();
            addNewEndpointsToZone(zone, zoneResource);
        } catch (ExternalServiceApiActionException e) {
            String errorMessage = "Update on selected Zone failed";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), zoneUri);
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        } catch (ExternalServiceApiReaderException e) {
            String errorMessage = "Update action was successful, but refreshing selected Zone failed";
            logger.i(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), zoneUri);
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        }
    }

    private Set<URI> collectEndpointUris(Set<Endpoint> endpoints) {
        return endpoints.stream().map(DiscoverableEntity::getSourceUri).collect(toSet());
    }

    private void addNewEndpointsToZone(Zone zone, ZoneResource zoneResource) throws ExternalServiceApiReaderException {
        StreamSupport.stream(zoneResource.getEndpoints().spliterator(), false)
            .map(endpointResourceSupplier -> getEndpoint(endpointResourceSupplier, zone.getService()))
            .filter(Objects::nonNull)
            .forEach(zone::addEndpoint);
    }

    private Endpoint getEndpoint(ResourceSupplier endpointResourceSupplier, ExternalService service) {
        try {
            List<DiscoverableEntity> discoverableEntities =
                discoverableEntityDao.getByExternalServiceAndSourceUri(service, endpointResourceSupplier.get().getUri());

            if (discoverableEntities.size() == 1) {
                return (Endpoint) discoverableEntities.iterator().next();
            } else {
                logger.e("More then one entity found with URI {}. {}", endpointResourceSupplier.get().getUri(),
                    "Entity will be omitted.");
                return null;
            }
        } catch (ExternalServiceApiReaderException e) {
            logger.e("Error while retrieving endpoint resource supplier {} -> {}", e.getMessage() + e.getCause());
            return null;
        }
    }
}
