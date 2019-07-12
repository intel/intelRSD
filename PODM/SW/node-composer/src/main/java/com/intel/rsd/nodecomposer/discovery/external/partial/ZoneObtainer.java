/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.discovery.external.partial;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ZoneResource;
import com.intel.rsd.nodecomposer.mappers.redfish.ZoneMapper;
import com.intel.rsd.nodecomposer.persistence.dao.DiscoverableEntityDao;
import com.intel.rsd.nodecomposer.persistence.dao.EndpointDao;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.Zone;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.net.URI;
import java.util.Objects;
import java.util.Optional;
import java.util.stream.StreamSupport;

import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromUri;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ZoneObtainer {
    private final WebClientBuilder webClientBuilder;
    private final DiscoverableEntityDao discoverableEntityDao;
    private final EndpointDao endpointDao;
    private final ZoneMapper mapper;
    private final GenericDao genericDao;

    @Autowired
    public ZoneObtainer(WebClientBuilder webClientBuilder, DiscoverableEntityDao discoverableEntityDao, EndpointDao endpointDao, ZoneMapper mapper,
                        GenericDao genericDao) {
        this.webClientBuilder = webClientBuilder;
        this.discoverableEntityDao = discoverableEntityDao;
        this.endpointDao = endpointDao;
        this.mapper = mapper;
        this.genericDao = genericDao;
    }

    @Transactional(MANDATORY)
    public Zone discoverZone(Fabric fabric, URI zoneUri) throws WebClientRequestException {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            ZoneResource zoneResource = (ZoneResource) webClient.get(zoneUri);
            URI sourceZoneUri = URI.create(zoneUri.getPath());
            Zone zone = (Zone) discoverableEntityDao.findOrCreateEntity(sourceZoneUri, Zone.class);
            mapper.map(zoneResource, zone);
            refreshZoneEndpointsAfterUpdate(zone, zoneResource);
            fabric.addZone(zone);

            return zone;
        }
    }

    @Transactional(MANDATORY)
    public void discoverZone(URI fabricUri, URI zoneUri) throws BusinessApiException {
        val fabricODataId = oDataIdFromUri(fabricUri);
        val fabric = genericDao.find(Fabric.class, fabricODataId);
        try {
            discoverZone(fabric, zoneUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "Zone refreshing failed on selected Fabric";
            log.warn(errorMessage + " on [ path: {} ]", zoneUri);
            throw new BusinessApiException(errorMessage, e);
        }
    }

    @Transactional(MANDATORY)
    public void refreshZoneEndpointsAfterUpdate(Zone zone, ZoneResource zoneResource) throws WebClientRequestException {
        zone.clearEndpointsCollection();

        StreamSupport.stream(zoneResource.getEndpoints().spliterator(), false)
            .map(this::getEndpoint)
            .filter(Objects::nonNull)
            .forEach(zone::addEndpoint);
    }

    public ZoneResource retrieveZone(URI zoneUri) throws WebClientRequestException {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            return (ZoneResource) webClient.get(zoneUri);
        }
    }

    public Optional<Zone> getZoneFromEndpoint(ODataId endpoint) {
        return genericDao.tryFind(Endpoint.class, endpoint).map(Endpoint::getZone);
    }

    private Endpoint getEndpoint(ResourceSupplier endpointResourceSupplier) {
        try {
            ExternalServiceResource externalResource = endpointResourceSupplier.get();
            ODataId endpointOdataId = oDataIdFromUri(externalResource.getUri());
            Optional<Endpoint> possibleEndpoint = endpointDao.tryFind(endpointOdataId);

            if (!possibleEndpoint.isPresent()) {
                log.error("Endpoint(id:{}) doesn't exist", endpointOdataId);
                return null;
            }
            return possibleEndpoint.get();
        } catch (WebClientRequestException e) {
            log.error(format("Error while retrieving endpoint resource supplier: %s", e.getMessage()), e);
            return null;
        }
    }
}
