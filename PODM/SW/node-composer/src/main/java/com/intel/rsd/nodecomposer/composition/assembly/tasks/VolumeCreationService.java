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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.intel.rsd.nodecomposer.composition.allocation.strategy.RemoteDriveAllocationContextDescriptor;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.actions.StorageServiceVolumeCreationRequest;
import com.intel.rsd.nodecomposer.externalservices.actions.StorageServiceVolumeCreationRequest.CapacitySourceRequest;
import com.intel.rsd.nodecomposer.externalservices.actions.StorageServiceVolumeCreationRequest.ReplicaInfoRequest;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.net.URI;

import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.VOLUMES_RESOURCE_NAME;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static com.intel.rsd.nodecomposer.utils.Converters.convertGibToBytes;
import static java.lang.Boolean.TRUE;
import static java.util.Collections.singletonList;
import static org.springframework.web.util.UriComponentsBuilder.fromUri;

@Component
public class VolumeCreationService {

    private final WebClientBuilder webClientBuilder;

    @Autowired
    public VolumeCreationService(WebClientBuilder webClientBuilder) {
        this.webClientBuilder = webClientBuilder;
    }

    public URI createVolume(RemoteDriveAllocationContextDescriptor resourceDescriptor) {
        requiresNonNull(resourceDescriptor, "ResourceDescriptor");
        try (val webClient = webClientBuilder.createResourceManagerInstance().build()) {
            val volumeUri = webClient.post(getTargetUri(resourceDescriptor.getStorageServiceODataId().toUri()), buildVolumeRequest(resourceDescriptor));
            requiresNonNull(volumeUri, "URI after create new Volume");
            return volumeUri;
        } catch (WebClientRequestException e) {
            throw new RuntimeException("Volume Creation Failed!", e);
        }
    }

    private URI getTargetUri(URI uri) {
        return fromUri(uri).pathSegment(VOLUMES_RESOURCE_NAME).build().toUri();
    }

    private StorageServiceVolumeCreationRequest buildVolumeRequest(RemoteDriveAllocationContextDescriptor resourceDescriptor) {
        val capacityInBytes = convertGibToBytes(resourceDescriptor.getCapacity());
        val creationRequest = new StorageServiceVolumeCreationRequest(capacityInBytes);
        if (resourceDescriptor.getMasterUri() != null) {
            val replicaInfo = new ReplicaInfoRequest();
            replicaInfo.setReplicaType(resourceDescriptor.getReplicaType());
            replicaInfo.setReplica(resourceDescriptor.getMasterUri());
            creationRequest.setReplicaInfos(singletonList(replicaInfo));
            creationRequest.setBootable(TRUE);
        } else {
            val capacity = new CapacitySourceRequest();
            capacity.setProvidingPools(singletonList(resourceDescriptor.getStoragePoolODataId()));
            creationRequest.setCapacitySources(singletonList(capacity));
        }
        return creationRequest;
    }

}
