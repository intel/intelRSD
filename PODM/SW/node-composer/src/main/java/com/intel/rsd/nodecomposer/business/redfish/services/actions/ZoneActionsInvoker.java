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

package com.intel.rsd.nodecomposer.business.redfish.services.actions;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.discovery.external.partial.ZoneObtainer;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.actions.ZoneActionJson;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ZoneResource;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Zone;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import lombok.SneakyThrows;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.net.URI;
import java.util.Set;

import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromUri;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.ZONES_RESOURCE_NAME;
import static java.util.stream.Collectors.toSet;
import static java.util.stream.Stream.of;
import static java.util.stream.StreamSupport.stream;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;
import static org.springframework.web.util.UriComponentsBuilder.fromUri;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ZoneActionsInvoker {
    private final WebClientBuilder webClientBuilder;
    private final ZoneObtainer zoneObtainer;
    private final RestRequestInvoker restRequestInvoker;

    @Autowired
    public ZoneActionsInvoker(WebClientBuilder webClientBuilder, ZoneObtainer zoneObtainer, RestRequestInvoker restRequestInvoker) {
        this.webClientBuilder = webClientBuilder;
        this.zoneObtainer = zoneObtainer;
        this.restRequestInvoker = restRequestInvoker;
    }

    @Transactional(MANDATORY)
    public void updateZone(Zone zone, Set<Endpoint> newEndpointsCollection) throws BusinessApiException {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            ZoneResource zoneResource = performUpdateZoneAction(webClient, newEndpointsCollection, zone);
            refreshZoneEndpointsAfterUpdate(zone, zoneResource);
        }
    }

    private ZoneResource performUpdateZoneAction(WebClient webClient, Set<Endpoint> newEndpointsCollection, Zone zone)
        throws BusinessApiException {

        URI zoneUri = zone.getUri().toUri();
        try {
            return webClient.patchAndRetrieve(zoneUri, new ZoneActionJson(collectEndpointUris(newEndpointsCollection)));
        } catch (WebClientRequestException e) {
            String errorMessage = "Update on selected Zone failed";
            log.warn(errorMessage + " on [ path: {} ]", zoneUri);
            throw new BusinessApiException(errorMessage, e);
        }
    }

    private Set<URI> collectEndpointUris(Set<Endpoint> endpoints) {
        return endpoints.stream()
            .map(DiscoverableEntity::getUri)
            .map(ODataId::toUri)
            .collect(toSet());
    }

    private void refreshZoneEndpointsAfterUpdate(Zone zone, ZoneResource zoneResource) throws BusinessApiException {
        try {
            zoneObtainer.refreshZoneEndpointsAfterUpdate(zone, zoneResource);
        } catch (WebClientRequestException e) {
            String errorMessage = "Update action was successful, but refreshing selected Zone failed";
            log.info(errorMessage + " on [ path: {} ]", zone.getUri());
            throw new BusinessApiException(errorMessage, e);
        }
    }

    public ODataId updateZone(ODataId zoneOdataId, ODataId targetEndpointOdataId) throws BusinessApiException {
        val requestUri = zoneOdataId.toUri();
        val zoneEndpointsUriCollection = getEndpointsFromZoneUri(requestUri);
        zoneEndpointsUriCollection.add(targetEndpointOdataId.toUri());
        val zoneUpdateRequest = new ZoneActionJson(zoneEndpointsUriCollection);
        restRequestInvoker.patchWithRetry(requestUri, zoneUpdateRequest);
        return zoneOdataId;
    }

    public ODataId createZone(ODataId affectedFabricUri, ODataId initiatorEndpointOdataId, ODataId targetEndpointOdataId) throws EntityOperationException {
        val zoneCollectionUri = fromUri(affectedFabricUri.toUri()).pathSegment(ZONES_RESOURCE_NAME).build().toUri();
        val zoneCreationRequest = new ZoneActionJson(of(initiatorEndpointOdataId, targetEndpointOdataId).map(ODataId::toUri).collect(toSet()));
        // TODO RSD-4696 try update when zone is on SS but not in DB
        val createdZoneOdataId = restRequestInvoker.postWithRetry(zoneCollectionUri, zoneCreationRequest);
        return oDataIdFromUri(createdZoneOdataId);
    }

    private Set<URI> getEndpointsFromZoneUri(URI zoneUri) throws BusinessApiException {
        try {
            val zone = zoneObtainer.retrieveZone(zoneUri);
            return stream(zone.getEndpoints().spliterator(), false).map(ResourceSupplier::getUri).collect(toSet());
        } catch (WebClientRequestException e) {
            String errorMessage = "Obtain endpoints from zone failed";
            log.info(errorMessage + " on [ get: {} ]", zoneUri);
            throw new BusinessApiException(errorMessage, e);
        }
    }

    @SneakyThrows
    public void removeFromZone(ODataId targetEndpointOdataId, ODataId zoneOdataId) {
        val zone = zoneObtainer.retrieveZone(zoneOdataId.toUri());
        val existingEndpoints = zone.getEndpoints();
        val zoneEndpointsUriCollection = stream(existingEndpoints.spliterator(), false).map(ResourceSupplier::getUri).collect(toSet());
        zoneEndpointsUriCollection.remove(targetEndpointOdataId.toUri());
        restRequestInvoker.patchWithRetry(zoneOdataId.toUri(), new ZoneActionJson(zoneEndpointsUriCollection));
    }
}
