/*
 * Copyright (c) 2018-2019 Intel Corporation
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
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.SneakyThrows;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.net.URI;

import static com.intel.rsd.collections.IterableHelper.optionalSingle;
import static com.intel.rsd.nodecomposer.business.redfish.services.helpers.TargetEndpointCreateRequestBuilder.buildTargetEndpointCreationRequest;
import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromUri;
import static com.intel.rsd.nodecomposer.externalservices.WebClientExceptionUtils.badRequestInTheRootCause;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.ENDPOINTS_RESOURCE_NAME;
import static javax.ws.rs.core.UriBuilder.fromUri;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
@Slf4j
public class TargetEndpointFinder {
    private final EndpointCreationService endpointCreationService;
    private final EndpointObtainer endpointObtainer;

    @Autowired
    public TargetEndpointFinder(EndpointCreationService endpointCreationService, EndpointObtainer endpointObtainer) {
        this.endpointCreationService = endpointCreationService;
        this.endpointObtainer = endpointObtainer;
    }

    public <T extends DiscoverableEntity & AttachableAsset> ODataId findOrCreateTargetEndpoint(T assetToAttach, ODataId fabricODataId, Protocol fabricType) {
        val endpoint = optionalSingle(assetToAttach.getEndpoints()).map(DiscoverableEntity::getUri);
        return endpoint.orElseGet(() -> createOrObtainTargetEndpoint(fabricODataId, fabricType, assetToAttach.getUri()));
    }

    @SneakyThrows
    private ODataId createOrObtainTargetEndpoint(ODataId fabricODataId, Protocol fabricType, ODataId assetToAttachUri) {
        val endpointsCollectionUri = fromUri(fabricODataId.toUri()).path(ENDPOINTS_RESOURCE_NAME).build();
        val targetEndpointCreationRequest = buildTargetEndpointCreationRequest(fabricType, assetToAttachUri);
        try {
            return endpointCreationService.createEndpointWithRetry(endpointsCollectionUri, targetEndpointCreationRequest);
        } catch (EntityOperationException e) {
            return tryToFindTargetEndpointOnError(assetToAttachUri, endpointsCollectionUri, e);
        }
    }

    private ODataId tryToFindTargetEndpointOnError(ODataId assetToAttachUri, URI endpointsCollectionUri, EntityOperationException e)
        throws EntityOperationException {
        if (badRequestInTheRootCause(e)) {
            return oDataIdFromUri(endpointObtainer.getTargetEndpointUriByRelatedAssetUri(assetToAttachUri, endpointsCollectionUri));
        }
        log.warn("Cannot find existing target endpoint, received bad request when creating target endpoint.");
        throw new EntityOperationException(e.getMessage(), e);
    }
}
