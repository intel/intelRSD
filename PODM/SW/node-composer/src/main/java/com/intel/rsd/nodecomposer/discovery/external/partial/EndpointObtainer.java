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

import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.discovery.external.EntityMultiMapper;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.MembersListResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ConnectedEntityResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.EndpointResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.EthernetInterfaceResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.IpTransportDetailsResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.PortResource;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetInterface;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.IpTransportDetails;
import com.intel.rsd.nodecomposer.persistence.redfish.Port;
import com.intel.rsd.nodecomposer.persistence.redfish.StorageService;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.types.EntityRole;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.net.URI;
import java.util.Objects;
import java.util.Optional;
import java.util.function.Predicate;

import static com.intel.rsd.nodecomposer.types.EntityRole.INITIATOR;
import static com.intel.rsd.nodecomposer.types.EntityRole.TARGET;
import static java.lang.String.format;
import static java.util.Optional.empty;
import static java.util.Optional.ofNullable;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class EndpointObtainer {
    private final WebClientBuilder webClientBuilder;
    private final EntityMultiMapper entityMultiMapper;

    @Autowired
    public EndpointObtainer(WebClientBuilder webClientBuilder, EntityMultiMapper entityMultiMapper) {
        this.webClientBuilder = webClientBuilder;
        this.entityMultiMapper = entityMultiMapper;
    }

    @Transactional(MANDATORY)
    public Endpoint discoverEndpoint(Fabric fabric, URI endpointUri) throws WebClientRequestException {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            val endpointResource = (EndpointResource) webClient.get(endpointUri);
            val endpoint = (Endpoint) entityMultiMapper.map(endpointResource);
            fabric.addEndpoint(endpoint);

            discoverConnectedEntities(endpoint, endpointResource);
            discoverTransports(endpoint, endpointResource);
            discoverInterface(endpointResource, endpoint);
            discoverLinks(endpointResource, endpoint);

            return endpoint;
        }
    }

    public Endpoint getInitiatorEndpoint(ComputerSystem computerSystem, Fabric fabric) {
        return getInitiatorEndpoint(computerSystem, fabric.getUri());
    }

    public Endpoint getInitiatorEndpoint(ComputerSystem computerSystem, ODataId fabricODataId) {
        Predicate<Endpoint> endpointPredicate = endpoint -> Objects.equals(endpoint.getFabric().getUri(), fabricODataId);

        return getInitiatorEndpoint(computerSystem, endpointPredicate);
    }

    public Endpoint getInitiatorEndpoint(ComputerSystem computerSystem, StorageService storageService) {
        Predicate<Endpoint> endpointPredicate = endpoint -> Objects.equals(endpoint.getStorageService(), storageService);

        return getInitiatorEndpoint(computerSystem, endpointPredicate);
    }

    private Endpoint getInitiatorEndpoint(ComputerSystem computerSystem, Predicate<Endpoint> endpointPredicate) {
        return computerSystem.getEndpoints().stream()
            .filter(endpointPredicate)
            .filter(endpoint -> endpoint.hasRole(INITIATOR))
            .findFirst()
            .orElse(null);
    }

    public URI getInitiatorEndpointUriBySystemUuid(URI endpointsUri, String uuid) throws EntityOperationException {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            return tryFindInitiatorEndpoint(webClient, endpointsUri, uuid).orElseThrow(() -> {
                String errorMessage = format("Cannot find initiator endpoint in collection [ collection: %s, nqn: %s ]", endpointsUri, uuid);
                log.error(errorMessage);
                return new EntityOperationException(errorMessage);
            });
        } catch (WebClientRequestException e) {
            String errorMessage = format("Error wile retrieve endpoints collection [ collection: %s, nqn: %s ]", endpointsUri, uuid);
            log.error(errorMessage);
            throw new EntityOperationException(errorMessage, e);
        }
    }

    public URI getTargetEndpointUriByRelatedAssetUri(ODataId relatedAssetUri, URI endpointsUri) throws EntityOperationException {
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

    private void discoverConnectedEntities(Endpoint endpoint, EndpointResource endpointResource) throws WebClientRequestException {
        for (ResourceSupplier connectedEntitySupplier : endpointResource.getConnectedEntities()) {
            ConnectedEntityResource connectedEntityResource = (ConnectedEntityResource) connectedEntitySupplier.get();

            ConnectedEntity connectedEntity = (ConnectedEntity) entityMultiMapper.map(connectedEntityResource);
            endpoint.addConnectedEntity(connectedEntity);

            ResourceSupplier entityLink = connectedEntityResource.getEntityLink();
            if (entityLink != null) {
                DiscoverableEntity discoverableEntity = entityMultiMapper.map(entityLink.get());
                connectedEntity.setEntityLink(discoverableEntity);
            }
        }
    }

    private void discoverTransports(Endpoint endpoint, EndpointResource resource) throws WebClientRequestException {
        for (ResourceSupplier supplier : resource.getTransports()) {
            IpTransportDetailsResource transportResource = (IpTransportDetailsResource) supplier.get();
            IpTransportDetails transport = (IpTransportDetails) entityMultiMapper.map(transportResource);
            endpoint.addIpTransportDetails(transport);
        }
    }

    private void discoverLinks(EndpointResource resource, Endpoint endpoint) throws WebClientRequestException {
        for (ResourceSupplier portSupplier : resource.getPorts()) {
            PortResource portResource = (PortResource) portSupplier.get();
            Port port = (Port) entityMultiMapper.map(portResource);
            endpoint.addPort(port);
        }
    }

    private void discoverInterface(EndpointResource resource, Endpoint endpoint) throws WebClientRequestException {
        for (ResourceSupplier interfaceSupplier : resource.getInterfaces()) {
            EthernetInterfaceResource ethInterfaceResource = (EthernetInterfaceResource) interfaceSupplier.get();
            EthernetInterface ethInterface = (EthernetInterface) entityMultiMapper.map(ethInterfaceResource);
            endpoint.addEthernetInterface(ethInterface);
        }
    }

    private Optional<URI> tryFindInitiatorEndpoint(WebClient webClient, URI endpointsUri, String uuid) throws WebClientRequestException {
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

    private boolean containsUuid(EndpointResource endpoint, String uuid) {
        return endpoint.getIdentifiers().stream()
            .anyMatch(identifier -> identifier.getDurableName().contains(uuid));
    }

    private boolean isEndpointOfType(EndpointResource endpointResource, EntityRole entityRole) {
        return endpointResource.getConnectedEntityResourceList().stream().anyMatch(connectedEntity -> entityRole.equals(connectedEntity.getEntityRole()));
    }

    private boolean containsRelatedAssetInConnectedEntity(EndpointResource endpoint, ODataId relatedAssetUri) {
        return endpoint.getConnectedEntityResourceList().stream()
            .anyMatch(connectedEntityResource -> connectedEntityResource.getEntityLink().getUri().equals(relatedAssetUri.toUri()));
    }
}
