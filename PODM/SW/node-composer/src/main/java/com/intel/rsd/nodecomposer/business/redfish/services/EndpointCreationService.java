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

package com.intel.rsd.nodecomposer.business.redfish.services;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.ODataIdResolvingException;
import com.intel.rsd.nodecomposer.business.dto.EndpointDto;
import com.intel.rsd.nodecomposer.business.redfish.EntityTreeTraverser;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.RestRequestInvoker;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.discovery.external.partial.EndpointObtainer;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.actions.EndpointCreationRequest;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.modelmapper.MappingException;
import org.modelmapper.ModelMapper;
import org.modelmapper.spi.ErrorMessage;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.annotation.PostConstruct;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Iterator;

import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromUri;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.ENDPOINTS_RESOURCE_NAME;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.apache.commons.lang3.exception.ExceptionUtils.getRootCause;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;
import static org.springframework.web.util.UriComponentsBuilder.fromUri;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EndpointCreationService {
    private final EntityTreeTraverser entityTreeTraverser;
    private final RestRequestInvoker restRequestInvoker;
    private final EndpointObtainer endpointObtainer;

    private ModelMapper mapper;

    @Autowired
    public EndpointCreationService(EntityTreeTraverser entityTreeTraverser, RestRequestInvoker restRequestInvoker, EndpointObtainer endpointObtainer) {
        this.entityTreeTraverser = entityTreeTraverser;
        this.restRequestInvoker = restRequestInvoker;
        this.endpointObtainer = endpointObtainer;
    }

    @PostConstruct
    private void init() {
        this.mapper = new ModelMapper();
    }

    @Transactional(MANDATORY)
    public ODataId createAndDiscoverEndpoint(ODataId fabricODataId, EndpointDto representation) throws BusinessApiException {
        Fabric fabric = (Fabric) entityTreeTraverser.traverseDiscoverableEntity(fabricODataId);
        return createAndDiscoverEndpoint(fabric, representation);
    }

    private ODataId createAndDiscoverEndpoint(Fabric fabric, EndpointDto representation) throws BusinessApiException {
        EndpointCreationRequest endpointCreationRequest = createEndpointCreationRequest(representation);
        URI locationOfCreatedEndpoint = sendCreateEndpointRequest(endpointCreationRequest, fabric);

        return discoverCreatedEndpoint(fabric, locationOfCreatedEndpoint);
    }

    public ODataId createEndpointWithRetry(URI endpointsCollectionUri, EndpointDto representation) throws BusinessApiException {
        val endpointCreationRequest = createEndpointCreationRequest(representation);
        return oDataIdFromUri(restRequestInvoker.postWithRetry(endpointsCollectionUri, endpointCreationRequest));
    }

    private ODataId discoverCreatedEndpoint(Fabric fabric, URI endpointUri) throws EntityOperationException {
        try {
            return endpointObtainer.discoverEndpoint(fabric, endpointUri).getUri();
        } catch (WebClientRequestException e) {
            String errorMessage = "Discovery of created Endpoint failed";
            log.warn(errorMessage + " on [ path: {} ]", endpointUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }

    private EndpointCreationRequest createEndpointCreationRequest(EndpointDto representation) throws ODataIdResolvingException {
        try {
            return mapper.map(representation, EndpointCreationRequest.class);
        } catch (MappingException e) {
            Iterator<ErrorMessage> iterator = e.getErrorMessages().iterator();
            if (iterator.hasNext()) {
                ErrorMessage error = iterator.next();
                Throwable rootCause = getRootCause(error.getCause());
                if (rootCause instanceof ODataIdResolvingException) {
                    throw (ODataIdResolvingException) rootCause;
                }
            }
            throw e;
        }
    }

    private URI sendCreateEndpointRequest(EndpointCreationRequest request, Fabric fabric) throws BusinessApiException {
        URI endpointsCollectionUri = fromUri(fabric.getUri().toUri()).pathSegment(ENDPOINTS_RESOURCE_NAME).build().toUri();
        return restRequestInvoker.post(endpointsCollectionUri, request);
    }
}
