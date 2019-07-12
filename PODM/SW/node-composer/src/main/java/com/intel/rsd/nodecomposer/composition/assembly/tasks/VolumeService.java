/*
 * Copyright (c) 2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.redfish.services.StateService;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.RemoteDriveAllocationContextDescriptor;
import com.intel.rsd.nodecomposer.discovery.external.partial.VolumeSynchronizationService;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import org.springframework.transaction.annotation.Transactional;

import java.util.function.Consumer;

import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromUri;
import static org.springframework.transaction.annotation.Propagation.REQUIRES_NEW;

@Component
public class VolumeService {

    private final VolumeSynchronizationService volumeSynchronizationService;

    private final VolumeCreationService volumeCreationService;

    private final StateService stateService;

    @Autowired
    public VolumeService(VolumeSynchronizationService volumeSynchronizationService, VolumeCreationService volumeCreationService, StateService stateService) {
        this.volumeSynchronizationService = volumeSynchronizationService;
        this.volumeCreationService = volumeCreationService;
        this.stateService = stateService;
    }

    @Transactional(propagation = REQUIRES_NEW, rollbackFor = BusinessApiException.class)
    public void discoverCreatedVolume(ODataId storageServiceODataId,
                                      ODataId volumeOdataId,
                                      Consumer<AttachableAsset> onAfterDiscoveryCallback) throws BusinessApiException {

        val discoveredVolume = volumeSynchronizationService.discoverVolume(storageServiceODataId, volumeOdataId);
        onAfterDiscoveryCallback.accept(discoveredVolume);
    }

    public ODataId createVolumeAndWaitUntilEnabled(RemoteDriveAllocationContextDescriptor resourceDescriptor) {
        val createdVolumeOdataId = volumeCreationService.createVolume(resourceDescriptor);
        stateService.waitForEnabledStateOnVolume(createdVolumeOdataId);
        return oDataIdFromUri(createdVolumeOdataId);
    }
}
