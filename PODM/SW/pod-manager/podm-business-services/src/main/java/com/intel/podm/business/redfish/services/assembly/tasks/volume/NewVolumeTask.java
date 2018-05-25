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

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.redfish.services.allocation.strategy.RemoteDriveAllocationContextDescriptor;
import com.intel.podm.business.redfish.services.assembly.tasks.NodeTask;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.UUID;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

public abstract class NewVolumeTask extends NodeTask {

    protected RemoteDriveAllocationContextDescriptor resourceDescriptor;
    @Inject
    protected GenericDao genericDao;

    @Override
    @Transactional(REQUIRES_NEW)
    public UUID getServiceUuid() {
        StorageService storageService = getStorageServiceFromResourceDescriptor();
        return storageService.getService().getUuid();
    }

    public void setResourceDescriptor(RemoteDriveAllocationContextDescriptor resourceDescriptor) {
        this.resourceDescriptor = resourceDescriptor;
    }

    protected StorageService getStorageServiceFromResourceDescriptor() {
        requiresNonNull(resourceDescriptor, "ResourceDescriptor");
        return genericDao.find(StorageService.class, resourceDescriptor.getStorageServiceId());
    }

    protected URI getNewRemoteVolumeUri() {
        requiresNonNull(resourceDescriptor, "ResourceDescriptor");
        requiresNonNull(resourceDescriptor.getNewRemoteVolumeUri(), "NewRemoteVolumeUri on ResourceDescriptor");
        return resourceDescriptor.getNewRemoteVolumeUri();
    }
}
