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

import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.discovery.external.partial.StorageServiceVolumeObtainer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class ObtainNewVolumeTask extends NewVolumeTask {
    @Inject
    private Logger logger;

    @Inject
    private StorageServiceVolumeObtainer volumeObtainer;

    @Override
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() {
        try {
            ComposedNode composedNode = genericDao.find(ComposedNode.class, nodeId);
            StorageService storageService = getStorageServiceFromResourceDescriptor();
            URI relativeVolumeUri = URI.create(getNewRemoteVolumeUri().getPath());
            Volume volume = volumeObtainer.discoverStorageServiceVolume(storageService, relativeVolumeUri);
            volume.getMetadata().setAllocated(true);
            composedNode.addVolume(volume);
            composedNode.setAssociatedStorageServiceUuid(volume.getService().getUuid());
            composedNode.addAssociatedVolumeIdentifiers(volume.getIdentifiers());
        } catch (WebClientRequestException e) {
            logger.e("Volume obtain failed for Node: {}, details: {}", nodeId, e.getMessage());
            throw new RuntimeException(e);
        }
    }
}
