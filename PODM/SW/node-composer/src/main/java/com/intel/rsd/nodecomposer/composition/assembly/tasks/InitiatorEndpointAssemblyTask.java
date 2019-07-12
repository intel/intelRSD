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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.hazelcast.spring.context.SpringAware;
import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.redfish.services.EndpointService;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.annotation.PostConstruct;
import javax.transaction.Transactional;
import java.io.Serializable;
import java.util.function.Consumer;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Slf4j
@SpringAware
@Component
@Scope(SCOPE_PROTOTYPE)
public class InitiatorEndpointAssemblyTask extends NodeTask implements Serializable {
    private static final long serialVersionUID = -1822121694086325941L;

    @Autowired
    private transient GenericDao genericDao;

    @Autowired
    private transient EndpointService endpointService;

    private transient Consumer<Endpoint> initiatorEndpointAttacher;

    @Setter
    private ODataId fabricOdataId;

    @PostConstruct
    public void init() {
        this.initiatorEndpointAttacher = endpoint -> {
            val node = genericDao.find(ComposedNode.class, composedNodeODataId);
            val system = node.getComputerSystem();
            requiresNonNull(system, "computerSystem");

            endpoint.setAllocated(true);
            system.addEndpoint(endpoint);
            endpoint.getConnectedEntities().forEach(system::addEntityConnection);
        };
    }

    @Override
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() {
        Fabric fabric = genericDao.find(Fabric.class, fabricOdataId);
        try {
            val node = getComposedNode();
            val system = node.getComputerSystem();

            val initiatorEndpointOdataId = endpointService.createInitiatorEndpoint(
                fabricOdataId, fabric.getFabricType(), system.getUuid(), system.getPcieConnectionIds()
            );

            endpointService.discoverEndpoint(fabricOdataId, initiatorEndpointOdataId, initiatorEndpointAttacher);
        } catch (BusinessApiException e) {
            throw new RuntimeException(e);
        }
    }

    public ComposedNode getComposedNode() {
        return genericDao.find(ComposedNode.class, composedNodeODataId);
    }
}
