/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.business.redfish.services.actions;

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.PcieZoneAttachDetachEndpointRequest;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Objects;

import static com.intel.podm.common.utils.Contracts.requires;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class PcieZoneActionsInvoker {
    @Inject
    private Logger logger;

    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private PcieDriveActionsInvoker pcieDriveActionsInvoker;

    @Transactional(MANDATORY)
    public void attachEndpoint(Zone zone, Endpoint endpoint) throws EntityOperationException {
        ExternalService service = checkPreconditions(zone, endpoint);

        zone.addEndpoint(endpoint);
        updateZone(service, zone);
        updateDriveErased(endpoint);
    }

    @Transactional(MANDATORY)
    public void detachEndpoint(Zone zone, Endpoint endpoint) throws EntityOperationException {
        ExternalService service = checkPreconditions(zone, endpoint);

        zone.unlinkEndpoint(endpoint);
        updateZone(service, zone);
    }

    private ExternalService checkPreconditions(Zone zone, Endpoint endpoint) throws EntityOperationException {
        requires(zone != null, "Cannot perform action if zone is unspecified(null)");
        ExternalService zoneService = zone.getService();
        requires(zoneService != null, "There is no Service associated with selected Zone");
        confirmSameService(zoneService, endpoint.getService());
        return zoneService;
    }

    private void updateDriveErased(Endpoint endpoint) throws EntityOperationException {
        for (ConnectedEntity connectedEntity : endpoint.getConnectedEntities()) {
            DiscoverableEntity entity = connectedEntity.getEntityLink();
            if (entity instanceof Drive) {
                pcieDriveActionsInvoker.updateDriveErased((Drive) entity, false);
            }
        }
    }

    private void updateZone(ExternalService service, Zone zone) throws EntityOperationException {
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            webClient.patch(zone.getSourceUri(), new PcieZoneAttachDetachEndpointRequest(zone.getEndpoints().stream()
                .map(DiscoverableEntity::getSourceUri)
                .collect(toSet())));
        } catch (WebClientRequestException e) {
            String errorMessage = "Patch Zone failed: " + e.getMessage();
            logger.w("{} on [ service: {}, Zone: {}, Endpoints: {}]",
                errorMessage,
                service.getBaseUri(),
                zone.getSourceUri(),
                zone.getEndpoints());
            throw new EntityOperationException(errorMessage, e);
        }
    }

    private void confirmSameService(ExternalService firstService, ExternalService secondService) throws EntityOperationException {
        boolean allDevicesBelongToTheSameService = Objects.equals(firstService, secondService);
        if (!allDevicesBelongToTheSameService) {
            throw new EntityOperationException("Not all devices belong to the same service");
        }
    }
}
