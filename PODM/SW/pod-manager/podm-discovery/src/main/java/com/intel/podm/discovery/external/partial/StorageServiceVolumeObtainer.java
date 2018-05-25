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

package com.intel.podm.discovery.external.partial;

import com.intel.podm.business.entities.redfish.CapacitySource;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.ReplicaInfo;
import com.intel.podm.business.entities.redfish.StoragePool;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.redfish.CapacitySourcesResource;
import com.intel.podm.client.resources.redfish.ReplicaInfoResource;
import com.intel.podm.client.resources.redfish.StoragePoolResource;
import com.intel.podm.client.resources.redfish.VolumeResource;
import com.intel.podm.discovery.external.EntityMultiMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
// TODO remove fan-out, clean up class
public class StorageServiceVolumeObtainer {
    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private EntityMultiMapper entityMultiMapper;

    @Transactional(MANDATORY)
    public Volume discoverStorageServiceVolume(StorageService storageService, URI volumeUri) throws WebClientRequestException {
        requiresNonNull(volumeUri, "volumeUri", "There is no Volume to discover");

        ExternalService externalService = storageService.getService();
        requiresNonNull(externalService, "service", "There is no Service associated with selected volume");

        try (WebClient webClient = webClientBuilder.newInstance(externalService.getBaseUri()).retryable().build()) {
            VolumeResource volumeResource = (VolumeResource) webClient.get(volumeUri);

            Volume volume = (Volume) entityMultiMapper.map(volumeResource, externalService);
            storageService.addVolume(volume);

            mapCapacitySources(externalService, volume, volumeResource.getCapacitySources(), storageService);
            mapReplicaInfos(externalService, volume, volumeResource.getReplicaInfos(), storageService);

            return volume;
        }
    }

    private void mapCapacitySources(ExternalService externalService, Volume volume, Iterable<ResourceSupplier> capacitySources, StorageService storageService)
        throws WebClientRequestException {
        for (ResourceSupplier capacitySourceResourceSupplier : capacitySources) {
            CapacitySourcesResource capacitySourcesResource = (CapacitySourcesResource) capacitySourceResourceSupplier.get();
            CapacitySource capacitySource = (CapacitySource) entityMultiMapper.map(capacitySourcesResource, externalService);
            volume.addCapacitySource(capacitySource);

            for (ResourceSupplier storagePoolResourceSupplier : capacitySourcesResource.getProvidingPools()) {
                StoragePoolResource storagePoolResource = (StoragePoolResource) storagePoolResourceSupplier.get();
                StoragePool storagePool = (StoragePool) entityMultiMapper.map(storagePoolResource, externalService);
                storageService.addStoragePool(storagePool);

                Set<Volume> previouslyAllocatedVolumes = new HashSet<>(storagePool.getAllocatedVolumes());
                Set<Volume> currentlyAllocatedVolumes = new HashSet<>(getAllocatedVolumes(externalService, storagePoolResource, storageService));
                updateAllocatedVolumes(storagePool, previouslyAllocatedVolumes, currentlyAllocatedVolumes);

                capacitySource.addProvidingPool(storagePool);
            }
        }
    }

    private Set<Volume> getAllocatedVolumes(ExternalService externalService, StoragePoolResource storagePoolResource, StorageService storageService)
        throws WebClientRequestException {
        Set<Volume> allocatedVolumes = new HashSet<>();
        for (ResourceSupplier allocatedVolumeResourceSupplier : storagePoolResource.getAllocatedVolumes()) {
            VolumeResource volumeResource = (VolumeResource) allocatedVolumeResourceSupplier.get();
            Volume volume = (Volume) entityMultiMapper.map(volumeResource, externalService);
            storageService.addVolume(volume);
            allocatedVolumes.add(volume);

        }
        return allocatedVolumes;
    }

    private void updateAllocatedVolumes(StoragePool storagePool, Set<Volume> previouslyAllocatedVolumes, Set<Volume> currentlyAllocatedVolumes) {
        for (Volume previouslyAllocatedVolume : previouslyAllocatedVolumes) {
            if (!currentlyAllocatedVolumes.contains(previouslyAllocatedVolume)) {
                storagePool.unlinkAllocatedVolume(previouslyAllocatedVolume);
            }
        }

        for (Volume allocatedVolume : currentlyAllocatedVolumes) {
            if (!previouslyAllocatedVolumes.contains(allocatedVolume)) {
                storagePool.addAllocatedVolume(allocatedVolume);
            }
        }
    }

    private void mapReplicaInfos(ExternalService externalService, Volume volume, Iterable<ResourceSupplier> replicaInfos, StorageService storageService)
        throws WebClientRequestException {
        for (ResourceSupplier replicaInfoResourceSupplier : replicaInfos) {
            ReplicaInfoResource replicaInfoResource = (ReplicaInfoResource) replicaInfoResourceSupplier.get();
            ReplicaInfo replicaInfo = (ReplicaInfo) entityMultiMapper.map(replicaInfoResource, externalService);

            ResourceSupplier replicaSupplier = replicaInfoResource.getReplica();
            if (replicaSupplier != null) {
                Volume replicaVolume = getReplicaVolume(externalService, volume, replicaSupplier, true, storageService);
                replicaInfo.setReplica(replicaVolume);
            }
            volume.addReplicaInfo(replicaInfo);
        }
    }

    private Volume getReplicaVolume(ExternalService externalService, Volume volume, ResourceSupplier replicaSupplier, boolean discoverReplicas,
                                    StorageService storageService)
        throws WebClientRequestException {
        if (Objects.equals(replicaSupplier.getUri(), volume.getSourceUri())) {
            return volume;
        }

        VolumeResource volumeReplicaResource = (VolumeResource) replicaSupplier.get();
        Volume volumeReplica = (Volume) entityMultiMapper.map(volumeReplicaResource, externalService);
        storageService.addVolume(volumeReplica);

        if (discoverReplicas) {
            for (ResourceSupplier replicaInfoResourceSupplier : volumeReplicaResource.getReplicaInfos()) {
                ReplicaInfoResource replicaInfoResource = (ReplicaInfoResource) replicaInfoResourceSupplier.get();
                ReplicaInfo replicaInfo = (ReplicaInfo) entityMultiMapper.map(replicaInfoResource, externalService);

                ResourceSupplier innerReplica = replicaInfoResource.getReplica();
                if (innerReplica != null) {
                    Volume replicaVolume = getReplicaVolume(externalService, volume, innerReplica, false, storageService);
                    replicaInfo.setReplica(replicaVolume);
                }
                volumeReplica.addReplicaInfo(replicaInfo);
            }
        }

        return volumeReplica;
    }
}
