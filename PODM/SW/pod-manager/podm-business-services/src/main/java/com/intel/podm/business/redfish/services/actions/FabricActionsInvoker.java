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

package com.intel.podm.business.redfish.services.actions;

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.ZoneActionJson;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.ZoneActionRequest;
import com.intel.podm.discovery.external.partial.ZoneObtainer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class FabricActionsInvoker {
    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private Logger logger;

    @Inject
    private ZoneObtainer zoneObtainer;

    @Transactional(MANDATORY)
    public Zone createZone(Fabric associatedFabric, ZoneActionRequest zoneActionRequest) throws EntityOperationException {
        ExternalService service = associatedFabric.getService();
        requiresNonNull(service, "service", "There is no Service associated with selected Fabric");

        URI fabricSourceUri = associatedFabric.getSourceUri();
        URI zoneUri = performCreateZoneAction(
            new ZoneActionJson(zoneActionRequest.getLinks().getEndpoints()),
            service,
            fabricSourceUri);
        try {
            return zoneObtainer.discoverZone(service, associatedFabric, zoneUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "Zone refreshing failed on selected Fabric";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), zoneUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }

    private URI performCreateZoneAction(ZoneActionJson zoneActionJson, ExternalService service, URI fabricUri) throws EntityOperationException {
        URI ruleUri;
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            URI zonesCollectionUri = URI.create(fabricUri + "/Zones");
            ruleUri = webClient.post(zonesCollectionUri, zoneActionJson);
        } catch (WebClientRequestException e) {
            String errorMessage = "Zone creation failed on selected Fabric";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), fabricUri);
            throw new EntityOperationException(errorMessage, e);
        }
        return ruleUri;
    }
}
