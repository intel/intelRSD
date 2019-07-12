/*
 * Copyright (c) 2016-2019 Intel Corporation
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
import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.SecureEraseInvoker;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.actions.ZoneActionJson;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Zone;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.io.Serializable;

import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@SpringAware
@Component
@Scope(SCOPE_PROTOTYPE)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
@Slf4j
public class DetachEndpointFromZoneTask extends NodeTask implements Serializable {
    private static final long serialVersionUID = 7126741623292947914L;

    @Autowired
    private transient GenericDao genericDao;
    @Autowired
    private transient WebClientBuilder webClientBuilder;
    @Autowired
    private transient SecureEraseInvoker secureEraseInvoker;

    @Setter
    private ODataId zoneODataId;
    @Setter
    private ODataId endpointODataId;

    @Override
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() {
        Zone zone = genericDao.find(Zone.class, zoneODataId);
        Endpoint endpoint = genericDao.find(Endpoint.class, endpointODataId);

        try {
            zone.unlinkEndpoint(endpoint);
            updateZone(zone);
            updateErasedFlag(endpoint);
        } catch (EntityOperationException e) {
            throw new RuntimeException("Detaching PCIe endpoint from PCIe zone failed", e);
        }
    }

    private void updateErasedFlag(Endpoint endpoint) throws EntityOperationException {
        for (ConnectedEntity connectedEntity : endpoint.getConnectedEntities()) {
            DiscoverableEntity entity = connectedEntity.getEntityLink();
            if (entity instanceof Drive) {
                secureEraseInvoker.updateErased(entity, false);
            }
        }
    }

    private void updateZone(Zone zone) throws EntityOperationException {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            webClient.patch(zone.getUri().toUri(), new ZoneActionJson(zone.getEndpoints().stream()
                .map(DiscoverableEntity::getUri)
                .map(ODataId::toUri)
                .collect(toSet())));
        } catch (WebClientRequestException e) {
            String errorMessage = "Patch Zone failed: " + e.getMessage();
            log.warn("{} on [ Zone: {}, Endpoints: {}]",
                errorMessage,
                zone.getUri(),
                zone.getEndpoints());
            throw new EntityOperationException(errorMessage, e);
        }
    }
}
