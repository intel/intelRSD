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

import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.discovery.external.EntityMultiMapper;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ConnectedEntityResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.EndpointResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.EthernetInterfaceResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.IpTransportDetailsResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.PortResource;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetInterface;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.IpTransportDetails;
import com.intel.rsd.nodecomposer.persistence.redfish.Port;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import static java.lang.String.format;
import static org.springframework.transaction.annotation.Propagation.MANDATORY;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
@Service
public class EndpointSynchronizationService {

    private final GenericDao genericDao;

    private final WebClientBuilder webClientBuilder;

    private final EntityMultiMapper entityMultiMapper;

    @Autowired
    public EndpointSynchronizationService(GenericDao genericDao, WebClientBuilder webClientBuilder, EntityMultiMapper entityMultiMapper) {
        this.genericDao = genericDao;
        this.webClientBuilder = webClientBuilder;
        this.entityMultiMapper = entityMultiMapper;
    }

    @Transactional(propagation = MANDATORY)
    public Endpoint discoverEndpoint(ODataId fabricOdataId, ODataId endpointOdataId) throws EntityOperationException {
        try {
            val fabric = genericDao.find(Fabric.class, fabricOdataId);
            try (val webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
                val endpointResource = (EndpointResource) webClient.get(endpointOdataId.toUri());
                val endpoint = (Endpoint) entityMultiMapper.map(endpointResource);
                fabric.addEndpoint(endpoint);
                discoverConnectedEntities(endpoint, endpointResource);
                discoverTransports(endpoint, endpointResource);
                discoverInterface(endpointResource, endpoint);
                discoverLinks(endpointResource, endpoint);
                return endpoint;
            }
        } catch (WebClientRequestException e) {
            throw new EntityOperationException(format("Discovery of Endpoint(%s)", endpointOdataId), e);
        }
    }

    private void discoverConnectedEntities(Endpoint endpoint, EndpointResource endpointResource) throws WebClientRequestException {
        for (ResourceSupplier connectedEntitySupplier : endpointResource.getConnectedEntities()) {
            val connectedEntityResource = (ConnectedEntityResource) connectedEntitySupplier.get();

            val connectedEntity = (ConnectedEntity) entityMultiMapper.map(connectedEntityResource);
            endpoint.addConnectedEntity(connectedEntity);

            val entityLink = connectedEntityResource.getEntityLink();
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

}
