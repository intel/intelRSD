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

package com.intel.rsd.nodecomposer.business.redfish.services.actions;

import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.discovery.external.partial.ZoneObtainer;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.actions.ZoneActionJson;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.Zone;
import com.intel.rsd.nodecomposer.types.actions.ZoneActionRequest;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.net.URI;

import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class FabricActionsInvoker {
    private final WebClientBuilder webClientBuilder;
    private final ZoneObtainer zoneObtainer;

    @Autowired
    public FabricActionsInvoker(WebClientBuilder webClientBuilder, ZoneObtainer zoneObtainer) {
        this.webClientBuilder = webClientBuilder;
        this.zoneObtainer = zoneObtainer;
    }

    @Transactional(MANDATORY)
    public Zone createZone(Fabric associatedFabric, ZoneActionRequest zoneActionRequest) throws EntityOperationException {
        URI fabricSourceUri = associatedFabric.getUri().toUri();
        URI zoneUri = performCreateZoneAction(
            new ZoneActionJson(zoneActionRequest.getLinks().getEndpoints()),
            fabricSourceUri);
        try {
            return zoneObtainer.discoverZone(associatedFabric, zoneUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "Zone refreshing failed on selected Fabric";
            log.warn(errorMessage + " on [ path: {} ]", zoneUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }

    private URI performCreateZoneAction(ZoneActionJson zoneActionJson, URI fabricUri) throws EntityOperationException {
        URI ruleUri;
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().build()) {
            URI zonesCollectionUri = URI.create(fabricUri + "/Zones");
            ruleUri = webClient.post(zonesCollectionUri, zoneActionJson);
        } catch (WebClientRequestException e) {
            String errorMessage = "Zone creation failed on selected Fabric";
            log.warn(errorMessage + " on [ path: {} ]", fabricUri);
            throw new EntityOperationException(errorMessage, e);
        }
        return ruleUri;
    }
}
