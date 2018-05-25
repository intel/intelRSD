/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.discovery.external.partial;

import com.intel.podm.business.entities.dao.DiscoverableEntityDao;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.redfish.ZoneResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;
import com.intel.podm.mappers.redfish.ZoneMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Objects;
import java.util.stream.StreamSupport;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ZoneObtainer {
    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private DiscoverableEntityDao discoverableEntityDao;

    @Inject
    private ZoneMapper mapper;

    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    public Zone discoverZone(ExternalService service, Fabric fabric, URI zoneUri) throws WebClientRequestException {
        requiresNonNull(service, "service", "There is no Service associated with selected volume");

        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            ZoneResource zoneResource = (ZoneResource) webClient.get(zoneUri);
            Id entityId = zoneResource.getGlobalId(service.getId());
            Zone zone = discoverableEntityDao.findOrCreateEntity(service, entityId, zoneResource.getUri(), Zone.class);
            mapper.map(zoneResource, zone);
            refreshZoneEndpointsAfterUpdate(zone, zoneResource);
            fabric.addZone(zone);

            return zone;
        }
    }

    @Transactional(MANDATORY)
    public void refreshZoneEndpointsAfterUpdate(Zone zone, ZoneResource zoneResource) throws WebClientRequestException {
        zone.clearEndpointsCollection();

        StreamSupport.stream(zoneResource.getEndpoints().spliterator(), false)
            .map(endpointResourceSupplier -> getEndpoint(endpointResourceSupplier, zone.getService()))
            .filter(Objects::nonNull)
            .forEach(zone::addEndpoint);
    }

    private Endpoint getEndpoint(ResourceSupplier endpointResourceSupplier, ExternalService service) {
        try {
            ExternalServiceResource externalResource = endpointResourceSupplier.get();
            Id globalId = externalResource.getGlobalId(service.getId());
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
