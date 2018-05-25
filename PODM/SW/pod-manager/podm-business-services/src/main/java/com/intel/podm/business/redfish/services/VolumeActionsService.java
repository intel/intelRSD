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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.StoragePool;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.VolumeActionsInvoker;
import com.intel.podm.business.redfish.services.helpers.VolumeHelper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.client.actions.StorageServiceVolumeCreationRequest;
import com.intel.podm.client.actions.StorageServiceVolumeCreationRequest.CapacitySourceRequest;
import com.intel.podm.client.actions.StorageServiceVolumeCreationRequest.ReplicaInfoRequest;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.redfish.OdataIdProvider;
import com.intel.podm.common.types.redfish.StorageServiceVolume;
import com.intel.podm.common.types.redfish.StorageServiceVolume.CapacitySource;
import com.intel.podm.common.types.redfish.StorageServiceVolume.ReplicaInfo;
import com.intel.podm.common.utils.Collections;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import static com.intel.podm.business.Violations.createWithViolations;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.business.services.context.ContextType.STORAGE_POOL;
import static com.intel.podm.business.services.context.ContextType.VOLUME;
import static com.intel.podm.business.services.context.UriToContextConverter.getContextFromUri;
import static com.intel.podm.common.types.Protocol.ISCSI;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class VolumeActionsService {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private VolumeActionsInvoker invoker;

    @Inject
    private VolumeAttacher volumeAttacher;

    @Inject
    private VolumeDetachService volumeDetachService;

    @Inject
    private VolumeHelper volumeHelper;

    @Transactional(REQUIRES_NEW)
    public Context create(Context creationalContext, StorageServiceVolume representation) throws BusinessApiException {
        StorageService storageService = (StorageService) traverser.traverse(creationalContext);
        StorageServiceVolumeCreationRequest request = buildRequest(representation, storageService);

        Volume createdVolume = invoker.createVolume(storageService, request);
        return toContext(createdVolume);
    }

    @Transactional(REQUIRES_NEW)
    void deleteVolume(Context context) throws EntityOperationException, ContextResolvingException {
        Volume volume = (Volume) traverser.traverse(context);
        invoker.deleteVolume(volume);
    }

    @Transactional(REQUIRES_NEW)
    public void attachVolume(Context target, Context resourceContext) throws BusinessApiException {
        Volume volume = (Volume) traverser.traverse(resourceContext);
        ComposedNode composedNode = (ComposedNode) traverser.traverse(target);

        volumeAttacher.attachVolume(composedNode, volume);
    }

    @Transactional(REQUIRES_NEW)
    public void detachVolume(Context composedNodeContext, Context volumeContext) throws BusinessApiException {
        Volume volume = (Volume) traverser.traverse(volumeContext);
        ComposedNode composedNode = (ComposedNode) traverser.traverse(composedNodeContext);
        validateDetachVolume(volume, composedNode);
        volumeDetachService.detachVolume(composedNode, volume);
    }

    private void validateDetachVolume(Volume volume, ComposedNode composedNode) throws RequestValidationException {
        if (!composedNode.getVolumes().contains(volume)) {
            throw new RequestValidationException(createWithViolations("Provided volume is not attached to this Node."));
        }
        if (ISCSI.equals(volumeHelper.retrieveProtocolFromVolume(volume))) {
            throw new RequestValidationException(createWithViolations("Detach volume with iSCSI protocol is not allowed."));
        }
    }

    private StorageServiceVolumeCreationRequest buildRequest(StorageServiceVolume representation, StorageService storageService)
        throws RequestValidationException {
        StorageServiceVolumeCreationRequest creationRequest = new StorageServiceVolumeCreationRequest(representation.getCapacityBytes());

        creationRequest.setName(representation.getName());
        creationRequest.setDescription(representation.getDescription());

        if (representation.getCapacitySources() != null) {
            creationRequest.setCapacitySources(buildCapacitySources(representation.getCapacitySources()));
        }

        creationRequest.setAccessCapabilities(representation.getAccessCapabilities());

        if (!Collections.nullOrEmpty(representation.getReplicaInfos())) {
            creationRequest.setReplicaInfos(buildReplicaInfos(representation.getReplicaInfos()));
        }

        return creationRequest;
    }

    private List<CapacitySourceRequest> buildCapacitySources(List<? extends CapacitySource> capacitySources) throws RequestValidationException {
        List<CapacitySourceRequest> capacitySourceRequestList = new ArrayList<>();

        for (CapacitySource capacitySource : capacitySources) {
            capacitySourceRequestList.add(buildCapacitySourceRequest(capacitySource));
        }

        return capacitySourceRequestList;
    }

    private CapacitySourceRequest buildCapacitySourceRequest(CapacitySource capacitySource) throws RequestValidationException {
        CapacitySourceRequest capacitySourceRequest = new CapacitySourceRequest();

        if (capacitySource.getProvidingPools() != null) {
            capacitySourceRequest.setProvidingPools(buildProvidingPools(capacitySource.getProvidingPools()));
        }

        return capacitySourceRequest;
    }

    private List<ODataId> buildProvidingPools(Set<? extends OdataIdProvider.ODataId> sourceProvidingPools) throws RequestValidationException {
        List<ODataId> targetProvidingPools = new ArrayList<>();

        for (OdataIdProvider.ODataId oDataId : sourceProvidingPools) {
            URI sourceUri = getProvidingPoolSourceUri(oDataId);
            targetProvidingPools.add(new ODataId(sourceUri));
        }

        return targetProvidingPools;
    }

    private URI getProvidingPoolSourceUri(OdataIdProvider.ODataId providingPool) throws RequestValidationException {
        Context providingPoolContext = getContextFromUri(providingPool.toUri(), STORAGE_POOL);

        StoragePool storagePool = (StoragePool) traverser.tryTraverse(providingPoolContext).orElseThrow(() ->
            new RequestValidationException(createWithViolations("Selected StoragePool does not exist")));

        return storagePool.getSourceUri();
    }

    private List<ReplicaInfoRequest> buildReplicaInfos(List<? extends ReplicaInfo> replicaInfos) throws RequestValidationException {
        List<ReplicaInfoRequest> replicaInfoRequestList = new ArrayList<>();

        for (ReplicaInfo replicaInfo : replicaInfos) {
            URI replicaSourceUri = getReplicaSourceUri(replicaInfo);

            ReplicaInfoRequest replicaInfoRequest = new ReplicaInfoRequest();
            replicaInfoRequest.setReplicaType(replicaInfo.getReplicaType());
            replicaInfoRequest.setReplica(replicaSourceUri);

            replicaInfoRequestList.add(replicaInfoRequest);
        }

        return replicaInfoRequestList;
    }

    private URI getReplicaSourceUri(ReplicaInfo replicaInfo) throws RequestValidationException {
        OdataIdProvider.ODataId replica = replicaInfo.getReplica();
        Context replicaContext = getContextFromUri(replica.toUri(), VOLUME);

        Volume volume = (Volume) traverser.tryTraverse(replicaContext).orElseThrow(() ->
            new RequestValidationException(createWithViolations("Selected Volume does not exist")));

        return volume.getSourceUri();
    }
}
