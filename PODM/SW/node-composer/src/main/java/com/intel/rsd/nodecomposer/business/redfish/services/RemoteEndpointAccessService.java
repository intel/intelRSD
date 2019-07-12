/*
 * Copyright (c) 2019 Intel Corporation
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
import com.intel.rsd.nodecomposer.business.redfish.services.actions.RestRequestInvoker;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.InitiatorEndpointCreateRequestBuilder;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.actions.EndpointCreationRequest;
import com.intel.rsd.nodecomposer.externalservices.resources.MembersListResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.EndpointResource;
import com.intel.rsd.nodecomposer.types.EntityRole;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.modelmapper.MappingException;
import org.modelmapper.ModelMapper;
import org.modelmapper.spi.ErrorMessage;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import javax.ws.rs.core.UriBuilder;
import java.net.URI;
import java.util.List;
import java.util.Optional;
import java.util.UUID;

import static com.intel.rsd.nodecomposer.business.redfish.services.helpers.TargetEndpointCreateRequestBuilder.buildTargetEndpointCreationRequest;
import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromUri;
import static com.intel.rsd.nodecomposer.types.EntityRole.INITIATOR;
import static com.intel.rsd.nodecomposer.types.EntityRole.TARGET;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.ENDPOINTS_RESOURCE_NAME;
import static java.lang.String.format;
import static java.util.Optional.empty;
import static java.util.Optional.ofNullable;
import static org.apache.commons.lang3.exception.ExceptionUtils.getRootCause;

@Service
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
@Slf4j
public class RemoteEndpointAccessService {

    private final ModelMapper modelMapper;

    private final WebClientBuilder webClientBuilder;

    private final RestRequestInvoker restRequestInvoker;

    private final InitiatorEndpointCreateRequestBuilder initiatorEndpointCreateRequestBuilder;

    @Autowired
    public RemoteEndpointAccessService(ModelMapper modelMapper,
                                       WebClientBuilder webClientBuilder,
                                       RestRequestInvoker restRequestInvoker,
                                       InitiatorEndpointCreateRequestBuilder initiatorEndpointCreateRequestBuilder) {

        this.modelMapper = modelMapper;
        this.webClientBuilder = webClientBuilder;
        this.restRequestInvoker = restRequestInvoker;
        this.initiatorEndpointCreateRequestBuilder = initiatorEndpointCreateRequestBuilder;
    }

    public ODataId createTargetEndpoint(ODataId fabricOdataId, Protocol fabricType, ODataId targetOdataId) throws BusinessApiException {
        val endpointDto = buildTargetEndpointCreationRequest(fabricType, targetOdataId);
        return createEndpointWithRetry(fabricOdataId, endpointDto);
    }

    public ODataId createInitiatorEndpoint(ODataId fabricOdataId, Protocol fabricType,
                                           UUID targetSystemUuid, List<String> targetSystemConnectionIds) throws BusinessApiException {

        val endpointDto = buildInitiatorEndpointCreationRequest(fabricType, targetSystemUuid, targetSystemConnectionIds);
        return createEndpointWithRetry(fabricOdataId, endpointDto);
    }

    public URI getInitiatorEndpointUriBySystemUuid(ODataId fabricOdataId, UUID uuid) throws EntityOperationException {
        val endpointsCollectionUri = UriBuilder.fromUri(fabricOdataId.toUri()).path(ENDPOINTS_RESOURCE_NAME).build();
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            return tryFindInitiatorEndpoint(webClient, endpointsCollectionUri, uuid).orElseThrow(() -> {
                String errorMessage = format("Cannot find initiator endpoint in collection [ collection: %s, nqn: %s ]", endpointsCollectionUri, uuid);
                log.error(errorMessage);
                return new EntityOperationException(errorMessage);
            });
        } catch (WebClientRequestException e) {
            String errorMessage = format("Error wile retrieve endpoints collection [ collection: %s, nqn: %s ]", endpointsCollectionUri, uuid);
            log.error(errorMessage);
            throw new EntityOperationException(errorMessage, e);
        }
    }

    public URI getTargetEndpointUriByRelatedAssetUri(ODataId fabricOdataId, ODataId relatedAssetUri) throws EntityOperationException {
        val endpointsUri = UriBuilder.fromUri(fabricOdataId.toUri()).path(ENDPOINTS_RESOURCE_NAME).build();
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            return tryFindTargetEndpoint(webClient, endpointsUri, relatedAssetUri).orElseThrow(() -> {
                String errorMessage = format("Cannot find target endpoint [%s] with related asset %s", endpointsUri, relatedAssetUri);
                log.error(errorMessage);
                return new EntityOperationException(errorMessage);
            });
        } catch (WebClientRequestException e) {
            String errorMessage = format("Error wile retrieve endpoint [%s] for asset %s", endpointsUri, relatedAssetUri);
            log.error(errorMessage);
            throw new EntityOperationException(errorMessage, e);
        }
    }

    private ODataId createEndpointWithRetry(ODataId fabricOdataId, EndpointDto representation) throws BusinessApiException {
        val endpointsCollectionUri = UriBuilder.fromUri(fabricOdataId.toUri()).path(ENDPOINTS_RESOURCE_NAME).build();
        val endpointCreationRequest = createEndpointCreationRequest(representation);
        return oDataIdFromUri(restRequestInvoker.postWithRetry(endpointsCollectionUri, endpointCreationRequest));
    }

    private EndpointCreationRequest createEndpointCreationRequest(EndpointDto representation) throws ODataIdResolvingException {
        try {
            return modelMapper.map(representation, EndpointCreationRequest.class);
        } catch (MappingException e) {
            for (ErrorMessage errorMessage : e.getErrorMessages()) {
                val rootCause = getRootCause(errorMessage.getCause());
                if (rootCause instanceof ODataIdResolvingException) {
                    throw (ODataIdResolvingException) rootCause;
                }
            }
            throw e;
        }
    }

    private EndpointDto buildInitiatorEndpointCreationRequest(Protocol fabricType, UUID targetSystemUuid, List<String> targetSystemConnectionIds) {
        return initiatorEndpointCreateRequestBuilder.buildInitiatorEndpointCreationRequest(fabricType, targetSystemUuid, targetSystemConnectionIds);
    }

    private Optional<URI> tryFindInitiatorEndpoint(WebClient webClient, URI endpointsUri, UUID uuid) throws WebClientRequestException {
        MembersListResource membersListResource = (MembersListResource) webClient.get(endpointsUri);
        for (ODataId oDataId : membersListResource.getMembers()) {
            val endpoint = (EndpointResource) webClient.get(oDataId.toUri());
            if (isEndpointOfType(endpoint, INITIATOR) && containsUuid(endpoint, uuid)) {
                return ofNullable(endpoint.getUri());
            }
        }
        return empty();
    }

    private Optional<URI> tryFindTargetEndpoint(WebClient webClient, URI endpointsUri, ODataId relatedAssetUri) throws WebClientRequestException {
        MembersListResource membersListResource = (MembersListResource) webClient.get(endpointsUri);
        for (ODataId oDataId : membersListResource.getMembers()) {
            val endpoint = (EndpointResource) webClient.get(oDataId.toUri());
            if (isEndpointOfType(endpoint, TARGET) && containsRelatedAssetInConnectedEntity(endpoint, relatedAssetUri)) {
                return ofNullable(endpoint.getUri());
            }
        }
        return empty();
    }

    private boolean containsUuid(EndpointResource endpoint, UUID uuid) {
        return endpoint.getIdentifiers().stream()
            .anyMatch(identifier -> identifier.getDurableName().contains(uuid.toString()));
    }

    private boolean isEndpointOfType(EndpointResource endpointResource, EntityRole entityRole) {
        return endpointResource.getConnectedEntityResourceList().stream().anyMatch(connectedEntity -> entityRole.equals(connectedEntity.getEntityRole()));
    }

    private boolean containsRelatedAssetInConnectedEntity(EndpointResource endpoint, ODataId relatedAssetUri) {
        return endpoint.getConnectedEntityResourceList().stream()
            .anyMatch(connectedEntityResource -> connectedEntityResource.getEntityLink().getUri().equals(relatedAssetUri.toUri()));
    }
}
