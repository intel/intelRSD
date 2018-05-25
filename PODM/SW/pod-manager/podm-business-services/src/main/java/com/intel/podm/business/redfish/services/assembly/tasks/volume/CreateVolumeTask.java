/*
 * Copyright (c) 2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.assembly.tasks.volume;

import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.StorageServiceVolumeCreationRequest;
import com.intel.podm.client.actions.StorageServiceVolumeCreationRequest.CapacitySourceRequest;
import com.intel.podm.client.actions.StorageServiceVolumeCreationRequest.ReplicaInfoRequest;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import javax.ws.rs.core.UriBuilder;
import java.math.BigDecimal;
import java.net.URI;

import static com.intel.podm.common.types.redfish.ResourceNames.VOLUMES_RESOURCE_NAME;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static com.intel.podm.common.utils.Converters.convertGibToBytes;
import static java.lang.Boolean.TRUE;
import static java.util.Collections.singletonList;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class CreateVolumeTask extends NewVolumeTask {

    @Inject
    private WebClientBuilder webClientBuilder;

    @Override
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() {
        requiresNonNull(resourceDescriptor, "ResourceDescriptor");
        StorageService storageService = getStorageServiceFromResourceDescriptor();
        try (WebClient webClient = webClientBuilder.newInstance(storageService.getService().getBaseUri()).retryable().build()) {
            URI volumeUri = webClient.post(getTargetUri(storageService), buildVolumeRequest());
            requiresNonNull(volumeUri, "URI after create new Volume");
            resourceDescriptor.setNewRemoteVolumeUri(volumeUri);
        } catch (WebClientRequestException e) {
            throw new RuntimeException("Volume Creation Failed!", e);
        }
    }

    private URI getTargetUri(StorageService storageService) {
        return UriBuilder.fromUri(storageService.getSourceUri())
            .path(VOLUMES_RESOURCE_NAME)
            .build();
    }

    private StorageServiceVolumeCreationRequest buildVolumeRequest() {
        BigDecimal capacityInBytes = convertGibToBytes(resourceDescriptor.getCapacity());
        StorageServiceVolumeCreationRequest creationRequest = new StorageServiceVolumeCreationRequest(capacityInBytes);
        if (resourceDescriptor.getMasterUri() != null) {
            ReplicaInfoRequest replicaInfo = new ReplicaInfoRequest();
            replicaInfo.setReplicaType(resourceDescriptor.getReplicaType());
            replicaInfo.setReplica(resourceDescriptor.getMasterUri());
            creationRequest.setReplicaInfos(singletonList(replicaInfo));
            creationRequest.setBootable(TRUE);
        } else {
            CapacitySourceRequest capacity = new CapacitySourceRequest();
            capacity.setProvidingPools(singletonList(new ODataId(resourceDescriptor.getStoragePoolUri())));
            creationRequest.setCapacitySources(singletonList(capacity));
        }
        return creationRequest;
    }
}
