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

package com.intel.rsd.nodecomposer.business.redfish.services;

import com.hazelcast.spring.context.SpringAware;
import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.dto.EndpointDto;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.InitiatorEndpointCreateRequestBuilder;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;

import static com.intel.rsd.nodecomposer.business.redfish.services.helpers.TargetEndpointCreateRequestBuilder.buildTargetEndpointCreationRequest;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Slf4j
@SpringAware
@Component
public class EndpointService {
    private final GenericDao genericDao;
    private final EndpointCreationService endpointCreationService;
    private final InitiatorEndpointCreateRequestBuilder initiatorEndpointCreateRequestBuilder;

    @Autowired
    public EndpointService(GenericDao genericDao, EndpointCreationService endpointCreationService,
                           InitiatorEndpointCreateRequestBuilder initiatorEndpointCreateRequestBuilder) {
        this.genericDao = genericDao;
        this.endpointCreationService = endpointCreationService;
        this.initiatorEndpointCreateRequestBuilder = initiatorEndpointCreateRequestBuilder;
    }

    @Transactional(MANDATORY)
    public void createInitiatorEndpoint(ComposedNode composedNode, Fabric fabric) {
        ComputerSystem computerSystem = composedNode.getComputerSystem();
        EndpointDto initiatorEndpointDto = initiatorEndpointCreateRequestBuilder.buildInitiatorEndpointCreationRequest(fabric.getFabricType(), computerSystem);

        Endpoint endpoint = createEndpoint(composedNode.getUri(), fabric.getUri(), initiatorEndpointDto);
        computerSystem.addEndpoint(endpoint);
        updateEntityLink(composedNode, endpoint);
    }

    @Transactional(MANDATORY)
    public void createTargetEndpoint(ODataId resourceUri, ComposedNode composedNode, Fabric fabric) {
        EndpointDto endpointDto = buildTargetEndpointCreationRequest(fabric, resourceUri);
        Endpoint endpoint = createEndpoint(composedNode.getUri(), fabric.getUri(), endpointDto);
        composedNode.addEndpoint(endpoint);
    }

    private void updateEntityLink(ComposedNode composedNode, Endpoint endpoint) {
        endpoint.getConnectedEntities()
            .forEach(connectedEntity -> connectedEntity.setEntityLink(composedNode.getComputerSystem()));
    }

    private Endpoint createEndpoint(ODataId composedNodeODataId, ODataId fabricUri, EndpointDto endpointDto) {
        try {
            ODataId endpointODataId = endpointCreationService.createAndDiscoverEndpoint(fabricUri, endpointDto);
            Endpoint endpoint = genericDao.find(Endpoint.class, endpointODataId);
            endpoint.setAllocated(true);

            return endpoint;
        } catch (BusinessApiException e) {
            log.error("Endpoint creation failed for ComposedNode: {}, details: {}", composedNodeODataId, e.getMessage());
            throw new RuntimeException(e);
        }
    }
}
