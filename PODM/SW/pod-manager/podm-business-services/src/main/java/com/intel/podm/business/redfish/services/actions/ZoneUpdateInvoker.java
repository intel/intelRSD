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

package com.intel.podm.business.redfish.services.actions;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.entities.dao.DiscoverableEntityDao;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.ZoneUpdateRequest;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.redfish.ZoneResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.actions.ZoneUpdateDefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Objects;
import java.util.Set;
import java.util.stream.StreamSupport;

import static com.intel.podm.common.utils.Contracts.requires;
import static java.lang.String.format;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ZoneUpdateInvoker {
    @Inject
    private DiscoverableEntityDao discoverableEntityDao;

    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    public void updateZone(Zone zone, Set<Endpoint> newEndpointsCollection) throws BusinessApiException {
        ExternalService service = zone.getService();
        requires(service != null, "There is no Service associated with selected zone");

        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            ZoneResource zoneResource = performUpdateZoneAction(webClient, newEndpointsCollection, service, zone);
            refreshZoneEndpointsAfterUpdate(service, zone, zoneResource);
        }
    }

    private ZoneResource performUpdateZoneAction(WebClient webClient, Set<Endpoint> newEndpointsCollection, ExternalService service, Zone zone)
        throws BusinessApiException {

        URI zoneUri = zone.getSourceUri();
        try {
            return webClient.patchAndRetrieve(zoneUri, new ZoneUpdateRequest(new ZoneUpdateDefinition(collectEndpointUris(newEndpointsCollection))));
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
            zone.clearEndpointsCollection();

            StreamSupport.stream(zoneResource.getEndpoints().spliterator(), false)
                .map(endpointResourceSupplier -> getEndpoint(endpointResourceSupplier, zone.getService()))
                .filter(Objects::nonNull)
                .forEach(zone::addEndpoint);
        } catch (WebClientRequestException e) {
            String errorMessage = "Update action was successful, but refreshing selected Zone failed";
            logger.i(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), zone.getSourceUri());
            throw new BusinessApiException(errorMessage, e);
        }
    }

    private Endpoint getEndpoint(ResourceSupplier endpointResourceSupplier, ExternalService service) {
        try {
            ExternalServiceResource externalResource = endpointResourceSupplier.get();
            Id globalId = externalResource.getGlobalId(service.getId(), externalResource.getUri());
            Endpoint endpoint = discoverableEntityDao.findByGlobalId(globalId, Endpoint.class);

            if (endpoint == null) {
                logger.e("Entity(id:{}) doesn't exist", globalId);
                return null;
            }
            return endpoint;
        } catch (WebClientRequestException e) {
            logger.e(format("Error while retrieving endpoint resource supplier: %s", e.getMessage()), e);
            return null;
        }
    }
}
