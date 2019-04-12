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

package com.intel.rsd.nodecomposer.business.redfish.services.helpers;

import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.redfish.services.EndpointCreationService;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.discovery.external.partial.EndpointObtainer;
import com.intel.rsd.nodecomposer.persistence.dao.DriveDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.SneakyThrows;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.net.URI;
import java.util.List;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromUri;
import static com.intel.rsd.nodecomposer.externalservices.WebClientExceptionUtils.badRequestInTheRootCause;
import static com.intel.rsd.nodecomposer.types.Protocol.NVME;
import static com.intel.rsd.nodecomposer.types.Protocol.PCIE;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.ENDPOINTS_RESOURCE_NAME;
import static java.util.Arrays.asList;
import static javax.ws.rs.core.UriBuilder.fromUri;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
@Slf4j
public class InitiatorEndpointFinder {
    public static final List<Protocol> PNC_FABRIC_TYPES = asList(NVME, PCIE);

    private final EndpointObtainer endpointObtainer;
    private final EndpointCreationService endpointCreationService;
    private final InitiatorEndpointCreateRequestBuilder initiatorEndpointCreateRequestBuilder;
    private final DriveDao driveDao;

    @Autowired
    public InitiatorEndpointFinder(EndpointObtainer endpointObtainer, EndpointCreationService endpointCreationService, DriveDao driveDao,
                                   InitiatorEndpointCreateRequestBuilder initiatorEndpointCreateRequestBuilder) {
        this.endpointObtainer = endpointObtainer;
        this.endpointCreationService = endpointCreationService;
        this.initiatorEndpointCreateRequestBuilder = initiatorEndpointCreateRequestBuilder;
        this.driveDao = driveDao;
    }

    public ODataId findOrCreateInitiatorEndpoint(ComposedNode composedNode, ODataId fabricODataId, Protocol fabricType) {
        val initiatorEndpoint = Optional.ofNullable(PNC_FABRIC_TYPES.contains(fabricType)
            ? driveDao.getConnectedInitiatorEndpoint(composedNode.getComputerSystem())
            : endpointObtainer.getInitiatorEndpoint(composedNode.getComputerSystem(), fabricODataId));

        return initiatorEndpoint.map(DiscoverableEntity::getUri)
            .orElseGet(() -> createOrObtainEndpoint(composedNode, fabricODataId, fabricType));
    }

    @SneakyThrows
    private ODataId createOrObtainEndpoint(ComposedNode composedNode, ODataId fabricODataId, Protocol fabricType) {
        val endpointsCollectionUri = fromUri(fabricODataId.toUri()).path(ENDPOINTS_RESOURCE_NAME).build();
        val endpointCreationRequest = initiatorEndpointCreateRequestBuilder.buildInitiatorEndpointCreationRequest(fabricType, composedNode.getComputerSystem());
        try {
            return endpointCreationService.createEndpointWithRetry(endpointsCollectionUri, endpointCreationRequest);
        } catch (EntityOperationException e) {
            return tryToFindInitiatorEndpointOnBadRequest(composedNode, endpointsCollectionUri, e);
        }
    }

    private ODataId tryToFindInitiatorEndpointOnBadRequest(ComposedNode composedNode, URI endpointsCollectionUri, EntityOperationException e)
        throws EntityOperationException {
        if (badRequestInTheRootCause(e)) {
            return oDataIdFromUri(endpointObtainer.getInitiatorEndpointUriBySystemUuid(endpointsCollectionUri,
                composedNode.getAssociatedComputerSystemUuid().toString()));
        }
        log.warn("Cannot find initiator endpoint, while creating initiator endpoint return bad request");
        throw new EntityOperationException(e.getMessage(), e);
    }
}
