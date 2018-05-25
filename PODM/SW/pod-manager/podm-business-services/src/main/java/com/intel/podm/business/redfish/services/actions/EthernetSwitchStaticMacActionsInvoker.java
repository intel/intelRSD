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
import com.intel.podm.business.entities.dao.DiscoverableEntityDao;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchStaticMac;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.EthernetSwitchStaticMacRequest;
import com.intel.podm.client.resources.redfish.EthernetSwitchStaticMacResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchStaticMac;
import com.intel.podm.discovery.external.partial.EthernetSwitchStaticMacObtainer;
import com.intel.podm.mappers.redfish.EthernetSwitchStaticMacMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import javax.ws.rs.core.UriBuilder;
import java.net.URI;

import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_SWITCH_STATIC_MACS_RESOURCE_NAME;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class EthernetSwitchStaticMacActionsInvoker {
    @Inject
    private Logger logger;

    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private EthernetSwitchStaticMacObtainer obtainer;

    @Inject
    private EthernetSwitchStaticMacMapper mapper;

    @Inject
    private DiscoverableEntityDao discoverableEntityDao;

    @Transactional(MANDATORY)
    public EthernetSwitchStaticMac createStaticMac(EthernetSwitchPort currentPort, RedfishEthernetSwitchStaticMac inboundRequest)
        throws EntityOperationException {
        ExternalService service = currentPort.getService();
        requiresNonNull(service, "service", "There is no Service associated with selected port");

        URI staticMacsCollectionUri = UriBuilder.fromUri(currentPort.getSourceUri())
            .path(ETHERNET_SWITCH_STATIC_MACS_RESOURCE_NAME)
            .build();

        URI newStaticMacUri = performCreateAction(service, convertInboundRequestToOutboundRequest(inboundRequest), staticMacsCollectionUri);

        EthernetSwitchStaticMac staticMac = obtainEntity(service, newStaticMacUri);

        currentPort.addEthernetSwitchStaticMac(staticMac);
        return staticMac;
    }

    private EthernetSwitchStaticMac obtainEntity(ExternalService service, URI uri) throws EntityOperationException {
        try {
            return obtainer.discover(service, uri);
        } catch (WebClientRequestException e) {
            String errorMessage = "EthernetSwitchStaticMac creation was successful, but failed on refreshing selected static MAC";
            logger.w("{} on [ service: {}, path: {} ]", errorMessage, service.getBaseUri(), e.getResourceUri());
            throw new EntityOperationException(errorMessage, e);
        }
    }

    private URI performCreateAction(ExternalService service, EthernetSwitchStaticMacRequest outboundRequest, URI collectionUri)
        throws EntityOperationException {
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            return webClient.post(collectionUri, outboundRequest);
        } catch (WebClientRequestException e) {
            String errorMessage = "EthernetSwitchStaticMac creation failed on selected port";
            logger.w("{} on [ service: {}, path: {} ]", errorMessage, service.getBaseUri(), collectionUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }

    private EthernetSwitchStaticMacRequest convertInboundRequestToOutboundRequest(RedfishEthernetSwitchStaticMac inboundRequest) {
        EthernetSwitchStaticMacRequest outboundRequest = new EthernetSwitchStaticMacRequest();
        outboundRequest.setMacAddress(inboundRequest.getMacAddress());
        outboundRequest.setVlanId(inboundRequest.getVlanId());
        return outboundRequest;
    }

    @Transactional(MANDATORY)
    public void delete(EthernetSwitchStaticMac ethernetSwitchStaticMac) throws EntityOperationException {
        ExternalService service = ethernetSwitchStaticMac.getService();
        requiresNonNull(service, "service", "There is no Service associated with selected static MAC");

        URI staticMacSourceUri = ethernetSwitchStaticMac.getSourceUri();
        URI serviceBaseUri = service.getBaseUri();

        try (WebClient webClient = webClientBuilder.newInstance(serviceBaseUri).retryable().build()) {
            webClient.delete(staticMacSourceUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "Selected EthernetSwitchStaticMac could not be deleted";
            logger.w("{} on [ service: {}, path: {} ]", errorMessage, serviceBaseUri, staticMacSourceUri);
            throw new EntityOperationException(errorMessage, e);
        }
        discoverableEntityDao.removeWithConnectedExternalLinks(ethernetSwitchStaticMac);
    }

    public void update(EthernetSwitchStaticMac staticMac, RedfishEthernetSwitchStaticMac inboundRequest) throws EntityOperationException {
        ExternalService service = staticMac.getService();
        requiresNonNull(service, "service", "There is no Service associated with selected static MAC");

        URI staticMacSourceUri = staticMac.getSourceUri();
        URI serviceBaseUri = service.getBaseUri();

        try (WebClient webClient = webClientBuilder.newInstance(serviceBaseUri).retryable().build()) {
            EthernetSwitchStaticMacResource resource = webClient.patchAndRetrieve(staticMacSourceUri, convertInboundRequestToOutboundRequest(inboundRequest));
            mapper.map(resource, staticMac);
        } catch (WebClientRequestException e) {
            String errorMessage = "Selected EthernetSwitchStaticMac could not be updated";
            logger.w("{} on [ service: {}, path: {} ]", errorMessage, serviceBaseUri, staticMacSourceUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }
}
