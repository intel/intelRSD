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

package com.intel.rsd.nodecomposer.discovery.external.partial;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.discovery.external.EntityMultiMapper;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.CapacitySourcesResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.StoragePoolResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.VolumeResource;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.CapacitySource;
import com.intel.rsd.nodecomposer.persistence.redfish.StoragePool;
import com.intel.rsd.nodecomposer.persistence.redfish.StorageService;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import org.springframework.transaction.annotation.Transactional;

import java.util.HashSet;
import java.util.Set;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static org.springframework.transaction.annotation.Propagation.MANDATORY;

@Component
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
// TODO remove fan-out, clean up class
public class VolumeSynchronizationService {

    private final WebClientBuilder webClientBuilder;
    private final EntityMultiMapper entityMultiMapper;
    private final GenericDao genericDao;

    @Autowired
    public VolumeSynchronizationService(WebClientBuilder webClientBuilder, EntityMultiMapper entityMultiMapper, GenericDao genericDao) {
        this.webClientBuilder = webClientBuilder;
        this.entityMultiMapper = entityMultiMapper;
        this.genericDao = genericDao;
    }

    @Transactional(propagation = MANDATORY)
    public Volume discoverVolume(ODataId storageServiceODataId, ODataId volumeOdataId) throws BusinessApiException {
        requiresNonNull(storageServiceODataId, "storageServiceODataId");
        requiresNonNull(volumeOdataId, "volumeOdataId");

        try (val webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            val volumeResource = (VolumeResource) webClient.get(volumeOdataId.toUri());
            val volume = (Volume) entityMultiMapper.map(volumeResource);
            val storageService = genericDao.find(StorageService.class, storageServiceODataId);
            storageService.addVolume(volume);
            mapCapacitySources(volume, volumeResource.getCapacitySources(), storageService);
            return volume;
        } catch (WebClientRequestException e) {
            throw new BusinessApiException(format("Cannot discover Volume(%s) on StorageService(%s)", volumeOdataId, storageServiceODataId), e);
        }
    }

    private void mapCapacitySources(Volume volume, Iterable<ResourceSupplier> capacitySources, StorageService storageService) throws WebClientRequestException {
        for (ResourceSupplier capacitySourceResourceSupplier : capacitySources) {
            val capacitySourcesResource = (CapacitySourcesResource) capacitySourceResourceSupplier.get();
            val capacitySource = (CapacitySource) entityMultiMapper.map(capacitySourcesResource);
            volume.addCapacitySource(capacitySource);

            for (ResourceSupplier storagePoolResourceSupplier : capacitySourcesResource.getProvidingPools()) {
                val storagePoolResource = (StoragePoolResource) storagePoolResourceSupplier.get();
                val storagePool = (StoragePool) entityMultiMapper.map(storagePoolResource);
                storageService.addStoragePool(storagePool);

                val previouslyAllocatedVolumes = new HashSet<>(storagePool.getAllocatedVolumes());
                val currentlyAllocatedVolumes = new HashSet<>(getAllocatedVolumes(storagePoolResource, storageService));
                updateAllocatedVolumes(storagePool, previouslyAllocatedVolumes, currentlyAllocatedVolumes);

                capacitySource.addProvidingPool(storagePool);
            }
        }
    }

    private Set<Volume> getAllocatedVolumes(StoragePoolResource storagePoolResource, StorageService storageService) throws WebClientRequestException {
        val allocatedVolumes = new HashSet<Volume>();
        for (ResourceSupplier allocatedVolumeResourceSupplier : storagePoolResource.getAllocatedVolumes()) {
            val volumeResource = (VolumeResource) allocatedVolumeResourceSupplier.get();
            val volume = (Volume) entityMultiMapper.map(volumeResource);
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
}
