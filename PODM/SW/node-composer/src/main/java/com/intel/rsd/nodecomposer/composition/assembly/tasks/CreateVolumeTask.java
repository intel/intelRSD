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

import com.hazelcast.spring.context.SpringAware;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.RemoteDriveAllocationContextDescriptor;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.actions.StorageServiceVolumeCreationRequest;
import com.intel.rsd.nodecomposer.externalservices.actions.StorageServiceVolumeCreationRequest.CapacitySourceRequest;
import com.intel.rsd.nodecomposer.externalservices.actions.StorageServiceVolumeCreationRequest.ReplicaInfoRequest;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.io.Serializable;
import java.math.BigDecimal;
import java.net.URI;

import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.VOLUMES_RESOURCE_NAME;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static com.intel.rsd.nodecomposer.utils.Converters.convertGibToBytes;
import static java.lang.Boolean.TRUE;
import static java.util.Collections.singletonList;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;
import static org.springframework.web.util.UriComponentsBuilder.fromUri;

@SpringAware
@Component
@Scope(SCOPE_PROTOTYPE)
public class CreateVolumeTask implements Serializable {
    private static final long serialVersionUID = 1093574730108977240L;

    @Autowired
    private transient WebClientBuilder webClientBuilder;

    @TimeMeasured(tag = "[AssemblyTask]")
    public URI createVolumeAndReturnItsUri(RemoteDriveAllocationContextDescriptor resourceDescriptor) {
        requiresNonNull(resourceDescriptor, "ResourceDescriptor");
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().build()) {
            URI volumeUri = webClient.post(getTargetUri(resourceDescriptor.getStorageServiceODataId().toUri()), buildVolumeRequest(resourceDescriptor));
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
            capacity.setProvidingPools(singletonList(resourceDescriptor.getStoragePoolODataId()));
            creationRequest.setCapacitySources(singletonList(capacity));
        }
        return creationRequest;
    }

}
