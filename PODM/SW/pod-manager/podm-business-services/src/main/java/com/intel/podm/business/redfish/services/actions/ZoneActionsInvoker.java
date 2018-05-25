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

package com.intel.podm.business.redfish.services.actions;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.dao.DiscoverableEntityDao;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.ZoneActionJson;
import com.intel.podm.client.resources.redfish.ZoneResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.ZoneActionRequest;
import com.intel.podm.discovery.external.partial.ZoneObtainer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requires;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ZoneActionsInvoker {
    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private Logger logger;

    @Inject
    private ZoneObtainer zoneObtainer;

    @Inject
    private DiscoverableEntityDao discoverableEntityDao;

    @Inject
    private FabricActionsInvoker fabricActionService;

    @Transactional(MANDATORY)
    public void updateZone(Zone zone, Set<Endpoint> newEndpointsCollection) throws BusinessApiException {
        ExternalService service = zone.getService();
        requires(service != null, "There is no Service associated with selected zone");

        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            ZoneResource zoneResource = performUpdateZoneAction(webClient, newEndpointsCollection, service, zone);
            refreshZoneEndpointsAfterUpdate(service, zone, zoneResource);
        }
    }

    @Transactional(MANDATORY)
    public void deleteZone(Zone zone) throws EntityOperationException {
        ExternalService service = zone.getService();
        requiresNonNull(service, "service");

        URI zoneUri = zone.getSourceUri();
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            webClient.delete(zoneUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "Selected Zone could not be deleted";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), zoneUri);
            throw new EntityOperationException(errorMessage, e);
        }

        discoverableEntityDao.removeWithConnectedExternalLinks(zone);
    }

    @Transactional(MANDATORY)
    public void createOrUpdateZone(Zone zone, Set<Endpoint> endpoints, Fabric fabric) throws BusinessApiException {
        if (zone == null) {
            fabricActionService.createZone(fabric, new ZoneActionRequest(endpoints.stream().map(DiscoverableEntity::getSourceUri).collect(toSet())));
        } else {
            updateZone(zone, endpoints);
        }
    }

    private ZoneResource performUpdateZoneAction(WebClient webClient, Set<Endpoint> newEndpointsCollection, ExternalService service, Zone zone)
        throws BusinessApiException {

        URI zoneUri = zone.getSourceUri();
        try {
            return webClient.patchAndRetrieve(zoneUri, new ZoneActionJson(collectEndpointUris(newEndpointsCollection)));
        } catch (WebClientRequestException e) {
            String errorMessage = "Update on selected Zone failed";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), zoneUri);
            throw new BusinessApiException(errorMessage, e);
        }
    }

    private Set<URI> collectEndpointUris(Set<Endpoint> endpoints) {
        return endpoints.stream().map(DiscoverableEntity::getSourceUri).collect(toSet());
    }

    private void refreshZoneEndpointsAfterUpdate(ExternalService service, Zone zone, ZoneResource zoneResource) throws BusinessApiException {
        try {
            zoneObtainer.refreshZoneEndpointsAfterUpdate(zone, zoneResource);
        } catch (WebClientRequestException e) {
            String errorMessage = "Update action was successful, but refreshing selected Zone failed";
            logger.i(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), zone.getSourceUri());
            throw new BusinessApiException(errorMessage, e);
        }
    }
}
