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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.dto.EndpointDto;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.business.redfish.services.EndpointCreationService;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.EntityRole;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.UUID;

import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.business.redfish.services.helpers.EndpointCreationRequestBuilder.buildInitiatorEndpointCreationRequest;
import static com.intel.podm.business.redfish.services.helpers.EndpointCreationRequestBuilder.buildTargetEndpointCreationRequest;
import static com.intel.podm.common.types.EntityRole.TARGET;
import static com.intel.podm.common.types.Protocol.ISCSI;
import static com.intel.podm.common.types.Protocol.NVME_OVER_FABRICS;
import static com.intel.podm.common.utils.IterableHelper.any;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class EndpointAssemblyTask extends NodeTask {
    @Inject
    private Logger logger;

    @Inject
    private GenericDao genericDao;

    @Inject
    private EndpointCreationService endpointCreationService;

    private Id volumeId;
    private EntityRole endpointRole;
    private Context context;

    @Override
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() {
        ComposedNode composedNode = getComposedNode();
        Volume volume = genericDao.find(Volume.class, volumeId);
        resolveContextDependency(volume);
        Fabric fabric = retrieveFabricFromVolume(volume);
        buildRequestAndCreateEndpoint(fabric, composedNode);
    }

    private void buildRequestAndCreateEndpoint(Fabric fabric, ComposedNode composedNode) {
        Endpoint endpoint;
        if (TARGET.equals(endpointRole)) {
            EndpointDto endpointDto = buildTargetEndpointCreationRequest(fabric, context);
            endpoint = createEndpoint(fabric, endpointDto);
            composedNode.addEndpoint(endpoint);
            composedNode.setAssociatedStorageServiceUuid(endpoint.getService().getUuid());
        } else {
            EndpointDto endpointDto = buildInitiatorEndpointCreationRequest(fabric, getComposedNode().getComputerSystem().getUuid());
            endpoint = createEndpoint(fabric, endpointDto);
            composedNode.getComputerSystem().addEndpoint(endpoint);
            updateEntityLink(composedNode, endpoint);
        }
    }

    private Endpoint createEndpoint(Fabric fabric, EndpointDto endpointDto) {
        try {
            Context endpointContext = endpointCreationService.createAndDiscoverEndpoint(toContext(fabric), endpointDto);
            Endpoint endpoint = genericDao.find(Endpoint.class, endpointContext.getId());
            endpoint.getMetadata().setAllocated(true);

            return endpoint;
        } catch (BusinessApiException e) {
            logger.e("Endpoint creation failed for ComposedNode: {}, details: {}", nodeId, e.getMessage());
            throw new RuntimeException(e);
        }
    }

    private void updateEntityLink(ComposedNode composedNode, Endpoint endpoint) {
        endpoint.getConnectedEntities()
            .forEach(connectedEntity -> connectedEntity.setEntityLink(composedNode.getComputerSystem()));
    }

    @Override
    @TimeMeasured(tag = "[AssemblyTask]")
    @Transactional(REQUIRES_NEW)
    public UUID getServiceUuid() {
        resolveVolumeDependency();
        return genericDao.find(Volume.class, volumeId).getService().getUuid();
    }

    private void resolveVolumeDependency() {
        if (volumeId == null) {
            ComposedNode composedNode = getComposedNode();
            volumeId = any(composedNode.getVolumes()).getId();
        }
    }

    private void resolveContextDependency(Volume volume) {
        if (context == null) {
            context = toContext(volume);
        }
    }

    public ComposedNode getComposedNode() {
        return genericDao.find(ComposedNode.class, nodeId);
    }

    public void setVolumeId(Id volumeId) {
        this.volumeId = volumeId;
    }

    public void setEndpointRole(EntityRole endpointRole) {
        this.endpointRole = endpointRole;
    }

    public void setContext(Context context) {
        this.context = context;
    }

    private Fabric retrieveFabricFromVolume(Volume volume) {
        return volume.getService().getOwnedEntities().stream()
            .filter(entity -> entity instanceof Fabric)
            .map(Fabric.class::cast)
            .filter(fabric -> NVME_OVER_FABRICS.equals(fabric.getFabricType()) || ISCSI.equals(fabric.getFabricType()))
            .findFirst()
            .orElseThrow(() -> new RuntimeException("Only assembly with Volume from NvmeOverFabrics or iSCSI Fabric allowed"));
    }
}
