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

package com.intel.podm.discovery.external.partial;

import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.IpTransportDetails;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.redfish.ConnectedEntityResource;
import com.intel.podm.client.resources.redfish.EndpointResource;
import com.intel.podm.client.resources.redfish.EthernetInterfaceResource;
import com.intel.podm.client.resources.redfish.IpTransportDetailsResource;
import com.intel.podm.discovery.external.EntityMultiMapper;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Objects;
import java.util.function.Predicate;

import static com.intel.podm.common.types.EntityRole.INITIATOR;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EndpointObtainer {

    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private EntityMultiMapper entityMultiMapper;

    @Transactional(MANDATORY)
    public Endpoint discoverEndpoint(ExternalService externalService, Fabric fabric, URI endpointUri) throws WebClientRequestException {
        try (WebClient webClient = webClientBuilder.newInstance(externalService.getBaseUri()).retryable().build()) {
            EndpointResource endpointResource = (EndpointResource) webClient.get(endpointUri);
            Endpoint endpoint = (Endpoint) entityMultiMapper.map(endpointResource, externalService);
            fabric.addEndpoint(endpoint);

            discoverConnectedEntities(externalService, endpoint, endpointResource);
            discoverTransports(externalService, endpoint, endpointResource);
            discoverInterface(externalService, endpointResource, endpoint);
            return endpoint;
        }
    }

    public Endpoint getInitiatorEndpoint(ComputerSystem computerSystem, Fabric fabric) {
        Predicate<Endpoint> endpointPredicate = endpoint -> Objects.equals(endpoint.getFabric(), fabric);

        return getInitiatorEndpoint(computerSystem, endpointPredicate);
    }

    public Endpoint getInitiatorEndpoint(ComputerSystem computerSystem, ExternalService externalService) {
        Predicate<Endpoint> endpointPredicate = endpoint -> Objects.equals(endpoint.getService(), externalService);

        return getInitiatorEndpoint(computerSystem, endpointPredicate);
    }

    private Endpoint getInitiatorEndpoint(ComputerSystem computerSystem, Predicate<Endpoint> endpointPredicate) {
        return computerSystem.getEndpoints().stream()
            .filter(endpointPredicate)
            .filter(endpoint -> endpoint.hasRole(INITIATOR))
            .findFirst()
            .orElse(null);
    }

    private void discoverConnectedEntities(ExternalService service, Endpoint endpoint, EndpointResource endpointResource) throws WebClientRequestException {
        for (ResourceSupplier connectedEntitySupplier : endpointResource.getConnectedEntities()) {
            ConnectedEntityResource connectedEntityResource = (ConnectedEntityResource) connectedEntitySupplier.get();

            ConnectedEntity connectedEntity = (ConnectedEntity) entityMultiMapper.map(connectedEntityResource, service);
            endpoint.addConnectedEntity(connectedEntity);

            ResourceSupplier entityLink = connectedEntityResource.getEntityLink();
            if (entityLink != null) {
                DiscoverableEntity discoverableEntity = entityMultiMapper.map(entityLink.get(), service);
                connectedEntity.setEntityLink(discoverableEntity);
            }
        }
    }

    private void discoverTransports(ExternalService service, Endpoint endpoint, EndpointResource resource) throws WebClientRequestException {
        for (ResourceSupplier supplier : resource.getTransports()) {
            IpTransportDetailsResource transportResource = (IpTransportDetailsResource) supplier.get();
            IpTransportDetails transport = (IpTransportDetails) entityMultiMapper.map(transportResource, service);
            endpoint.addTransport(transport);
        }
    }

    private void discoverInterface(ExternalService service, EndpointResource resource, Endpoint endpoint) throws WebClientRequestException {
        for (ResourceSupplier interfaceSupplier : resource.getInterfaces()) {
            EthernetInterfaceResource ethInterfaceResource = (EthernetInterfaceResource) interfaceSupplier.get();
            EthernetInterface ethInterface = (EthernetInterface) entityMultiMapper.map(ethInterfaceResource, service);
            endpoint.addEthernetInterface(ethInterface);
        }
    }
}
